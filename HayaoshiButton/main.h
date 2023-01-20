#ifndef HAYAOSHI_BUTTON_H
#define HAYAOSHI_BUTTON_H

#define STATUS_LED_PIN 2
#define SOUND_OUT_PIN 3
#define CORRECT_PIN 4
#define WRONG_PIN 5

#define IOpinCnt 8
#define BUTTON_PIN_0 6
#define BUTTON_PIN_1 7
#define BUTTON_PIN_2 8
#define BUTTON_PIN_3 9
#define BUTTON_PIN_4 14
#define BUTTON_PIN_5 15
#define BUTTON_PIN_6 16
#define BUTTON_PIN_7 17

#define SER_74HC595 12   // Data pin of 74HC595
#define RCLK_74HC595 11  // Latch pin of 74HC595
#define SRCLK_74HC595 13 // Clock pin of 74HC595

int ButtonPins[IOpinCnt] = {
  BUTTON_PIN_0, BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3,
  BUTTON_PIN_4, BUTTON_PIN_5, BUTTON_PIN_6, BUTTON_PIN_7
};

typedef enum event
{
  evBUTTON_PUSHED = 0,
  evRESET_PUSHED,
  evCORRECT_PUSHED,
  evWRONG_PUSHED,
  evINIT_END,

  evNONE = -1
} Event;

typedef enum state
{
  stTURN_ON = 0,
  stSTANDBY,
  stANSWER,

  stNONE = -1
} State;





#endif