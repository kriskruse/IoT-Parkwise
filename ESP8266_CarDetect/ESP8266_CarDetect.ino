#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <Ultrasonic.h>

/*
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library
*/

// Replace with your network credentials
const char* ssid = "kunikunkun";
const char* password = "58354723";

// #define DHTPIN 27     // Digital pin connected to the DHT sensor
const int trigPin = D0;
const int echoPin = D1;
const int photores = A0;

Ultrasonic ultrasonic (trigPin, echoPin);

// long duration;
int distance;
int value;
int photoThres = 500; // photores  sensor threshold
int proxiThres = 20; // proximity sensor threshold

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDistance() {
  /*
  digitalWrite(trigPin, LOW);
  delay(20);
  digitalWrite(trigPin, HIGH);
  delay(100);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  */

  distance = ultrasonic.read();
  
  if (isnan(distance)) {    
    return "--";
  }
  else {
    return String(distance);
  }
}

String readPhotores() {
  value = analogRead(photores);
  delay(500);

  if (isnan(value)) {    
    return "--";
  }
  else {
    return String(value);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<title>
IoT Project - Car Detection Algorithm
</title>
<head>  
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Calibri;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .values{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 Car Sensing Server</h2>
  <p>
    <span class="values">Distance</span> 
    <span id="value1">%DISTANCE%</span>
    <sup class="units"></sup>
  </p>
  <p>
    <span class="values">Brightness Value</span>
    <span id="value2">%BRIGHTNESS%</span>
    <sup class="units"></sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("value1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/value1", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("value2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/value2", true);
  xhttp.send();
}, 1000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "DISTANCE"){
    return readDistance();
  }
  else if(var == "BRIGHTNESS"){
    return readPhotores();
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/value1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDistance().c_str());
  });
  server.on("/value2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readPhotores().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
