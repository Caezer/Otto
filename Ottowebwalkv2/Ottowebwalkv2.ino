
// Download and Include the below libraries commented 
//#include<ESP32Servo.h>
//#include "ESP32Tone.h"
//#include "ESP32PWM.h"

#include "Otto.h"
#include <WiFi.h>
Otto Otto;  //This is Otto!

//---------------------------------------------------------
//-- First step: Configure the pins where the servos are attached
/*
           --------------- --> EYES ECHO --D1 TRIG D2
          |     O   O     | o)) BZ D4
  RA D3 ==|               | == LA D0
          |---------------|
  BR D6==>|               | <== BL D5
          ---------------
             ||     ||
  FR D8==> -----   ------  <== FL D7
          |-----   ------|
*/
#define PIN_BL 27       // servo[0] Body Left
#define PIN_BR 14       // servo[1] Body Right
#define PIN_FL 13       // servo[2] Foot Left
#define PIN_FR 12       // servo[3] Foot Right
#define PIN_LA 25       // servo[4] Left Arm
#define PIN_RA 26       // servo[5] Right Arm
#define BUZZER 32       // Buzzer

///////////////////////////////////////////////////////////////////
//-- Setup ----------------l--------------------------------------//
///////////////////////////////////////////////////////////////////
const char* ssid = "Seth’s iphone";
const char* password = "12345678";

WiFiServer server(80);

String header;

String forwalk = "off";
String bacwalk = "off";
String turnr = "off";
String turnl = "off";
String updwn = "off";
String flapp = "off";


unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;


void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  bool load_calibration = false;
  Otto.init(PIN_BL, PIN_BR, PIN_FL, PIN_FR,PIN_LA, PIN_RA, load_calibration, BUZZER);
  Otto.setTrims(0, 0, 0, 0, 0, 0);
  //BL, BR, FL, FR, LA, RA;
}

void loop() {
    WiFiClient client = server.available();
    if (client) {                             // If client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /walk/on") >= 0) {
              Serial.println("ON");
              forwalk = "off";
              Otto.walk(10,1000,FORWARD);
              }
             else if (header.indexOf("GET /back/on") >= 0){
                Serial.println("Walking Back");
                bacwalk = "off";
                Otto.walk(6, 1000, BACKWARD);
              }
             else if (header.indexOf("GET /turnr/on") >= 0){
                Serial.println("Turning right");
                turnr = "off";
                Otto.turn(7, 2000, RIGHT); 
              }
              else if (header.indexOf("GET /turnl/on") >= 0){
                Serial.println("Turning left");
                turnl = "off";
                Otto.turn(7, 2000, LEFT); 
              }
              else if (header.indexOf("GET /updwn/on") >= 0){
                Serial.println("updown");
                updwn = "off";
                Otto.updown(3);
              }
              else if (header.indexOf("GET /flapp/on") >= 0){
                Serial.println("flapping");
                flapp = "off";
                Otto.flapping(3);
              }
                           

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style buttons
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println("</style></head>");
            
            // Webpage heading
            client.println("<body><h1>ESP32 Otto Bot Web Server</h1>");
            
            // Display buttons  
            client.println("<p>Action W</p>");
            if (forwalk=="off") {
              client.println("<p><a href=\"/walk/on\"><button class=\"button\">WALK</button></a></p>");
            } 
            
            client.println("<p>Action B</p>");
            if (bacwalk=="off") {
              client.println("<p><a href=\"/back/on\"><button class=\"button\">BACK</button></a></p>");
            }
            
            client.println("<p>Action R</p>");
            if (turnr=="off") {
              client.println("<p><a href=\"/turnr/on\"><button class=\"button\">RIGHT</button></a></p>");
            }
            
            client.println("<p>Action L</p>");
            if (turnl=="off") {
              client.println("<p><a href=\"/turnl/on\"><button class=\"button\">LEFT</button></a></p>");
            }
            
            client.println("<p>Action UPD</p>");
            if (updwn=="off") {
              client.println("<p><a href=\"/updwn/on\"><button class=\"button\">UPDOWN</button></a></p>");
            }
            
            client.println("<p>Action FLP</p>");
            if (flapp=="off") {
              client.println("<p><a href=\"/flapp/on\"><button class=\"button\">FLAP</button></a></p>");
            }
            
             client.println("</body></html>");

             client.println();

            
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
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
