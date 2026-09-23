#include <WiFi.h>

// add your WI-FI name & passworrd
const char* ssid     = "your wifi name";
const char* password = "your password";


WiFiServer server(80);


const int ledPin = 2;
String header;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 

  // connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // diplay ip address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  

  if (client) {                             
    Serial.println("New Client Connected."); 
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            
            if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              digitalWrite(ledPin, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              digitalWrite(ledPin, LOW);
            }
            
            // HTML page  (Web Interface)
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { font-family: Arial; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #f44336;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Wi-Fi Web Server</h1>");
            
            // Light state
            if (digitalRead(ledPin) == LOW) {
              client.println("<p>LED is OFF</p>");
              client.println("<p><a href=\"/4/on\"><button class=\"button\">TURN ON</button></a></p>");
            } else {
              client.println("<p>LED is ON</p>");
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">TURN OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // HTTP response close.
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
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
