/*
   The code below is divided into 4 main segments:
     initIO()                   -   This code is called during device init. Place your init code here.
     IOprocessInLoop()          -   This code is called with every iteration in the main loop/
     timerCallback()            -   This code is called every N seconds (5 by default) using the timer
     mqttReceivedCallback(char* topic, byte* payload, unsigned int length)    -    This code is called everytime a subscribed MQTT topic receives a new message

   These additional helpers are available for you:
     CharToByte(char* chars, byte* bytes, unsigned int count)   -   Helper function for MQTT publish - publish requires byte array
     mqtt_send_buffer[64]                                       -   To temporarly store the message to be published
     announceNodeState()                                        -   Called during MQTT connection init. Can be called standalone at other times as well.
     appendSubtopic(char *topic)                                -   Appends the given topic to the UUID and stores the result in the topic. So all topics look like this: UUID/<status_topic>. Important to call once per topic only!
*/

/*
   Place your Arduino Shield specific #includes here
*/


/*
   When your device connects to an MQTT broker it announces its state in the UUID/<status_topic> topic
   Place your additional variable inits here
*/
#include "digitpothandler.h"
#include "counterhandler.h"
#include "displayhandler.h"

byte mqtt_send_buffer[64];
int ctr0 = 1;
int ctr1 = 1;

DisplayHandler dispHandl;

DigitpotHandler digitPot0(33, 32); //inc, dir pins
DigitpotHandler digitPot1(27, 14); //inc, dir pins

CounterHandler counter0(34, 35);
CounterHandler counter1(25, 26);

void IRAM_ATTR isr0() {
  dispHandl.ResetTimeout();
  counter0.CalcStatDir();
}

void IRAM_ATTR isr1() {
  dispHandl.ResetTimeout();
  counter1.CalcStatDir();
}

void initIO() {
  /*
    List the topics to subscribe in the array below. The UUID is automatically inserted before the topic(s) below
    Make sure to set the mqttSubTopicCount variable accordingly.
    Place your additional init code here.
  */

  dispHandl.PointToVariables(&ctr0, &ctr1);
  dispHandl.Init();
  digitPot0.Init(&ctr0);
  digitPot0.SetIterRate(50);
  digitPot1.Init(&ctr1);
  digitPot1.SetIterRate(50);

  counter0.Init(&ctr0);
  counter1.Init(&ctr1);

  attachInterrupt(counter0.aPin, isr0, FALLING);
  attachInterrupt(counter1.aPin, isr1, FALLING);
  delay(100);
  ctr0 = 20;
  ctr1 = 50;
  strcpy(espConfig.mqttSubTopic[0], "/command");
  strcpy(espConfig.mqttSubTopic[1], "/setLocalState");
  espConfig.mqttSubTopicCount = 2;
}


void IOprocessInLoop() {
  /*
     Place your code here that needs to be executed in every loop() iteration in the main
     To publish on MQTT use theis function:
     void mqttPublish(char* topic, char* text);
  */
  counter0.IterateInLoop();
  counter1.IterateInLoop();
  dispHandl.CheckStatus();
  digitPot0.IterateInLoop();
  digitPot1.IterateInLoop();
}


bool timerCallback(void *) {
  /*
     This function is called periodically defined by TIMERVALUE, 5000ms by default
  */
  //Serial.println("Called every 5 sec");
  return true;
}


void mqttReceivedCallback(char* subtopic, byte * payload, unsigned int length) {
  /*
     Called every time a message arrives to a subscribed MQTT topic
     If you subscribe to multiple topics please note that you need to manually select the correct topics here for your application with strcmp for example.
     char PDU[] contains the received byte array to char array so you can use strcmp and such.
  */
  char PDU[length];
  for (unsigned int i = 0; i < (length); i++) {
    PDU[i] = char(payload[i]);
    PDU[i + 1] = '\0';
  }
#ifdef DEBUG
  Serial.print("Received topic: ");
  Serial.println(subtopic);
  Serial.print("MQTT payload received: ");
  Serial.println(PDU);
#endif
}


/*
   HELPERS
   void mqttPublish(char* topic, char* text);
*/
