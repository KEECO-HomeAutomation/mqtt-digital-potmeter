#ifndef DISPLAY_H
#define DISPLAY_H

class DisplayHandler {
  public:
    int switchoff_timeout;
    int* volume;
    int* fader;


    DisplayHandler(void);
    void Init(void);
    void PointToVariables(int *vol, int *fad);
    void DrawSlider(int verpos, int amount);
    void DrawSliderFilled(int verpos, int amount);
    void DrawConnectionStatus(bool stat);
    void ResetTimeout(void);
    void CheckStatus(void);
    bool GetPowerstat(void);
    void SetWifiStatus(bool stat);

  private:
    long lastupdated;
    bool display_power;
    bool wifi_ok;
};
#endif
