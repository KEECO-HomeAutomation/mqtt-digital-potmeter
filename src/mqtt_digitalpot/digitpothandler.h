#ifndef DIGITPOT_H
#define DIGITPOT_H

class DigitpotHandler {
  public:    
    DigitpotHandler(void);
    DigitpotHandler(int inc, int dir);
    void Init(int *sp_p);
    void SetToZero(void);
    void IterateInLoop(void);
    void SetPins(int inc, int dir);
    void SetSetpoint(int sp);
    void AssignSetpointVar(int *sp_p);
    void SetIterRate(int rate);

  private:
    int *setpoint;
    int actual_value;
    int incPin;
    int dirPin;
    int iter_rate;
    long last_updated;
};
#endif
