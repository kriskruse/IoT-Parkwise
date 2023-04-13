#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library

ESP8266WiFiMulti wifiMulti;
// Create an instance of the server
ESP8266WebServer server(80);
// pin 2 is D4 on the board
const int led1 = 2;
const int led2 = 4;
int x = 2;
void handlespot1(); 
void handleLED();  
void handleNotFound();

void setup() {
   Serial.begin(115200);
  delay(10);

  pinMode(led1, OUTPUT);
  digitalWrite(led1,1);
  
  pinMode(led2, OUTPUT);
  digitalWrite(led2,1);
  
  // Connect to WiFi network
  Serial.println();
  wifiMulti.addAP("IoTFotonik", "Cyberteknologi");  // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("1+", "Vdqg1808");  
  
  Serial.println();
  Serial.print("Connecting ...");
  //WiFi.begin(ssid, password);
 
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected to ");
  Serial.println(WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("iot")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }



  server.on("/", HTTP_GET, handlespot1);
  server.on("/LED1", HTTP_POST, handleLED1);
  server.on("/LED2", HTTP_POST, handleLED2);  
  server.onNotFound(handleNotFound);
    
  // Start the server
  server.begin();
  Serial.println("Server started"); 
}

void loop() {
  // Check if a client has connected
  server.handleClient();
}


void handlespot1() {                         // When URI / is requested, send a web page with a button to toggle the LED
  if (x == 1){
    server.send(200, "text/html", "<html><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Velkommen til denne WebServer TESTER </h1> \ 
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/LED1\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED1\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <form action=\"/LED2\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED2\" style=\"width:500px; height:100px; font-size:24px\"></form> \      
      <p>Med en Arduino ESP8266 kan du lave et have a sjove projekter</p> \
      <p>Vil du vide mere: Kig på hjemmesiden for uddannelsen : <a href=\"www.dtu.dk/net\">Netværksteknologi og it</a></p> \
      </body></html>");
  }
  else{
  server.send(200, "text/html", "<html><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Velkommen til denne WebServer TEST!</h1> \ 
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/LED1\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED1\" style=\"width:500px; height:100px; font-size:24px\"></form> \    
      <p>Med en Arduino ESP8266 kan du lave et have a sjove projekter</p> \
      <p>Vil du vide mere: Kig på hjemmesiden for uddannelsen : <a href=\"www.dtu.dk/net\">Netværksteknologi og it</a></p> \
      </body></html>");
  }
}


void handleLED1() {                          // If a POST request is made to URI /LED
  digitalWrite(led1,!digitalRead(led1));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleLED2() {                          // If a POST request is made to URI /LED
  digitalWrite(led2,!digitalRead(led2));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
  
