#include "main.h"
#include "my_sound.h"
#include "StatusLedController.h"

int PushedButtonIdx = -1;
Event ev = evNONE;
State st = stNONE;
// StatusLedController ledController(STATUS_LED_PIN);
StatusLedController* ledController;

void setup()
{
  // put your setup code here, to run once:
  // Stateの初期化
  st = stTURN_ON;

  // デバッグ用出力の有効化
  Serial.begin(115200);
  while (!Serial)
  {
  }

  Serial.println("setup start");

  // ステータス表示用LEDの初期化
  // ledController.init();
  // ledController.onLed();
  ledController = new StatusLedController(STATUS_LED_PIN);

  // 必要なピンの割り込みを有効にする
  PCICR |= B00000111;  // [X X X X X PCMSK2 PCMSK1 PCMSK0]
  PCMSK0 |= B00000011; // PCINT0_vect[X X SCK MISO MOSI D10 D9 D8] // SCKはまともに動作しなさそう
  PCMSK1 |= B00001111; // PCINT1_vect[X RESET A5 A4 A3 A2 A1 A0]
  PCMSK2 |= B11110000; // PCINT2_vect[D7 D6 D5 D4 D3 D2 D1 D0]

  // Set all the pins of 74HC595 as OUTPUT
  pinMode(SER_74HC595, OUTPUT);
  pinMode(RCLK_74HC595, OUTPUT);
  pinMode(SRCLK_74HC595, OUTPUT);
  clearLeds();

  // 音の再生の初期設定
  initSoundOutput();  
  
  ev = evINIT_END;
}

void loop()
{
  switch (st)
  {
  case stTURN_ON:
    switch (ev)
    {
    case evINIT_END:
      ev = evNONE;
      Serial.println("setup end");
      ledController->onLed();
      st = stSTANDBY;
      break;
    default:
      break;
    }
  case stSTANDBY:
    switch (ev)
    {
    case evBUTTON_PUSHED:
      ev = evNONE;
      // 何番が押されたかチェックする
      for (size_t i = 0; i < IOpinCnt; i++)
      {
        if (digitalRead(ButtonPins[i]) == LOW)
        {
          PushedButtonIdx = i;
          Serial.print(PushedButtonIdx);
          Serial.println(" was pushed");
          break;
        }
      }

      if (PushedButtonIdx != -1)
      {
        // 対応するLEDを点灯する
        LedON(PushedButtonIdx);
        // 音声を再生する
        playSound(sound_buzzer, sound_buzzer_len);
        // 状態表示用LEDを点滅状態にする
        ledController->setLedBlinkCount(PushedButtonIdx);
        ledController->startBlink();
        // 解答中状態に遷移する
        st = stANSWER;
      }
      break;
    default:
      break;
    }
    break;
  case stANSWER:
    switch (ev)
    {
    case evBUTTON_PUSHED:
      ev = evNONE;

      // 押されたのがリセットボタンかチェックする
      if (digitalRead(CORRECT_PIN) == LOW)
      {
        PushedButtonIdx = -1;
        ev = evCORRECT_PUSHED;
        break;
      }
      if (digitalRead(WRONG_PIN) == LOW)
      {
        PushedButtonIdx = -1;
        ev = evWRONG_PUSHED;
        break;
      }
      break;
    case evCORRECT_PUSHED:
      Serial.print("stANSWER");
      Serial.println(" evCORRECT_PUSHED");
      ev = evNONE;
      // LEDを消灯する
      clearLeds();
      // 音を再生する
      playSound(sound_correct, sound_correct_len);
      // 点滅を止める
      ledController->stopBlink();
      // ボタン待機状態に戻る
      st = stSTANDBY;
      break;
    case evWRONG_PUSHED:
      Serial.print("stANSWER");
      Serial.println(" evWRONG_PUSHED");
      ev = evNONE;
      // LEDを消灯する
      clearLeds();
      // 音を再生する
      playSound(sound_wrong, sound_wrong_len);
      // 点滅を止める
      ledController->stopBlink();
      // ボタン待機状態に戻る
      st = stSTANDBY;
      break;

    default:
      break;
    }
    break;
  default:
    break;
  }
  ledController->handleEvent();
}

// PCINT2のいずれかのピンに割り込み（CHANGE)が発生したときに呼ばれる関数
ISR(PCINT0_vect)
{
  ev = evBUTTON_PUSHED;
}
// PCINT2のいずれかのピンに割り込み（CHANGE)が発生したときに呼ばれる関数
ISR(PCINT1_vect)
{
  ev = evBUTTON_PUSHED;
}
// PCINT2のいずれかのピンに割り込み（CHANGE)が発生したときに呼ばれる関数
ISR(PCINT2_vect)
{
  ev = evBUTTON_PUSHED;
}

// 音声の再生
// 初期設定
void initSoundOutput()
{
  Serial.println("initSoundOutput start");
  pinMode(SOUND_OUT_PIN, OUTPUT);
  digitalWrite(SOUND_OUT_PIN, LOW);
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  Serial.println("initSoundOutput end");
}

// 音声を再生する
void playSound(const unsigned char sound[], int len)
{
  Serial.println("play sound");
  for (int i = 0; i < len; i++)
  {
    OCR2B = pgm_read_byte_near(&sound[i]);
    delayMicroseconds(100);
  }
  // OCR2B = pgm_read_byte_near(0x00);
  Serial.println("play sound end");
}

// すべてのLEDを消灯する
void clearLeds()
{
  byte leds = 0;
  updateShiftRegister(leds);
}

void LedON(int i)
{
  byte leds = 0;
  leds = 1 << i;
  updateShiftRegister(leds);
}
/*
   updateShiftRegister() - This function sets the latchPin to low, then calls the Arduino function 'shiftOut' to shift out contents of variable 'leds' in the shift register before putting the 'latchPin' high again.
*/
void updateShiftRegister(byte leds)
{
  digitalWrite(RCLK_74HC595, LOW);
  shiftOut(SER_74HC595, SRCLK_74HC595, LSBFIRST, leds);
  digitalWrite(RCLK_74HC595, HIGH);
}