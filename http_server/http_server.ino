#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>


/********************************************/
//                                          //
//         Configuration Constants          //
//                                          //
/********************************************/


#define CONF_SECRET_SSID "SMART_AND_CONNECT_LIGHTING_AP"
#define CONF_SECRET_PASS "[f/%ua/8aMG8S<6e"
#define CONF_IP_ADDR "10.10.10.1"
#define CONF_PORT 80

const char IP_ADDR[] = CONF_IP_ADDR;
const int PORT = CONF_PORT;
const int keyIndex = 0;

char ssid[] = CONF_SECRET_SSID;
char pass[] = CONF_SECRET_PASS;


/********************************************/
//                                          //
//            User Action Defs              //
//                                          //
/********************************************/


enum ACTION {
  COOKING,
  READING,
  WORKOUT,
  SCREENTIME,
  CODING,
  OFFICE_WORK,
  EATING,
  NONE
};

String actionToString(ACTION action) {
    switch (action) {
        case ACTION::COOKING: return "COOKING";
        case ACTION::READING: return "READING";
        case ACTION::WORKOUT: return "WORKOUT";
        case ACTION::SCREENTIME: return "SCREENTIME";
        case ACTION::CODING: return "CODING";
        case ACTION::OFFICE_WORK: return "OFFICE_WORK";
        case ACTION::EATING: return "EATING";
        default: return "NONE";
    }
};

ACTION stringToAction(String action) {
   if (action == "COOKING")
      return ACTION::COOKING;
   else if (action == "READING")
      return ACTION::READING;
   else if (action == "WORKOUT")
      return ACTION::WORKOUT;
   else if (action == "SCREENTIME")
      return ACTION::SCREENTIME;
   else if (action == "CODING")
      return ACTION::CODING;
   else if (action == "OFFICE_WORK")
      return ACTION::OFFICE_WORK;
   else if (action == "OFFICE_WORK")
      return ACTION::OFFICE_WORK;
   else if (action == "EATING")
      return ACTION::EATING;
   else 
      return ACTION::NONE;
};


/********************************************/
//                                          //
//              Helper Classes              //
//                                          //
/********************************************/

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
    SENSORHelper* sensorHelper;
    LEDHelper* ledHelper;
    unsigned long previousMillis = 0;
    long timeOut = 0;

  public:
  
    ACTION currentAction;
    
    ACTIONHelper() {
      this->currentAction = ACTION::NONE;
      this->sensorHelper = new SENSORHelper();
      this->ledHelper = new LEDHelper();
    }

    void setActionAndDuration(ACTION action, int durationInMinutes) {

      this->currentAction = action;

      // TODO: use the optimal lighting to get or set the 
      // lighting according to the action and the sensor value

      long clampedDurationInMinutes = constrain(durationInMinutes, 1, 1440);

      // Set timeout for the action to be reset to NONE
      long timeoutInMillis = clampedDurationInMinutes*60000;
      this->timeOut = timeoutInMillis;
      
    }
    
    void checkActionTimeout() {

      unsigned long currentMillis = millis();
      
      // Reset action if timeout has passed
      if (this->timeOut != 0) {
        if (currentMillis - this->previousMillis > this->timeOut) {
          this->previousMillis = currentMillis;  
          this->currentAction = ACTION::NONE;
          this->timeOut = 0;
        }
      }
      
    }

    ACTION getCurrentAction() {
      return this->currentAction;
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
  
  public:
    ACTIONHelper* actionHelper;
  
  private:
    HTTPHelper* httpHelper;

  public:

    HTTPServer() {
      this->httpHelper = new HTTPHelper();
      this->actionHelper = new ACTIONHelper();
    }

    void setUp();
    void serverLoop();

};


void HTTPServer::setUp() {

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
    String endpoint = "";
    String jsonBody = "";
    int lineCount = 0;
    boolean reachedBody = false;

    // Read the incoming data
    while (client.connected()) {

      // Read the incoming HTTP request
      if (client.available()) {

        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (reachedBody != true) {
            if (lineCount == 0) {
              if (currentLine.indexOf("GET") >= 0 || currentLine.indexOf("POST") >= 0 ) {
                endpoint = currentLine;
              }
            }
            lineCount = lineCount+1;
            if (currentLine == "") {
              reachedBody = true;
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          if (reachedBody) {
            jsonBody += c;
          } else {
            currentLine += c;
          }
        }

      } else {
        break;
      }
    }


    /********************************************/
    //                                          //
    //             Server endpoints             //
    //                                          //
    /********************************************/


    if (endpoint.startsWith("GET /intensity")) {

      /**
       *
       * Server endpoint: GET /intensity
       *
       **/

      httpHelper->setJSONResponseHeaders(client);
      int lightIntensity = actionHelper->getLightIntensity();

      String responseString = "";
      DynamicJsonDocument responseBody(1024);
      responseBody["status"] = "success";
      responseBody["intensity"] = String(lightIntensity);
      responseBody["unit"] = "lux";
      serializeJson(responseBody, responseString);

      httpHelper->writeBody(client,responseString);
      httpHelper->endHTTPResponse(client);
      
    } else if (endpoint.startsWith("GET /action")) {

      /**
       *
       * Server endpoint: GET /action
       *
       **/

      DynamicJsonDocument requestBody(1024);
      deserializeJson(requestBody, jsonBody);
      
      httpHelper->setJSONResponseHeaders(client);
      ACTION currentAction = actionHelper->getCurrentAction();
      String enumString = actionToString(currentAction);

      String responseString = "";
      DynamicJsonDocument responseBody(1024);
      responseBody["status"] = "success";
      responseBody["action"] = enumString;
      serializeJson(responseBody, responseString);
      
      httpHelper->writeBody(client,responseString);
      httpHelper->endHTTPResponse(client);
      
    } else if (endpoint.startsWith("POST /action")) {

      /**
       *
       * Server endpoint: POST /action
       *
       **/

      DynamicJsonDocument requestBody(100);
      deserializeJson(requestBody, jsonBody);

      ACTION action = stringToAction(requestBody["action"]);
      int durationInMinutes = requestBody["durationInMinutes"];
      actionHelper->setActionAndDuration(action, durationInMinutes);
      httpHelper->setJSONResponseHeaders(client);

      String responseString = "";
      DynamicJsonDocument responseBody(1024);
      responseBody["status"] = "success";
      serializeJson(responseBody, responseString);
      
      httpHelper->writeBody(client,responseString);
      httpHelper->endHTTPResponse(client);
      
    } else {

      /**
       *
       * Server endpoint: any other
       *
       **/

      httpHelper->setJSONResponseHeaders(client);

      String responseString = "";
      DynamicJsonDocument responseBody(1024);
      responseBody["status"] = "failure";
      serializeJson(responseBody, responseString);
      
      httpHelper->writeBody(client,responseString);
      httpHelper->endHTTPResponse(client);
      
    }

    // close the connection:
    client.stop();
    Serial.println("client disconnected");

  }
}


/********************************************/
//                                          //
//          Ardunio Control Loop            //
//                                          //
/********************************************/


HTTPServer* httpServer = new HTTPServer();

void setup() {
  httpServer->setUp();
}

void loop() {
  httpServer->actionHelper->checkActionTimeout();
  httpServer->serverLoop();
}
