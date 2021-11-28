
DigitpotHandler::DigitpotHandler() {
  actual_value = 1;
  iter_rate = 0;
  last_updated = 0;
}

DigitpotHandler::DigitpotHandler(int inc, int dir) {
  actual_value = 1;
  iter_rate = 0;
  last_updated = 0;
  incPin = inc;
  dirPin = dir;
}

void DigitpotHandler::SetPins(int inc, int dir) {
  incPin = inc;
  dirPin = dir;
}

void DigitpotHandler::SetIterRate(int rate) {
  iter_rate = rate;
}

void DigitpotHandler::SetSetpoint(int sp) {
  *setpoint = sp;
}

int DigitpotHandler::GetActualValue(void) {
  return actual_value;
}

void DigitpotHandler::AssignSetpointVar(int *sp_p) {
  setpoint = sp_p;
}

void DigitpotHandler::Init(int *sp_p) {
  setpoint = sp_p;
  pinMode(incPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  SetToZero();
}

void DigitpotHandler::SetToZero(void) {
  digitalWrite(dirPin, LOW);
  delayMicroseconds(50);
  for (int i = 0; i < 101; i++) {
    digitalWrite(incPin, HIGH);
    delayMicroseconds(50);
    digitalWrite(incPin, LOW);
    delayMicroseconds(50);
  }
}

void DigitpotHandler::IterateInLoop(void) {
  if (millis() - last_updated > iter_rate) {
    last_updated = millis();
    if (*setpoint != actual_value) {
      Serial.print("Setting resistance: ");
      Serial.println(actual_value);
      if (*setpoint > actual_value) {
        digitalWrite(dirPin, HIGH);
        actual_value++;
      }
      else {
        digitalWrite(dirPin, LOW);
        actual_value--;
      }
      if (actual_value < 1) {
        actual_value = 1;
      }
      if (actual_value > 100) {
        actual_value = 100;
      }
      delayMicroseconds(50);
      digitalWrite(incPin, HIGH);
      delayMicroseconds(50);
      digitalWrite(incPin, LOW);
      delayMicroseconds(50);
    }
  }
}
