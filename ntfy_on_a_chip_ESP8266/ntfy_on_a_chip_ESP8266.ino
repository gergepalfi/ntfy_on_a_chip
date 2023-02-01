//NTFY_on_a_chip written by Gerge (based on library example BasicHttpsClient)

//This code was tested with ESP8266 Wemos D1 Mini (should be compatible with other ESP8266 devices)

//Arduino IDE instructions
// 1) To add ESP8266 to boards manager go to file > preferences > Additional Boards Manager URLs > add the link for your board:
// https://arduino.esp8266.com/stable/package_esp8266com_index.json
// 2) Make sure to install the appropriate ESP8266 boards in your boards manager (ESP8266 by ESP8266 community)

//Libraries needed: (all should be installed by default with the board, except arduinojson)
//go to Sketch > Include Library > Manage Libraries > search and install arduinojson
#include <ESP8266WiFi.h>
#include <CertStoreBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

//  __________________________________________________
// |                                                  |
// |               Compilation Settings               |
// |__________________________________________________|

#define devicename "ntfy_on_a_chip"
#define ssid ""
#define pass ""
String serverName = "https://ntfy.sh/gerge_test";
//#define base64_login "" //If you have access control use the following command to generate it in terminal: echo -n 'user:pass' | base64
#define getinterval 20000

//_____________________________________________________(end of config. no need to edit beyond this line)

//pins
#define LEDpin LED_BUILTIN //inbuilt LED
#define OFFSTATE HIGH
#define ONSTATE LOW

//Globals
byte ledstate = 0;                        //0=off, 1=on (default state 0)
unsigned long gettimestamp = 0;           //millis() timestamp of last HTTPS GET call
byte respond =0;                          //when set to 1 the board sends a response message with current state of LED
unsigned long lastMessageTime=1666810118; //UNIX time of last message that was processed (prevents processing of the same message multiple times)
BearSSL::CertStore certStore;


//  __________________________________________________
// |                                                  |
// |                      Setup                       |
// |__________________________________________________|

void setup() {
  Serial.begin(115200);
  Serial.println();
  LittleFS.begin();

  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);        //turn off on board LED

  //  __________________________________________________
  // |                                                  |
  // |                        WiFi                      |
  // |__________________________________________________|
  Serial.print("Connect to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(devicename);
  WiFi.begin(ssid, pass);
  byte notfoundcounter = 0;
  unsigned long timestamp=millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
  }
  Serial.println();
  Serial.println("Connected!");
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/"); 

  setClock();  // Required for X.509 validation
  
  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
    return;  // Can't connect to anything w/o certs!
  }
  NTFY_GET();    //get latest timestamp
  respond =0;    //reset default state (ignore processed old messages)
  ledstate = 0;  //reset default state (ignore processed old messages)
}

//  __________________________________________________
// |                                                  |
// |                      Loop                        |
// |__________________________________________________|

void loop() {
  if (respond ==1 ) {
    respond =0;
    NTFY_POST();
  }
  if ((gettimestamp + getinterval) < millis() ) {
    NTFY_GET();
    gettimestamp = millis();
  }
  if(ledstate == 1){
    digitalWrite(LEDpin, ONSTATE);
  }else{
    digitalWrite(LEDpin, OFFSTATE);
  }
}

//  __________________________________________________
// |                                                  |
// |                    Functions                     |
// |__________________________________________________|

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  //Serial.print(timeinfo);
  Serial.print(asctime(&timeinfo));
}

void NTFY_GET(){
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    String serverPath = serverName + "/json?poll=1&since=" + lastMessageTime;
    
    BearSSL::WiFiClientSecure *client = new BearSSL::WiFiClientSecure();
    client->setCertStore(&certStore);
    HTTPClient https;
    Serial.print("Attempting to fetch ");
    Serial.println(serverPath);
    if (https.begin(*client, serverPath.c_str())) {  // HTTPS
      #ifdef base64_login
      https.addHeader("Authorization", "Basic "base64_login);
      #endif

      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          
          //loop to process all commands/messages
          while (payload.indexOf("}") > -1){               //loop until there are no more messages to process
            int pos=payload.indexOf("}");                  //get position index of first "}" end of message json
            int length = payload.length();                 //get length of payload
            String message=payload.substring(0, pos+1);    //first message is separated out
            payload=payload.substring(pos+1, length);      //message that is processed is removed from payload
            Serial.print("message: ");
            Serial.println(message);
            //set a buffer for processing json
            StaticJsonDocument<400> doc;
            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, message);
          
            // Test if parsing succeeds.
            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }
            lastMessageTime = doc["time"];
            lastMessageTime+=1;                         //increment the unix time by 1 to avoid getting the last processed message with the same unix timestamp
            Serial.print("lastMessageTime: ");
            Serial.println(lastMessageTime);
            
            int startindex;
            startindex=message.indexOf("\"message\":\"1\"");
            if (startindex != -1) {
              //turn LED on
              ledstate = 1;
              respond=1;
            }
            startindex=message.indexOf("\"message\":\"0\"");
            if (startindex != -1) {
              //turn LED off
              ledstate = 0;
              respond=1;
            }
          }
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
    delete client;
  }
}

void NTFY_POST(){
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    
    BearSSL::WiFiClientSecure *client = new BearSSL::WiFiClientSecure();
    client->setCertStore(&certStore);

    HTTPClient https;

    if (https.begin(*client, serverName)) {  // HTTPS
      https.addHeader("Content-Type", "text/plain");
      #ifdef base64_login
      https.addHeader("Authorization", "Basic "base64_login);
      #endif
      https.addHeader("title", devicename);
      String httpsRequestData = "Hello from ESP8266. LED=";
      httpsRequestData += String(ledstate);
      int httpsResponseCode = https.POST(httpsRequestData);
      Serial.print("[HTTPS] POST... code: ");
      Serial.println(httpsResponseCode);
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
    delete client;
  }
}
