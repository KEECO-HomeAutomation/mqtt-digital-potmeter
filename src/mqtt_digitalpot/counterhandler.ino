CounterHandler::CounterHandler() {
  last_IRQ = 0;
}

CounterHandler::CounterHandler(int a_Pin, int b_Pin) {
  last_IRQ = 0;
  aPin = a_Pin;
  bPin = b_Pin;
}

void CounterHandler::Init(int *ctr_p) {
  ctr_var = ctr_p;
  pinMode(aPin, INPUT);
  pinMode(bPin, INPUT);
}
void CounterHandler::IterateInLoop(void) {
  if (*ctr_var < 1) {
    *ctr_var = 1;
  }
  if (*ctr_var > 100) {
    *ctr_var = 100;
  }
}

bool CounterHandler::TimeToPublish(void) {
return (millis()-last_IRQ > 1000)? true:false;
}

void CounterHandler::SetPins(int a_Pin, int b_Pin) {
  aPin = a_Pin;
  bPin = b_Pin;
}
void CounterHandler::AssignCounterVar(int *ctr_p) {
  ctr_var = ctr_p;
}
void CounterHandler::CalcStatDir(void) {
  int sum = 0;
  if (millis() - last_IRQ > 1) {
    for (int i = 0; i < 13; i++) {
      sum += digitalRead(bPin);
    }
    if (sum >= 7) {
      Increment();
    }
    else {
      Decrement();
    }
    last_IRQ = millis();
  }
}
void CounterHandler::Increment(void) {
  (*ctr_var)++;
}

void CounterHandler::Decrement(void) {
  (*ctr_var)--;
}
