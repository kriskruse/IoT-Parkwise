#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> 

// Structure example to receive data
// Must match the sender structure
typedef struct SubData {
    int id;
    int state;
    bool reserved;
}SubData;

// Create a SubData called myData
SubData data; // temp store for incoming
SubData board1 = {1, 0, false};
SubData board2 = {2, 0, false};
SubData boardsData[] = {board1, board2};

uint8_t broadcastAddress1[] = {0xC8, 0xC9, 0xA3, 0x64, 0xB4, 0x7B};
uint8_t broadcastAddress2[] = {0xC8, 0xC9, 0xA3, 0x64, 0xB4, 0x7B};
uint8_t *adrArr[] = {broadcastAddress1, broadcastAddress2};
// esp now info
esp_now_peer_info_t peerInfo;

// time 
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

//HTTP Server Initialization
// Replace with your network credentials      
const char* ssid = "KruseNet";           
const char* password = "Krusers47";
WiFiServer server(80); // Set web server port number to 80
String header; // Variable to store the HTTP request
int parkingStates[2] = {0,0}; // Auxiliar variables to store the current output state
char* colors[] = {"green", "yellow", "red","orange", "gray", "gray"};



// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
    memcpy(&data, incomingData, sizeof(data));
    Serial.print("Recieved packet from id: ");
    Serial.println(data.id);
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    if (status == ESP_NOW_SEND_SUCCESS){
        Serial.println("Sendt the Data");
    }
    else{
        Serial.println("Failed to send the Data");
    }
}


void setup(){
    Serial.begin(115200); // Init Serial Monitor
    WiFi.mode(WIFI_AP_STA); // Set device as a Wi-Fi Station
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK) { Serial.println("ESP-Now_err"); ESP.restart(); return; } 

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
    memcpy(peerInfo.peer_addr, adrArr[0], 6); // Register peer
    peerInfo.channel = 1;  
    peerInfo.encrypt = false;
    // Add peer        
    if (esp_now_add_peer(&peerInfo) == ESP_OK){
        Serial.println("Added Peer");
    }else{Serial.println("Failed to add peer");}

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);

    //HTTP Server Setup
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
        
}

static unsigned long lastMethod1Time = 0;

void loop(){
    unsigned long currentTime = millis();

    if (currentTime - lastMethod1Time >= 5000){
        esp_err_t result = esp_now_send(adrArr[0], (uint8_t *) &data, sizeof(data));
        lastMethod1Time = currentTime;
    }
    
    
    
}