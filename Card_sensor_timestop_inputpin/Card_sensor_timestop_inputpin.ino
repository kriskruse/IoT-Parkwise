#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define SENSOR_PIN 2 // Replace with the actual pin number where the sensor is connected

MFRC522 rfid(SS_PIN, RST_PIN);
unsigned long startTime = 0; // Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
boolean timerRunning = false; // Flag to indicate if timer is running

  // DIFFERENCE here we check for pin instead of analog pin distance
  
void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(SENSOR_PIN, INPUT); // Set sensor pin as input
}

void loop() {
    // DIFFERENCE here we check for pin instead of analog pin distance
  if (!timerRunning && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
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

    // DIFFERENCE here we check for pin instead of analog pin distance
    
    // Check for sensor input
    if (digitalRead(SENSOR_PIN) == HIGH) {
      // Stop timer and reset if sensor input is detected
      timerRunning = false; // Reset timerRunning flag to false
      elapsedTime = 0; // Reset elapsed time
    }
  } else {
    // If timer is not running, check for sensor input to start detecting new card
    if (digitalRead(SENSOR_PIN) == HIGH) {
      // Reset any previous card data
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }
}