#include <SPI.h>
#include <WiFiNINA.h>
#include "server_config.h"


/********************************************/
//                                          //
//         Configuration Constants          //
//                                          //
/********************************************/


const char IP_ADDR[] = CONF_IP_ADDR;
const int PORT = CONF_PORT;
const int keyIndex = 0;

char ssid[] = CONF_SECRET_SSID;
char pass[] = CONF_SECRET_PASS;


/********************************************/
//                                          //
//             Helper classes               //
//                                          //
/********************************************/

enum ACTION {
  COOKING,
  READING,
  WORKOUT,
  SCREENTIME,
  CODING,
  OFFICE_WORK,
  EATING
};

// LEDHelper class that wrapps the libray written by Houda and Meryem
class LEDHelper {
  public:
    void setLightIntensity() {
      // TODO: call the light library libray written by Houda and Meryem
      return;
    }
};


// SENSORHelper class that wraps the libray written by Houda and Meryem
class SENSORHelper {
  public:
    int getLightIntensity() {
      // TODO: call the light library libray written by Houda and Meryem
      return 0;
    }
};


// ACTIONHelper class for setting the action of the user
class ACTIONHelper {

  private:
    ACTION currentAction = ACTION::READING;
    SENSORHelper* sensorHelper;
    LEDHelper* ledHelper;

  public:

    ACTIONHelper() {
      this->sensorHelper = new SENSORHelper();
      this->ledHelper = new LEDHelper();
    }

    void setActionAndDuration(ACTION action, int duration) {

      currentAction = action;

      // TODO: use the library libray written by Houda and Meryem
      // to set the light intensity

      // TODO: Reset the action after the defined duration

    }

    ACTION getCurrentAction() {
      return currentAction;
    }

    int getLightIntensity() {
      return sensorHelper->getLightIntensity();
    }

};

// HTTPHelper class to help write HTTP responses
class HTTPHelper {

  public:

    void setJSONResponseHeaders(WiFiClient client) {
      // Set the HTTP response headers
      // such that the client knows the response
      // is of type json
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:application/json");
      client.println();
    }

    void endHTTPResponse(WiFiClient client) {
      client.println();
    }

    void writeBody(WiFiClient client, String bodyString) {
      client.print(bodyString);
    }

};


int status = WL_IDLE_STATUS;
WiFiServer server(PORT);


/********************************************/
//                                          //
//           HTTP Server module             //
//                                          //
/********************************************/


// HTTPServer class that will handle setup of the HTTP Server
// on the Arduino and also reading requests sent from the clients
// and finally responding to those requests
class HTTPServer {
  
  private:
    HTTPHelper* httpHelper;
    ACTIONHelper* actionHelper;

  public:

    HTTPServer() {
      this->httpHelper = new HTTPHelper();
      this->actionHelper = new ACTIONHelper();
    }

    void setUp();
    void serverLoop();

};


void HTTPServer::setUp() {

  // base code author: https://github.com/arduino-libraries/WiFiNINA
  // base code adapted for our use case

  // Initialize serial and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // Check the firmware version as the WiFiNINA requires
  // the correct version
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Set the IP address of the AP
  WiFi.config(IPAddress(IP_ADDR[0],IP_ADDR[1],IP_ADDR[2],IP_ADDR[3]));

  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:

  status = WiFi.beginAP(ssid, pass);

  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }

  // wait 10 seconds for connection
  delay(10000);
  server.begin();

  // print wifi stats info
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("Password: ");
  Serial.println(pass);

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

}

void HTTPServer::serverLoop() {

  // base code author: https://github.com/arduino-libraries/WiFiNINA
  // base code adapted for our use case

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

    // make a String to hold incoming data from the client
    String currentLine = "";

    while (client.connected()) {

      // Read the incoming HTTP request
      if (client.available()) {

        char c = client.read();

        Serial.write(c);

        if (c == '\n') {

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

          } else {
            currentLine = "";
          }

        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /intensity")) {

          // TODO: use the library to get the light intensity
          httpHelper->setJSONResponseHeaders(client);
          int lightIntensity = actionHelper->getLightIntensity();
          httpHelper->writeBody(client,"{ 'status': 'success' }");
          httpHelper->endHTTPResponse(client);
          break;

        }

        if (currentLine.endsWith("GET /action")) {

          // TODO: use the library to get the current action and duration
          httpHelper->setJSONResponseHeaders(client);
          ACTION currentAction = actionHelper->getCurrentAction();
          httpHelper->writeBody(client,"{ 'status': 'success' }");
          httpHelper->endHTTPResponse(client);
          break;

        }

        if (currentLine.endsWith("POST /action")) {

          // TODO: use the library to get the current action and duration
          httpHelper->setJSONResponseHeaders(client);
          actionHelper->setActionAndDuration(ACTION::READING, 20);
          httpHelper->writeBody(client,"{ 'status': 'success' }");
          httpHelper->endHTTPResponse(client);
          break;

        }

      }

    }

    // close the connection:
    client.stop();
    Serial.println("client disconnected");

  }
}

HTTPServer* httpServer = new HTTPServer();

void setup() {
  httpServer->setUp();
}

void loop() {
  httpServer->serverLoop();
}