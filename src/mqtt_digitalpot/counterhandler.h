#ifndef COUNTERHANDLER_H
#define COUNTERHANDLER_H

class CounterHandler {
  public:
    int aPin;
    int bPin;
    CounterHandler(void);
    CounterHandler(int a_Pin, int b_Pin);
    void Init(int *ctr_p);
    void IterateInLoop(void);
    void SetPins(int a_Pin, int b_Pin);
    void AssignCounterVar(int *ctr_p);
    void CalcStatDir(void);
    void Increment(void);
    void Decrement(void);
    bool TimeToPublish(void);

  private:
    int *ctr_var;
    long last_IRQ;
};
#endif
