#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Ultrasonic.h>

// Sensors ------
const int trigPin = D1;
const int echoPin = D2;
const int photores = A0;

int distance;
int value;
int photoThres = 500; // photores  sensor threshold
int proxiThres = 20;  // proximity sensor threshold
Ultrasonic usensor (trigPin, echoPin);

// RFID -------
#define SS_PIN D8
#define RST_PIN D0
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Communication code --------
// REPLACE WITH Main MAC Address
uint8_t broadcastAddress[] = {0x08, 0xB6, 0x1F, 0x81, 0x0F, 0xEC};

// Updates sensor readings every 1 seconds
const long interval = 10000; 
unsigned long previousMillis = 0; 


// payment interval
const long payinter = 30000;
unsigned long landingtime = 0;
bool trigger = false;
bool paid = true;

// Structure example to send data
// Must match the receiver structure
typedef struct SubData {
  int id;
  int state;
  bool reserved;
  uint8_t mac[];
}SubData;

// Create a SubData called myData
SubData myData;  // incoming data
SubData boardData = { 1, 0, false, {0x08, 0xB6, 0x1F, 0x81, 0x09, 0xF4}}; // mac adr is this boards


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // create mac string so we can serial print who we send data to
  char macStr[18];
  Serial.print("Packet to:");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status: ");

  // show if the delivery was succesful
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  boardData.reserved = myData.reserved;
}

bool checkForCard(){
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    Serial.println("Card detected! ");
    return true;
  }
  else{
    return false;
  }
}

bool readDistance() {
  distance = usensor.read();
  if (isnan(distance))    
    return false;
  if (distance < proxiThres){
    return true;
  }
  return false;
}

bool readPhotores() {
  value = analogRead(photores);
  if (isnan(value))    
    return false;
  if (value < photoThres){
    return true;
  }
  return false;
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // RFID Init
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  delay(10);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) { // Init ESP-NOW
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Set ESP-NOW Role
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0); // Register peer
  esp_now_register_recv_cb(OnDataRecv); // Register for a callback function that will be called when data is received

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    //Get data readings
    if (readDistance() && readPhotores()){
      boardData.state = 1;
      if (!trigger) {
        // Makes the timer only start ones
        trigger = true;
        landingtime = currentMillis;
      }
      else if (landingtime - previousMillis >= payinter && !paid){
        // not paid in time
        boardData.state = 3;
      }
    }
    else if (readDistance() && !readPhotores()){
      // one sensor failed, call support
      boardData.state = 5;
      paid = false;
      trigger = false;
      Serial.println("Error on the sensors!");
      Serial.println(distance);
      Serial.println(value);
    }

    else if (!readDistance() && readPhotores()){
      // one sensor failed, call support
      boardData.state = 5;
      paid = false;
      trigger = false;
      Serial.println("Error on the sensors!");
      Serial.println(distance);
      Serial.println(value);
    }

    else {
      // car has left, reset to 0
      boardData.state = 0;
      paid = false;
      trigger = false;
      }



    // Set values to send
    boardData.reserved = false; // recieved from main

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &boardData, sizeof(boardData));
  }

  if (checkForCard()){ 
    paid = true;
  }
}
