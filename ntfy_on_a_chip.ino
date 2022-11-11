//NTFY_on_a_chip written by Gerge (based on ESP8266HTTPClient library example BasicHttpsClient)

//This code was tested with ESP8266 Wemos D1 Mini and ESP32 LOLIN32 Lite (should be compatible with other ESP8266 and ESP32 devices)
//For ESP32 comment out this line:
#define board_esp8266

//Arduino IDE instructions
// 1) To add ESP8266 or ESP32 to boards manager go to file > preferences > Additional Boards Manager URLs > add the link for your board:
// https://arduino.esp8266.com/stable/package_esp8266com_index.json
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// 2) Make sure to install the appropriate ESP8266 or ESP32 boards in your boards manager (ESP32 by Espressif Systems or ESP8266 by ESP8266 community)

//Libraries needed: (all should be installed by default with the board, except arduinojson)
//go to Sketch > Include Library > Manage Libraries > search and install arduinojson
#ifdef board_esp8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#else //for ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#endif
#include <ArduinoJson.h>

//  __________________________________________________
// |                                                  |
// |               Compilation Settings               |
// |__________________________________________________|

#define ssid ""
#define password ""
String serverName = "https://ntfy.sh/gerge_test";
//#define base64_login "" //If you have access control use the following command to generate it in terminal: echo -n 'user:pass' | base64
#define getinterval 10000

#ifdef board_esp8266
// Fingerprints expire over time as sites renew SSL certificates, needs to be updated for your specific website
const uint8_t fingerprint[20] = {0x40, 0xAC, 0x41, 0xD2, 0xAB, 0x27, 0x74, 0x9A, 0x18, 0xD6, 0x83, 0x51, 0xB3, 0x65, 0x74, 0x86, 0xD9, 0xE7, 0xAB, 0xB5};  //ntfy.sh SSL SHA1
#else //for ESP32
//Public certificate for HTTPS (for details on how to get it (unfortunately chrome only instructions) refer to: https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request/)
const char root_ca[] PROGMEM = R"rawliteral(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)rawliteral";
#endif


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
#ifdef board_esp8266
ESP8266WiFiMulti WiFiMulti;
#endif

//  __________________________________________________
// |                                                  |
// |                      Setup                       |
// |__________________________________________________|

void setup() {

  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT);
  // Serial.setDebugOutput(true);

  Serial.println();
  #ifdef board_esp8266
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  #else // for ESP32
  WiFi.begin(ssid, password); 
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
 
  Serial.println("..Connected");
  #endif
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
#ifdef board_esp8266
void NTFY_GET(){
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    String serverPath = serverName + "/json?poll=1&since=" + lastMessageTime;

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();

    HTTPClient https;

    //Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, serverPath.c_str())) {  // HTTPS
      #ifdef base64_login
      https.addHeader("Authorization", "Basic "base64_login);
      #endif

      //Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          //Serial.println(payload);
          //loop to process all commands/messages
          while (payload.indexOf("}") > -1){               //loop until there are no more messages to process
            int pos=payload.indexOf("}");                  //get position index of first "}" end of message json
            int length = payload.length();                 //get length of payload
            String message=payload.substring(0, pos+1);    //first message is separated out
            payload=payload.substring(pos+1, length);      //message that is processed is removed from payload
            Serial.print("message: ");
            Serial.println(message);
            //set a buffer for processing json
            StaticJsonDocument<200> doc;
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
  }
}

void NTFY_POST(){
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();

    HTTPClient https;

    //Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, serverName)) {  // HTTPS
      https.addHeader("Content-Type", "text/plain");
      #ifdef base64_login
      https.addHeader("Authorization", "Basic "base64_login);
      #endif
      https.addHeader("title", "NTFY_on_a_chip");
      String httpsRequestData = "Hello from ESP8266. LED=";
      httpsRequestData += String(ledstate);
      int httpsResponseCode = https.POST(httpsRequestData);
      Serial.print("[HTTPS] POST... code: ");
      Serial.println(httpsResponseCode);
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}
#else // for ESP32 use these functions
void NTFY_GET(){
  if ((WiFi.status() == WL_CONNECTED)) {
    String serverPath = serverName + "/json?poll=1&since=" + lastMessageTime;
    HTTPClient https;
    if (https.begin(serverPath, root_ca)){ //Specify the URL and certificate
      #ifdef base64_login
      https.addHeader("Authorization", "Basic "base64_login);
      #endif
      int httpCode = https.GET();                                                  //Make the request
   
      if (httpCode > 0) { //Check for the returning code
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          //Serial.println(payload);
          //loop to process all commands/messages
          while (payload.indexOf("}") > -1){               //loop until there are no more messages to process
            int pos=payload.indexOf("}");                  //get position index of first "}" end of message json
            int length = payload.length();                 //get length of payload
            String message=payload.substring(0, pos+1);    //first message is separated out
            payload=payload.substring(pos+1, length);      //message that is processed is removed from payload
            Serial.print("message: ");
            Serial.println(message);
            //set a buffer for processing json
            StaticJsonDocument<200> doc;
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
  }
}

void NTFY_POST(){
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient https;
    if (https.begin(serverName, root_ca)){ //Specify the URL and certificate
      https.addHeader("Content-Type", "text/plain");
      #ifdef base64_login
      https.addHeader("Authorization", "Basic "base64_login);
      #endif
      https.addHeader("title", "NTFY_on_a_chip");
      String httpsRequestData = "Hello from ESP32. LED=";
      httpsRequestData += String(ledstate);
      int httpsResponseCode = https.POST(httpsRequestData);
      Serial.print("[HTTPS] POST... code: ");
      Serial.println(httpsResponseCode);
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}
#endif
