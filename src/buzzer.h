
#include <Arduino.h>

#define BUZZER_MODE_NONE     0
#define BUZZER_MODE_PIR_SENS 1
#define BUZZER_MODE_BTN_BELL 2

#define MAX_TIME_BUZZER_MODE_PIR_SENS 1*7*1000
#define MAX_TIME_BUZZER_MODE_BTN_BELL 1*5*1000

class Buzzer {
  uint8_t pin;
  uint8_t mode;
  uint32_t timer;
  uint32_t maxTime;

  public:
    Buzzer(uint8_t _pin) {
      pin = _pin;
      pinMode(pin, OUTPUT);

      mode = BUZZER_MODE_NONE;
      digitalWrite(pin, LOW);

      timer = 0;
      maxTime = 0;
    }

    void setMode(uint8_t newMode) {
      if(newMode == BUZZER_MODE_NONE) {
        digitalWrite(pin, LOW);
        maxTime = 0;
      } else {
        timer = 0;
        switch(newMode) {
          case BUZZER_MODE_PIR_SENS:
            if(maxTime < MAX_TIME_BUZZER_MODE_PIR_SENS)
              maxTime = MAX_TIME_BUZZER_MODE_PIR_SENS;
          break;
          case BUZZER_MODE_BTN_BELL:
            if(maxTime < MAX_TIME_BUZZER_MODE_BTN_BELL)
              maxTime = MAX_TIME_BUZZER_MODE_BTN_BELL;
          break;
        }
      }
      mode = newMode;
    }


    void update(uint32_t dt, bool mute) {
      timer += dt;

      if(mute) {
        digitalWrite(pin, LOW);
        return;
      }

      if(timer > maxTime)
        mode = BUZZER_MODE_NONE;

      switch(mode) {
        case BUZZER_MODE_NONE: {
          digitalWrite(pin, LOW);
          break; 
        }
        case BUZZER_MODE_PIR_SENS: {
          uint32_t xTime = timer / 50;
          if(xTime%4 == 0)
            digitalWrite(pin, HIGH);
          else
            digitalWrite(pin, LOW);
          break; 
        }
        case BUZZER_MODE_BTN_BELL: {
          digitalWrite(pin, HIGH);
          break; 
        }
      }
    }
};