#include "StatusLedController.h"
#include "Arduino.h"

/// @brief コンストラクタ
/// @param ledPin 状態表示LEDのピン番号
StatusLedController::StatusLedController(int ledPin)
{
    m_ledPin = ledPin;
    pinMode(m_ledPin, OUTPUT);
    m_ledBlinkCnt = 0;
    init();
}
/// @brief 初期化
void StatusLedController::init(void)
{
    if (m_ledPin == -1)
    {
        return;
    }
    clear();
    st = stBLINKING_STAY;
}
/// @brief 点滅のカウントをクリアする
void StatusLedController::clear(void)
{
    m_ledBlinkCnt = 0;
}
/// @brief LEDを点灯する
void StatusLedController::onLed(void)
{
    // LEDを点灯する
    st = stON;
    // カウントはクリアしておく
    clear();
}
/// @brief LEDを消灯する
void StatusLedController::offLed(void)
{
    // LEDを消灯する
    st = stOFF;
    // カウントはクリアしておく
    clear();
}
/// @brief handleEvent
void StatusLedController::handleEvent(void)
{
    unsigned long currentMillis = millis();
    switch (st)
    {
    case stBLINKING_BLINK:
        if (currentMillis - previousMillis >= LED_BLINK_TIME_S)
        {
            // save the last time you blinked the LED
            previousMillis = currentMillis;

            // if the LED is off turn it on and vice-versa:
            if (currentLedState == LOW)
            {
                m_ledBlinkCnt++;
                currentLedState = HIGH;
            }
            else
            {
                currentLedState = LOW;
            }
            if (m_ledBlinkCntMax < m_ledBlinkCnt)
            {
                m_ledBlinkCnt = 0;
                st = stBLINKING_STAY;
            }
            digitalWrite(m_ledPin, currentLedState);
        }
        break;
    case stBLINKING_STAY:
        digitalWrite(m_ledPin, LOW);
        if (currentMillis - previousMillis >= LED_BLINK_TIME_L)
        {
            previousMillis = currentMillis;
            st = stBLINKING_BLINK;
        }
        break;
    case stON:
        digitalWrite(m_ledPin, HIGH);
        break;
    case stOFF:
        digitalWrite(m_ledPin, LOW);
        break;
    default:
        break;
    }
}
/// @brief 点滅させる回数を設定する
/// @param cnt 点滅させる回数
void StatusLedController::setLedBlinkCount(int cnt)
{
    m_ledBlinkCntMax = cnt + 1;
}
/// @brief 点滅をスタートする
void StatusLedController::startBlink()
{
    st = stBLINKING_BLINK;
    currentLedState = HIGH;
}
/// @brief 点滅をストップする（デフォルトのON状態にする）
void StatusLedController::stopBlink()
{
    st = stON;
}