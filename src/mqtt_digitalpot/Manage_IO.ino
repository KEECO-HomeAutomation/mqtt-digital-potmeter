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
#define POWPIN 17   //On-Off toggle button
#define RELAYPIN 16 //Relay control pin

byte mqtt_send_buffer[64];
int ctr0 = 1;
int ctr0_lastsent;
long last_sent_at0;
int ctr1 = 1;
int ctr1_lastsent;
long last_sent_at1;

bool dev_power = false;
bool dev_power_prev = false;
long last_pow_int = 0;

DisplayHandler dispHandl;

DigitpotHandler digitPot0(32, 33); //inc, dir pins
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

void IRAM_ATTR powisr() {
  if ((millis() - last_pow_int) > 300) {
    last_pow_int = millis();
    dev_power = !dev_power;
  }
}

void initDisplay() {
  dispHandl.PointToVariables(&ctr0, &ctr1, &dev_power);
  dispHandl.Init();
  dispHandl.DrawText("Connecting to WiFi..");
}

void initIO() {
  /*
    List the topics to subscribe in the array below. The UUID is automatically inserted before the topic(s) below
    Make sure to set the mqttSubTopicCount variable accordingly.
    Place your additional init code here.
  */

  ctr0_lastsent = 0;
  last_sent_at0 = 0;
  ctr1_lastsent = 0;
  last_sent_at1 = 0;

  digitPot0.Init(&ctr0);
  digitPot0.SetIterRate(8);
  digitPot1.Init(&ctr1);
  digitPot1.SetIterRate(8);

  counter0.Init(&ctr0);
  counter1.Init(&ctr1);

  pinMode(POWPIN, INPUT);
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);

  attachInterrupt(counter0.aPin, isr0, FALLING);
  attachInterrupt(counter1.aPin, isr1, FALLING);
  attachInterrupt(POWPIN, powisr, FALLING);
  strcpy(espConfig.mqttSubTopic[0], "/volume");
  strcpy(espConfig.mqttSubTopic[1], "/fader");
  strcpy(espConfig.mqttSubTopic[2], "/power");
  espConfig.mqttSubTopicCount = 3;
}


void IOprocessInLoop() {
  /*
     Place your code here that needs to be executed in every loop() iteration in the main
     To publish on MQTT use theis function:
     void mqttPublish(char* topic, char* text);
  */
  counter0.IterateInLoop();
  counter1.IterateInLoop();
  handlePowerStatus();
  dispHandl.CheckStatus(digitPot0.GetActualValue(), digitPot1.GetActualValue());
  digitPot0.IterateInLoop();
  digitPot1.IterateInLoop();

  sendOnceAfterawhile("/volume_pub", ctr0, &ctr0_lastsent, &last_sent_at0, 1000);
  sendOnceAfterawhile("/fader_pub", ctr1, &ctr1_lastsent, &last_sent_at1, 1000);
}


bool timerCallback(void *) {
  /*
     This function is called periodically defined by TIMERVALUE, 5000ms by default
  */
  //Serial.println("Called every 5 sec");
  ctr0 = 20;
  ctr1 = 50;

  mh.mqttPublish("/power_pub", "0");

  dispHandl.ResetTimeout();
  return false;
}


void mqttReceivedCallback(char* subtopic, byte * payload, unsigned int length) {
  /*
     Called every time a message arrives to a subscribed MQTT topic
     If you subscribe to multiple topics please note that you need to manually select the correct topics here for your application with strcmp for example.
     char PDU[] contains the received byte array to char array so you can use strcmp and such.
  */
  char PDU[length];
  int rec_val = 0;
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
  rec_val = atoi(PDU);
  rec_val = (rec_val > 100) ? 100 : rec_val;
  rec_val = (rec_val < 1) ? 1 : rec_val;

  if (strcmp(subtopic, "/volume") == 0) {
    ctr0 = rec_val;
  }
  if (strcmp(subtopic, "/fader") == 0) {
    ctr1 = rec_val;
  }
  if (strcmp(subtopic, "/power") == 0) {
    if (strcmp(PDU, "1") == 0 ) {
      dev_power = true;
    }
    else {
      dev_power = false;
    }
  }
  dispHandl.ResetTimeout();
}

void sendOnceAfterawhile(char* topic, int val, int *prev_val, long *last_sent, int timeout) {
  char numstring[10];
  if ((val != *prev_val) && ( millis() - *last_sent > timeout)) {
    *prev_val = val;
    *last_sent = millis();
    sprintf(numstring, "%d", val);
    mh.mqttPublish(topic, numstring);
    dispHandl.SetWifiStatus(espConfig.statuses.wifiIsConnected);
  }
}

void handlePowerStatus() {
  if (dev_power_prev != dev_power) {
    dev_power_prev = dev_power;
    if (dev_power) {
      mh.mqttPublish("/power_pub", "1");
      digitalWrite(RELAYPIN, HIGH);
    }
    else {
      mh.mqttPublish("/power_pub", "0");
      digitalWrite(RELAYPIN, LOW);
    }
    dispHandl.ResetTimeout();
  }
}


/*
   HELPERS
   void mqttPublish(char* topic, char* text);
*/
