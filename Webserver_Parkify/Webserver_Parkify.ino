#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library


//TODO: add a page that it takes you to once you book your spot

ESP8266WiFiMulti wifiMulti;
// Create an instance of the server
ESP8266WebServer server(80);

const int led = 4;  //D2
const int led1 =0; //D3
unsigned long payStartTime[] ={0,0}; //
int parkingStates[2] = {1,1};

void handleRoot();  
void handleLED();  
void handleNotFound();

void setup() {
   Serial.begin(115200);
  delay(10);

  pinMode(led, OUTPUT);
  digitalWrite(led,1);
  
  pinMode(led1, OUTPUT);
  digitalWrite(led1,1);  
  // Connect to WiFi network
  Serial.println();
  wifiMulti.addAP("Jordan’s phone", "jordan123");  // add Wi-Fi networks you want to connect to

  
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



  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED0", HTTP_POST, handleLED0);
  server.on("/LED1", HTTP_POST, handleLED1);

  server.onNotFound(handleNotFound);
    
  // Start the server
  server.begin();
  Serial.println("Server started"); 
}

<<<<<<< Updated upstream
void loop() {
  // Check if a client has connected
  server.handleClient();
}
=======
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            //button methods
            if (header.indexOf("GET /spot0/on") >= 0) {
              Serial.println("Spot0");
              bookSpot(0);
            } else if (header.indexOf("GET /spot0/off") >= 0) {
              Serial.println("Spot0");
              parkingStates[0]=0;
            } else if (header.indexOf("GET /spot1/on") >= 0) {
              Serial.println("Spot1");
              bookSpot(1);
            } else if (header.indexOf("GET /spot1/off") >= 0) {
              Serial.println("Spot1");
              parkingStates[0]=0;
            } else if (header.indexOf("GET /cancel/on") >= 0) {
              cancelBookings();
              digitalWrite(output27, HIGH);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555; color: white;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Parkify Parking Lot</h1>");
            client.println("<p>The map below illustrates the availability of parking spots.</p>");
            client.println("<p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available.</p>");

            //Map
            updateParkingStates();
            client.println("<div style=\"display: flex; justify-content: center; height: 5vh; margin: 0 auto;\">");
            client.println("  <div style=\"background-color: " + String(colors[parkingStates[0]]) + "; width: 100px; height: 50px; text-align: center; line-height: 50px; color: black;\">Spot 0</div>");
            client.println("  <div style=\"background-color: " + String(colors[parkingStates[1]]) + "; width: 100px; height: 50px; text-align: center; line-height: 50px; color: black;\">Spot 1</div>");
            client.println("</div>");
            
            client.println("<p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer).</p>");
            client.println("<p>Click on the available spot button that you want to book.</p>");
>>>>>>> Stashed changes


/*void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<html><title>Parkify</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>DTU Parking Lot</h1> \ 
      <p>The map below illustrates the availability of parking spots. </p> \
      <p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available</p> \ <p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer)</p> \
      <p>Click on the available spot button that you want to book</p> \
      <form action=\"/LED0\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot0\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <form action=\"/LED1\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot1\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
      
}*/
void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  sendHomePage();
  //Serial.println("i c u");
      
}

void sendHomePage() {
  updateParkingSpace();
  if (parkingStates[0]==0 && parkingStates[1]==0) {
    server.send(200, "text/html", "<html><title>Parkify</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>DTU Parking Lot</h1> \ 
      <p>The map below illustrates the availability of parking spots. </p> \
      <p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available</p> \ <p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer)</p> \
      <p>Click on the available spot button that you want to book</p> \
      <form action=\"/LED0\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot0\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <form action=\"/LED1\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot1\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
  } else if(parkingStates[0]==0 && parkingStates[1]!=0){
    server.send(200, "text/html", "<html><title>Parkify</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>DTU Parking Lot</h1> \ 
      <p>The map below illustrates the availability of parking spots. </p> \
      <p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available</p> \ <p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer)</p> \
      <p>Click on the available spot button that you want to book</p> \
      <form action=\"/LED0\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot0\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
  } else if (parkingStates[0]!=0 && parkingStates[1]==0){
    server.send(200, "text/html", "<html><title>Parkify</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>DTU Parking Lot</h1> \ 
      <p>The map below illustrates the availability of parking spots. </p> \
      <p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available</p> \ <p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer)</p> \
      <p>Click on the available spot button that you want to book</p> \
      <form action=\"/LED1\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot1\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
  } else  {
    server.send(200, "text/html", "<html><title>Parkify</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>DTU Parking Lot</h1> \ 
      <p>The map below illustrates the availability of parking spots. </p> \
      <p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available</p> \ <p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer)</p> \
      <p>Click on the available spot button that you want to book</p> \
      </body></html>");
  }
  
}
/*    This seems to be the correct way to code this, but unfortunately, the ESP8266 seems to not have enough memory to do this
String generateHTML() {
  

  int* parkingStates = getParkingStates();

  String htmlString = "<html><title>Parkify</title><meta charset=\"utf-8\" \/> \ 
  </head><body><h1>DTU Parking Lot</h1> \ 
  <p>The map below illustrates the availability of parking spots. </p> \
  <p>Red means that someone is parked there, yellow means someone reserved the spot, and green means the spot is available</p> \ <p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer)</p> \
  <p>Click on the available spot button that you want to book</p> \
  ";

  if (parkingStates[0] == 0) {
    htmlString += "<form action=\"/LED0\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot0\" style=\"width:500px; height:100px; font-size:24px\"></form> ";
  }

  if (parkingStates[1] == 0) {
    htmlString += "<form action=\"/LED1\" method=\"POST\" ><input type=\"submit\" value=\"Check in to Spot1\" style=\"width:500px; height:100px; font-size:24px\"></form> ";
  }

  htmlString += "</body></html>";
  return(htmlString);
}
*/
void handleLED0() {                          // If a POST request is made to URI /LED
  digitalWrite(led,!digitalRead(led));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handleLED1() {                          // If a POST request is made to URI /LED
  digitalWrite(led1,!digitalRead(led1));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}


void updateParkingSpace()  { //0 means empty, 1 means reserved, and 2 means parked
  //Dummy method
  //return(parkingStates);
  parkingStates[0] = 0;
  parkingStates[1] = 0;
}
