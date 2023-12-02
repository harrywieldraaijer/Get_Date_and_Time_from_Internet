/*

   Module: Get date, time etc. from the internet

   Framwork Code date/version: 20231127/2

   Project documentation location: PlatformIO/Projects/Get_Date_and_Time_from_Internet

   Developed and created by: Harry Wieldraaijer
   
   Short description:

      Connect to WiFi with stored password, if no stored password enter it via an webbrowser.
      Get date, time etc. from the internet

   Date last modified: 02-December-2023 - Version  2.14 Add OTA funtionality and minor changes
   Date last modified: 02-December-2023 - Version  2.13 Back to version 2.11
   Date last modified: 02-December-2023 - Version  2.12 Try to remove strings
   Date last modified: 01-December-2023 - Version  2.11 Minor changes
   Date last modified: 30-November-2023 - Version  2.1  Clean up and devided in sections
   Date last modified: 29-November-2023 - Version  2.0  Moved from Arduino_JSON.h to ArduinoJSON.h 
   Date last modified: 28-November-2023 - Version  1.0  Initial and original release
  

*/


/* ------------------ Hardware notes ------------------
 
*/


// ------------------ Include and describe libraries ------------------
// ------------------ Section connect to WiFi ------------------
#include <Arduino.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
/// ------------------ End of section connect to WiFi ------------------
// ------------------ Section connect get parameter from http: ------------------
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJSON.h>
// ------------------ End of section connect get parameter from http:  ------------------
// ------------------ Section to enable OTA  ------------------
#include <ArduinoOTA.h>
// ------------------ End of section to enable OTA  ------------------
// ------------------ Declare section framework ------------------
#define DebugOutputPort Serial
// ------------------ End declare section framework ------------------
// ------------------ Function description framework ------------------
boolean ConnectToWifiNetwork();
String GetParameterFromURL(String myURL,String myParameter) ;
// ------------------ End function description framework ------------------
// ------------------ Begin of project section ------------------




// ------------------ End of project section ------------------

// ------------------ Initialization section ------------------

void setup() {
// ------------------ Setup section for framework------------------ 
DebugOutputPort.begin(115200); // For debugging
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif
DebugOutputPort.setDebugOutput(true);
//ESP.wdtDisable(); // Used to debug, disable wachdog timer.
// Iets van flush doen......
DebugOutputPort.print("\n");
// ------------------ Connect to WIFI network ------------------
// Get or set the credentials of the WIFI network
WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
//WiFi.mode(WIFI_STA); // it is a good practice to make sure your code sets wifi mode how you want it.
if (!ConnectToWifiNetwork()) {
  DebugOutputPort.println("");
  DebugOutputPort.println("Could not connect to WIFI network!");
  DebugOutputPort.println("");
  delay(5000);
  ESP.reset(); // Reset and try again
  }
// ------------------ Startup OTA ------------------
ArduinoOTA.begin();
// ------------------ Setup section for project ------------------

// ------------------ End setup section for project ------------------
}

void loop() {
// put your main code here, to run repeatedly:
// ------------------ Startup OTA ------------------
ArduinoOTA.handle();

/* Get datetime from the Internet
# curl "http://worldtimeapi.org/api/timezone/Europe/Amsterdam"
{
  "abbreviation": "CET",
  "client_ip": "62.45.99.171",
  "datetime": "2023-11-28T11:57:58.549719+01:00",
  "day_of_week": 2,
  "day_of_year": 332,
  "dst": false,
  "dst_from": null,
  "dst_offset": 0,
  "dst_until": null,
  "raw_offset": 3600,
  "timezone": "Europe/Amsterdam",
  "unixtime": 1701169078,
  "utc_datetime": "2023-11-28T10:57:58.549719+00:00",
  "utc_offset": "+01:00",
  "week_number": 48
}
*/
  String myURL = "http://worldtimeapi.org/api/timezone/Europe/Amsterdam";
  String myParameter = "datetime";
  String Result;
  Result = GetParameterFromURL(myURL,myParameter);
  DebugOutputPort.println(Result);
  delay(10000);
  
  
}

// put function definitions here:

String GetParameterFromURL(String myURL,String myParameter){
  WiFiClient client;
  HTTPClient http;
  String ReturnValue;


  DebugOutputPort.print("[HTTP] begin...\n");
    if (http.begin(client, myURL)) {  // HTTP

      // start connection and send HTTP header
      DebugOutputPort.println("[HTTP] GET...\n");
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        DebugOutputPort.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String received = http.getString();
          // DebugOutputPort.println(received);
          const int capacity  = 512; //From JSON assistant
          StaticJsonDocument<capacity> doc;
          DeserializationError err =  deserializeJson(doc,received);
          if  (err) {
              DebugOutputPort.print(F("deserializeJson() failed with code "));
              DebugOutputPort.println(err.c_str());
          }
          // DebugOutputPort.println(doc[myParameter].as<const char*>());
          const char* tmpReturnValue =  doc[myParameter];
          ReturnValue=tmpReturnValue;
          // DebugOutputPort.println(ReturnValue);
        }
        } else {
          DebugOutputPort.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        DebugOutputPort.println("Close  HTTP here");
        
        http.end();
        
      
    } else {
      DebugOutputPort.println("[HTTP] Unable to connect");
    }

return ReturnValue;
}

// ------------------ Routine to connect to the WIFI network ------------------
boolean ConnectToWifiNetwork() {

  DebugOutputPort.println();
  DebugOutputPort.println();
  DebugOutputPort.print("Connecting to WIFI network");
  DebugOutputPort.println();

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  // New OOB ESP8266 has no Wi-Fi credentials so will connect and not need the next command to be uncommented and compiled in, a used one with incorrect credentials will
  // so restart the ESP8266 and connect your PC to the wireless access point called 'ESP8266_AP' or whatever you call it below in ""
  // Set a timeout until configuration is turned off, useful to retry or go to sleep in n-seconds
  wm.setTimeout(180);

  // reset settings - wipe credentials for testing
  // Command to be included if needed, then connect to http://192.168.4.1/ and follow instructions to make the WiFi connection
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  // res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res) {
    DebugOutputPort.println("Failed to connect");
    return false;
  }

  DebugOutputPort.println();
  DebugOutputPort.println("WiFi connected");
  DebugOutputPort.println();
  DebugOutputPort.print("SSID : ");
  DebugOutputPort.println(WiFi.SSID());
  DebugOutputPort.print("MAC address: ");
  DebugOutputPort.println(WiFi.macAddress());
  DebugOutputPort.print("IP address: ");
  DebugOutputPort.println(WiFi.localIP());
  DebugOutputPort.print("WiFi Signal strength: ");
  DebugOutputPort.print(WiFi.RSSI());
  DebugOutputPort.print(" dB");
  DebugOutputPort.println("");

  return true;
}
