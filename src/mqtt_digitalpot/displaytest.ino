
#include "displayhandler.h"
#include "digitpothandler.h"
#include "counterhandler.h"

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

void setup() {
  dispHandl.PointToVariables(&ctr0, &ctr1);
  dispHandl.Init();
  Serial.begin(9600);

  digitPot0.Init(&ctr0);
  digitPot0.SetIterRate(50);
  digitPot1.Init(&ctr1);
  digitPot1.SetIterRate(50);

  counter0.Init(&ctr0);
  counter1.Init(&ctr1);

  attachInterrupt(counter0.aPin, isr0, FALLING);
  attachInterrupt(counter1.aPin, isr1, FALLING);
  delay(1000);
  ctr0 = 20;
  ctr1 = 50;
}


void loop() {
  counter0.IterateInLoop();
  counter1.IterateInLoop();
  dispHandl.CheckStatus();
  digitPot0.IterateInLoop();
  digitPot1.IterateInLoop();
  delay(1);
}
