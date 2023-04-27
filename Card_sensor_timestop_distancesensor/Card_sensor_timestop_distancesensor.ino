#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
unsigned long startTime = 0; // Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
boolean timerRunning = false; // Flag to indicate if timer is running
int distancePin = A0; // Analog input pin for distance sensor
int distanceThreshold = 500; // Threshold distance for detecting an object

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
}

void loop() {
  int distanceValue = analogRead(distancePin); // Read distance sensor value

  // if (!timerRunning && distanceValue > distanceThreshold && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())  
  if (!timerRunning  && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    // Start the timer when a card is detected
    startTime = millis();
    elapsedTime = 0; // Reset elapsed time
    timerRunning = true; // Set timerRunning flag to true
  }

  if (timerRunning) {
    // Calculate elapsed time and print to Serial Monitor every second
    unsigned long currentTime = millis();
    if (currentTime - startTime >= 1000) {
      elapsedTime += 1000; // Add 1 second to elapsed time
      Serial.print(elapsedTime / 1000);
      Serial.println(" sec");
      startTime = currentTime; // Update the start time
    }
  }

  // Stop timer and allow RFID detection when distance sensor detects an object
  // elapsedTime has to be replaced by the distance sensor.
  //distanceValue <= distanceThreshold
  if (timerRunning && elapsedTime>= 10000)  { //distanceValue <= distanceThreshold
    timerRunning = false; // Reset timerRunning flag to false
  }
}