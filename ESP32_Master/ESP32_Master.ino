
#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct SubData {
  int id;
  int state;
  bool reserved;
  uint8_t mac[];
}SubData;

// Create a SubData called myData
SubData myData;

// Create a structure to hold the readings from each board
SubData board1 = {id = 1, state = 0, reserved = false, mac =  {0x08, 0xB6, 0x1F, 0x81, 0x09, 0xF4}};
SubData board2 = {id = 2, state = 0, reserved = false, mac =  {0xC8, 0xC9, 0xA3, 0x64, 0xB4, 0x7B}};

// Create an array with all the structures
SubData boardsData[2] = {board1, board2};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsData[myData.id-1].state = myData.state;
  boardsData[myData.id-1].reserved = myData.reserved;
  Serial.printf("state value: %d \n", boardsData[myData.id-1].state);
  Serial.printf("reserved value: %d \n", boardsData[myData.id-1].reserved);
  Serial.println();
}
 
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Acess the variables for each board
  

  // Send message via ESP-NOW
  for (SubData sentData : boardsData){
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sentData, sizeof(sentData));
  }
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(10000);
}


 
