// defines pins numbers
const int trigPin = 9;
const int echoPin = 10;
const int photores = A0;

// defines variables
long duration;
int distance;
int value;
int photoThres; // photores  sensor threshold
int proxiThres; // proximity sensor threshold

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(photores, INPUT);
  Serial.begin(9600);
}


void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delay(200);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delay(500);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor

  value = analogRead(photores);
  delay(500);

  photoThres = 500;
  proxiThres = 20;

  
  if (distance < proxiThres && value > photoThres) {
    Serial.println("CAR IS DETECTED!");
    delay(10000);
  }
  else if (distance < proxiThres && value < photoThres)
    Serial.println("Sensor mismatch! Photoresistor does not detect any car!");
  else if (distance > proxiThres && value > photoThres)
    Serial.println("Sensor mismatch! Proximity sensor does not detect any car!");
  else
    Serial.println("CAR IS NOT DETECTED!");
  
  /*
  // for debugging only
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Value pRES: ");
  Serial.println(value);
  Serial.println();
  */

}

