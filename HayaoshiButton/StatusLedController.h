#ifndef STATUS_LED_CONTROLLER_H
#define STATUS_LED_CONTROLLER_H

#define LED_BLINK_TIME_S 200
#define LED_BLINK_TIME_L 800

/// @brief m_ledBlinkCntMaxだけ点滅して休みを繰り返す
class StatusLedController
{
public:
  StatusLedController(int ledPin);
  void init();
  void handleEvent();
  void onLed();
  void offLed();
  void startBlink();
  void stopBlink();
  void setLedBlinkCount(int cnt);

private:
  void clear();
  int m_ledPin = -1;
  int m_ledBlinkCnt = 0;
  int m_ledBlinkCntMax = -1;
  unsigned long previousMillis = 0;
  int currentLedState = 0;
  typedef enum state
  {
    stBLINKING_BLINK = 0,
    stBLINKING_STAY,
    stON,
    stOFF,

    stNONE = -1
  } State;
  State st = stNONE;
};

#endif // STATUS_LED_CONTROLLER_H