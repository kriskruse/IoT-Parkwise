/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/?s=esp-now
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based on JC Servaye example: https://https://github.com/Servayejc/esp8266_espnow
*/

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

// payment interval
const long payinter = 30000;
unsigned long landingtime = 0;
bool trigger = false;
bool paid = true;

// Updates sensor readings every 1 seconds
const long interval = 1000; 
unsigned long previousMillisSens = 0; 

// Communication ------
uint8_t channel = 1;
int readingId = 0;
int id = 1;

unsigned long currentMillis = millis(); 
unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

uint8_t broadcastAddressX[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

enum PairingStatus {PAIR_REQUEST, PAIR_REQUESTED, PAIR_PAIRED, };
PairingStatus pairingStatus = PAIR_REQUEST;

enum MessageType {PAIRING, DATA,};
MessageType messageType;

// Define variables to store DHT readings to be sent
int state;
bool reserved;

// Define variables to store incoming readings
int incomingState;
bool incomingReserved;
int incomingReadingsId;

// Updates DHT readings every 10 seconds
//const long interval = 10000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  uint8_t msgType;
  uint8_t id;
  int state;
  bool reserved;
  unsigned int readingId;
} struct_message;

typedef struct struct_pairing {       // new structure for pairing
    uint8_t msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
} struct_pairing;

// Create a struct_message called myData
struct_message myData;
struct_message incomingReadings;
struct_pairing pairingData;

#define BOARD_ID 1
unsigned long start;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

void printIncomingReadings(){
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("State: ");
  Serial.println(incomingState);
  Serial.print("Reserved: ");
  Serial.println(incomingReserved);
  Serial.print("Led: ");
  Serial.println(incomingReadingsId);
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  Serial.print("Size of message : ");
  Serial.print(len);
  Serial.print(" from ");
  printMAC(mac);
  Serial.println();
  uint8_t type = incomingData[0];
  switch (type) {
  case DATA :  
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    Serial.print(len);
    Serial.print(" Data bytes received from: ");
    printMAC(mac);
    Serial.println();
    //incomingState = incomingReadings.state;
    incomingReserved = incomingReadings.reserved;
    incomingReadingsId = incomingReadings.readingId;
    printIncomingReadings();
    
    /* if (incomingReadings.readingId % 2 == 1){
      digitalWrite(LED_BUILTIN, LOW);
    } else { 
      digitalWrite(LED_BUILTIN, HIGH);
    } */
    break;

  case PAIRING:
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    if (pairingData.id == 0) {                // the message comes from server
      Serial.print("Pairing done for ");
      printMAC(pairingData.macAddr);
      Serial.print(" on channel " );
      Serial.print(pairingData.channel);    // channel used by the server
      Serial.print(" in ");
      Serial.print(millis()-start);
      Serial.println("ms");
      //esp_now_del_peer(pairingData.macAddr);
      //esp_now_del_peer(mac);
      esp_now_add_peer(pairingData.macAddr, ESP_NOW_ROLE_COMBO, pairingData.channel, NULL, 0); // add the server to the peer list 
      pairingStatus = PAIR_PAIRED ;            // set the pairing status
    }
    break;
  }  
}

void getReadings(){
  state = getState();

  /* Serial.print(readDistance());
  Serial.print("  ");
  Serial.println(readPhotores()); */
  
}

int getState(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisSens >= interval) {
    // save the last time you updated the DHT values
    previousMillisSens = currentMillis;

    //Get data readings
    if (readDistance() && readPhotores()){
      if (!trigger) {
        // Makes the timer only start ones
        trigger = true;
        paid = false;
        return 3;
      }
      if (paid){
        return 1;
      }else {
        return 3;
      }
    }
    else if (readDistance() && !readPhotores()){
      // one sensor failed, call support
      paid = false;
      trigger = false;
      Serial.println("Error on the Photoresister sensors!");
      Serial.println(distance);
      Serial.println(value);
      return 5;
    }

    else if (!readDistance() && readPhotores()){
      // one sensor failed, call support
      paid = false;
      trigger = false;
      Serial.println("Error on the Distance sensors!");
      Serial.println(distance);
      Serial.println(value);
      return 5;
    }

    else {
      // car has left, reset to 0
      paid = false;
      trigger = false;
      return 0;
      }
  }
  if (checkForCard()){ 
    paid = true;
    Serial.println("Paid!");
  }
  return state;
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


PairingStatus autoPairing(){
  switch(pairingStatus) {
  case PAIR_REQUEST:
    Serial.print("Pairing request on channel "  );
    Serial.println(channel);
  
    // clean esp now
    esp_now_deinit();
    WiFi.mode(WIFI_STA);
    // set WiFi channel   
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);
    //WiFi.printDiag(Serial);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
    }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    // set callback routines
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // set pairing data to send to the server
    pairingData.id = BOARD_ID;     
    pairingData.channel = channel;
    previousMillis = millis();
    // add peer and send request
    Serial.println(esp_now_send(broadcastAddressX, (uint8_t *) &pairingData, sizeof(pairingData)));
    pairingStatus = PAIR_REQUESTED;
    break;

  case PAIR_REQUESTED:
    // time out to allow receiving response from server
    currentMillis = millis();
    if(currentMillis - previousMillis > 100) {
      previousMillis = currentMillis;
      // time out expired,  try next channel
      channel ++;
      if (channel > 11) {
        channel = 0;
      }
      pairingStatus = PAIR_REQUEST; 
    }
    break;

  case PAIR_PAIRED:
    //Serial.println("Paired!");
    break;
  }
  return pairingStatus;
} 



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  //pinMode(LED_BUILTIN, OUTPUT);
  // Init DHT sensor
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  delay(10);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  pairingData.id = BOARD_ID;
}
 
void loop() { 
  if (autoPairing() == PAIR_PAIRED) { 
    getReadings();
    static unsigned long lastEventTime = millis();
    static const unsigned long EVENT_INTERVAL_MS = 2000;
    if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
      Serial.print(".");
      

      //Set values to send
      myData.msgType = DATA;
      myData.id = BOARD_ID;
      myData.state = state;
      myData.reserved = reserved;
      myData.readingId = readingId ++;
      
      // Send message via ESP-NOW to all peers 
      esp_now_send(pairingData.macAddr, (uint8_t *) &myData, sizeof(myData));
      lastEventTime = millis();
    }
  }
}
