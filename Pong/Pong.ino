#include <ESP8266WiFi.h>
#include <espnow.h>


// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x08, 0xB6, 0x1F, 0x81, 0x0F, 0xEC};
 

// Variable to store if sending data was successful
String success;
String data = "Pong!";
String incoming;
bool gotdata = false;


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == 0){
        Serial.println("Pong Delivery success");
    }
    else{
        Serial.println("Pong Delivery fail");
    }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    memcpy(&incoming, incomingData, sizeof(incoming));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.println("Recieved a Ping");
    gotdata = true;
}

 
void setup() {
    // Init Serial Monitor
    Serial.begin(115200);


    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }

    // Set ESP-NOW Role
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
    if (gotdata){
        delay(3000);
        esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
        gotdata = false;
    }
}
