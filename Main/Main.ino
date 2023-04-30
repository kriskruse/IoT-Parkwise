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
SubData board1 = {1, 0, false, {0xC8, 0xC9, 0xA3, 0x64, 0xB4, 0x7B}};
SubData boardsData[1] = {board1};


esp_now_peer_info_t peerInfo;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

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
    char macStr[18];
    Serial.print("Packet sent to: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void setup(){
    //Initialize Serial Monitor
    Serial.begin(115200);

    //Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);


    //Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    


    // Register peer
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    memcpy(peerInfo.peer_addr, boardsData[0].mac, 6);      
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.print("Failed to add peer 1:  ");
        Serial.println(esp_now_add_peer(&peerInfo) - ESP_ERR_WIFI_BASE - 100);
    return;
    }else{
        Serial.println("Added peer 1");
    }

    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

}

static unsigned long lastMethod1Time = 0;
void loop(){
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastMethod1Time;
  
  // Acess the variables for each board
    if (elapsedTime >= 5000) {
    Serial.println("Sending data to boards!");
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(boardsData[0].mac,(uint8_t *) &boardsData[0], sizeof(boardsData[0]));

    Serial.print("Data to board1 func return: ");
    Serial.println(result - ESP_ERR_WIFI_BASE - 100);
    //Serial.println(result == ESP_OK ? "Succes" : "Failure");
    delay(10);
    lastMethod1Time = currentTime;
    }
    
}