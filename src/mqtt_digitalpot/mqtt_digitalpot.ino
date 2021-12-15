/*
   KEECO HW Node application software
   Version 3.9
   Developed by https://github.com/litechniks

   Only Manage_IO needs to be modified to implement your application
   Modified for NodeMCU ESP32S HW


*/

#include "Arduino.h"
#include <WiFi.h>
//#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <arduino-timer.h>             //https://github.com/contrem/arduino-timer
#include <FS.h>
#include "SPIFFS.h"
#include <PubSubClient.h>      //https://pubsubclient.knolleary.net/api.html
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "configFileHandler.h"
#include "mqtthandler.h"
#include <stdio.h>



#define DEBUG     //to enable debug purpose serial output 
#define OTA       //to enable OTA updates
#define CSS       //makes the web interface nicer but also slower and less reliable
#define TIMERVALUE 1000

const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "admin";

WebServer webserver(80);

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

ConfigurationHandler espConfig;
MqttHandler mh;


//timer for various tasks - for future scalability
auto timer = timer_create_default();

//https://bit.ly/2WPt42i


/*
  Initializing the KEECO HW Node
  Serial Timeout is needed for the serial command terminal
  LittleFS - is used to store the configuration data - SSID, Password, mqtt_server, UUID
  espConfig - global object storing the configuration variables
  initWifiOnBoot - try to connect to Infrastructure WiFi (60 sec timeout). If not successful start AP mode. Later if disconnected from STA then AP reactivates. Also starts mDNS.
  initWebserver - webserver to set configuration parameters
  initMqtt - connect to the set mqtt server. Name is resolved with mDNS. Set subscriptions.
  initIO - place your custom init code in this function
  InitOTA - initializing OTA
  timer - setup a timer that calls publishIO() - place your periodically called code there

*/
void setup() {
  Serial.setTimeout(10);
  Serial.begin(115200);
  while (!Serial);
  initDisplay();
  Serial.println("Starting KEECO Node...");
  SPIFFS.begin(true);                 //added true so that it will format if needed
  Serial.println("[=_______]");
  espConfig.initConfiguration();
  mh.setConfigFileHandler(&espConfig);
  Serial.println("[==______]");
  initWifiOnBoot();
  Serial.println("[===_____]");
  initWebserver();
  Serial.println("[====____]");
  initIO();
  Serial.println("[=====___]");
  mh.initMqtt();
  Serial.println("[======__]");
  InitOTA();
  Serial.println("[=======_]");
  timer.every(TIMERVALUE, timerCallback);
  Serial.println("[========]");
  Serial.println("Welcome - ver 3.2!");
  Serial.println("Help: {\"command\":\"help\"}");
  Serial.println("https://bit.ly/2WPt42i");
  delay(1000);
}

void loop() {
  timer.tick();
  webserverInLoop();
  mh.mqttInLoop();
  IOprocessInLoop();
  OTAInLoop();
  espConfig.serialCmdCheckInLoop();
}
