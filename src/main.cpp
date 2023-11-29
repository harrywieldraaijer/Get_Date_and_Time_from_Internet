/*

   Module: Get date, time etc. from the internet

   Master Code: 20231127/2

   Project Location:

   Created by: Harry Wieldraaijer

   Created on: 28-November-2023
   
   Short description:

   Connect to WiFi with stored password, if no stored password enter it via an webbrowser.
   Get date, time etc. from the internet

   Date last modified: 01-Januari-2021  - Version 1 
   Date last modified: 28-November-2023 - Initial and original release
  

*/


/* ------------------ Hardware notes ------------------
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


// ------------------ Include and describe libraries ------------------
#include <Arduino.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

// ------------------ Begin of project section ------------------
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

// ------------------ End of project section ------------------


// ------------------ Declare section ------------------
#define DebugOutputPort Serial




// ------------------ Function description ------------------


// put function declarations here:

boolean ConnectToWifiNetwork();
// ------------------ Begin of project section ------------------

void objRec(JSONVar myObject);

String GetParameterFromURL(String myURL,String myParameter) ;

// ------------------ End of project section ------------------

// ------------------ Initialization section ------------------

void setup() {
  

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

  // ------------------ Begin of project section ------------------

  // ------------------ End of project section ------------------

}

void loop() {
  // put your main code here, to run repeatedly:
  String myURL = "http://worldtimeapi.org/api/timezone/Europe/Amsterdam";
  String myParameter = "";
  String Result;
  Result = GetParameterFromURL(myURL,myParameter);
  delay(10000);
  
  
}

// put function definitions here:

String GetParameterFromURL(String myURL,String myParameter){
  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
    if (http.begin(client, myURL)) {  // HTTP


      DebugOutputPort.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        // DebugOutputPort.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          JSONVar payload = http.getString();
          DebugOutputPort.println("We are here");
          DebugOutputPort.println(payload);
          DebugOutputPort.println("We were here");
          objRec(payload);
        }
      } else {
        DebugOutputPort.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      // DebugOutputPort.println(payload);
      http.end();
      DebugOutputPort.println("Do we come here");
      
    } else {
      DebugOutputPort.println("[HTTP] Unable to connect");
    }

return "";
}

void objRec(JSONVar myObject) {
  Serial.println(myObject);
   Serial.println("{");
  for (int x = 0; x < myObject.keys().length(); x++) {
    // if ((JSON.typeof(myObject[myObject.keys()[x]])).equals("object")) {
      Serial.print(myObject.keys()[x]);
      Serial.println(" : ");
      objRec(myObject[myObject.keys()[x]]);
    //}
    // else {
      Serial.print(myObject.keys()[x]);
      Serial.print(" : ");
      Serial.println(myObject[myObject.keys()[x]]);
    }
  //}
  Serial.println("}");
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
