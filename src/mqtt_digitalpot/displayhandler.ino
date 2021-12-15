#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels..
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DisplayHandler::DisplayHandler() {
  switchoff_timeout = 5000;
  display_power = true;
  wifi_ok = false;
}

void DisplayHandler::PointToVariables(int *vol, int *fad, bool *power) {
  volume = vol;
  fader = fad;
  dev_pow = power;
}

void DisplayHandler::Init(void) {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
}

void DisplayHandler::DrawText(char* text) {
  display.clearDisplay();
  display.setCursor(2, 10);
  display.print(text);
  display.display();
}

void DisplayHandler::DrawSlider(int verpos, int current, int amount) {
  float posfloat = ((float)display.width() - 33.0) / 102.0 * (float)amount;
  float currfloat = ((float)display.width() - 33.0) / 102.0 * (float)current;
  int pos = (int)posfloat;
  int curr = (int)currfloat;
  display.setCursor(10, verpos - 1);
  display.print("F/R");
  display.drawRect(30, verpos, display.width() - 30, 5, SSD1306_WHITE);
  display.fillRect(30 + pos, verpos + 1, 4, 3, SSD1306_WHITE);
  if (verpos > current) {
    display.drawLine(30 + curr, verpos + 2, 30 + pos, verpos + 2, SSD1306_WHITE);
  }
  if (verpos < current) {
    display.drawLine(30 + pos, verpos + 2, 30 + curr, verpos + 2, SSD1306_WHITE);
  }

}

void DisplayHandler::DrawSliderFilled(int verpos, int current, int amount) {
  float posfloat = ((float)display.width() - 31.0) / 100.0 * (float)amount;
  float currfloat = ((float)display.width() - 31.0) / 100.0 * (float)current;
  int pos = (int)posfloat;
  int curr = (int)currfloat;
  display.setCursor(10, verpos - 1);
  display.print("Vol");
  display.drawRect(30, verpos, display.width() - 30, 5, SSD1306_WHITE);
  //display.fillRect(31, verpos + 1, pos, 3, SSD1306_WHITE);
  display.drawRect(31, verpos+1, pos, 3, SSD1306_WHITE);
  if (verpos > current) {
    display.drawLine(30 + curr, verpos + 2, 30 + pos, verpos + 2, SSD1306_WHITE);
  }
  if (verpos < current) {
    display.drawLine(30 + pos, verpos + 2, 30 + curr, verpos + 2, SSD1306_WHITE);
  }
}

void DisplayHandler::DrawPowerStatus(bool stat) {
  if (stat) {
    display.fillRect(1, 1, 3, 30, SSD1306_WHITE);
  }
  else {
    display.drawRect(1, 1, 3, 30, SSD1306_WHITE);
  }
}

void DisplayHandler::DrawConnectionStatus(bool stat) {
  if (stat) {
    display.drawLine(5, 1, 5, 30, SSD1306_WHITE);
  }
}

void DisplayHandler::ResetTimeout(void) {
  lastupdated = millis();
  display_power = true;
}

void DisplayHandler::CheckStatus(int volcur, int fadcur) {
  if (display_power) {
    display.clearDisplay();
    //configure the screen here
    DrawSliderFilled(6, volcur, *volume);
    DrawSlider(20, fadcur, *fader);
    DrawPowerStatus(*dev_pow);
    DrawConnectionStatus(wifi_ok);
    if (millis() - lastupdated > switchoff_timeout) {
      display_power = false;
      display.clearDisplay();
    }
    display.display();
  }
}

bool DisplayHandler::GetPowerstat(void) {
  return display_power;
}

void DisplayHandler::SetWifiStatus(bool stat) {
  wifi_ok = stat;
}
