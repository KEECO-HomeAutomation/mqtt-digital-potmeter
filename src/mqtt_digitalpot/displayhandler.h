#ifndef DISPLAY_H
#define DISPLAY_H

class DisplayHandler {
  public:
    int switchoff_timeout;
    int* volume;
    int* fader;
    bool* dev_pow;


    DisplayHandler(void);
    void Init(void);
    void PointToVariables(int *vol, int *fad, bool *power);
    void DrawText(char* text);
    void DrawSlider(int verpos, int current, int amount);
    void DrawSliderFilled(int verpos, int current, int amount);
    void DrawConnectionStatus(bool stat);
    void DrawPowerStatus(bool stat);
    void ResetTimeout(void);
    void CheckStatus(int volcur, int fadcur);
    bool GetPowerstat(void);
    void SetWifiStatus(bool stat);

  private:
    long lastupdated;
    bool display_power;
    bool wifi_ok;
};
#endif
