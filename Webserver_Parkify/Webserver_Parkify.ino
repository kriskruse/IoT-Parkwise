//Code inspired by https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Jordan’s phone";
const char* password = "jordan123";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state

int parkingStates[2] = {0,0};
char* colors[] = {"green", "yellow", "red"};


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  //pinMode(output26, OUTPUT);
  //pinMode(output27, OUTPUT);
  // Set outputs to LOW
  //digitalWrite(output26, LOW);
  //digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
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
            client.println("  <div style=\"background-color: " + String(colors[parkingStates[0]]) + "; width: 100px; height: 50px; text-align: center; line-height: 50px; color: white;\">Spot 0</div>");
            client.println("  <div style=\"background-color: " + String(colors[parkingStates[1]]) + "; width: 100px; height: 50px; text-align: center; line-height: 50px; color: white;\">Spot 1</div>");
            client.println("</div>");
            
            client.println("<p>As soon as you reserve or park in a spot, the timer for your parking begins (you pay based on the timer).</p>");
            client.println("<p>Click on the available spot button that you want to book.</p>");



            

            if (parkingStates[0]==0) {
              client.println("<p>Spot 0</p>");
              client.println("<p><a href=\"/spot0/on\"><button class=\"button\">Book</button></a></p>");
            } else {
              //client.println("<p><a href=\"/spot0/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            if (parkingStates[1]==0) {
              client.println("<p>Spot 1</p>");
              client.println("<p><a href=\"/spot1/on\"><button class=\"button\">Book</button></a></p>");
            } else {
              //client.println("<p><a href=\"/spot0/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("<p></p>");
            client.println("<p></p>");
            client.println("<p>Cancel all bookings</p>");
            client.println("<p><a href=\"/cancel/on\"><button class=\"button2\">Cancel</button></a></p>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void updateParkingStates()  { //0 means empty, 1 means reserved, and 2 means parked
  //Dummy method
  //parkingStates[0] = 0;
  //parkingStates[1] = 0;
}
void bookSpot(int spotNum)  { //first check if someone pulled in to the spot or reserved it since you reserved it
  updateParkingStates();
  if (parkingStates[spotNum]==0)  {
    parkingStates[spotNum]=1;
  }
  //tell the other server the same thing and that records the timestamp
}
void cancelBookings(){//demo method
  updateParkingStates();
  for (int i =0; i<2; i++){
    if (parkingStates[i]==1)  {
      parkingStates[i]=0;
    }
  }
}
