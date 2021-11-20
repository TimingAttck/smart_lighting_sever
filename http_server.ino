#include <SPI.h>
#include <WiFiNINA.h>
#include "server_config.h"

// base code author: https://github.com/arduino-libraries/WiFiNINA
// base code adapted for our use case

const int IP_ADDR = CONF_IP_ADDR;
const int PORT = CONF_PORT;
const int keyIndex = 0;

char ssid[] = CONF_SECRET_SSID;
char pass[] = CONF_SECRET_PASS;

int status = WL_IDLE_STATUS;

WiFiServer server(PORT);


void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  Serial.println("Access Point Web Server");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    // don't continue

    while (true);

  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {

    Serial.println("Please upgrade the firmware");

  }

  // Set the IP address of the AP
  WiFi.config(IPAddress(IP_ADDR))

  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:

  status = WiFi.beginAP(ssid, pass);

  if (status != WL_AP_LISTENING) {

    Serial.println("Creating access point failed");

    // don't continue

    while (true);

  }

  // wait 10 seconds for connection:

  delay(10000);

  // start the web server on port 80

  server.begin();

  // you're connected now, so print out the status

  printWiFiStatus();
}

void loop() {

  if (status != WiFi.status()) {
    status = WiFi.status();
    if (status == WL_AP_CONNECTED) {
      Serial.println("Device connected to AP");
    } else {
      Serial.println("Device disconnected from AP");
    }
  }

  // listen for incoming clients
  WiFiClient client = server.available();   

  if (client) {

    Serial.println("new client");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected

      if (client.available()) {             // if there's bytes to read from the client,

        char c = client.read();             // read a byte, then

        Serial.write(c);                    // print it out the serial monitor

        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println();

            // the content of the HTTP response follows the header:

            client.print("{ 'status': 'success' }");

            // The HTTP response ends with another blank line:

            client.println();

            // break out of the while loop:

            break;

          }

          else {      // if you got a newline, then clear currentLine:

            currentLine = "";

          }

        }

        else if (c != '\r') {    // if you got anything else but a carriage return character,

          currentLine += c;      // add it to the end of the currentLine

        }


        if (currentLine.endsWith("GET /intensity")) {

          // TODO: use the library to get the light intensity

        }

        if (currentLine.endsWith("GET /action")) {

          // TODO: use the library to get the current action and duration

        }

        if (currentLine.endsWith("POST /action")) {

          // TODO: use the library to set the action and duration

        }

      }

    }

    // close the connection:

    client.stop();
    Serial.println("client disconnected");

  }
}

void printWiFiStatus() {

  // print wifi stats info

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("Password: ");
  Serial.println(SECRET_PASS);

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

}