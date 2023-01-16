
#include <Arduino.h>

#define DISPLAY_RELAY_ON HIGH
#define DISPLAY_RELAY_OFF LOW

#define MAX_TIME_DISPLAY_RELAY 1*60*1000

class DisplayRelay {
  uint8_t pin;
  bool isActive;
  uint32_t timer;
  uint32_t maxTime;

  public:
    DisplayRelay(uint8_t _pin) {
      pin = _pin;
      pinMode(pin, OUTPUT);

      isActive = false;
      digitalWrite(pin, LOW);

      timer = 0;
      maxTime = 0;
    }


    void setMode(bool newActive) {
      if(newActive == DISPLAY_RELAY_ON) {
        timer = 0;
        maxTime = MAX_TIME_DISPLAY_RELAY;
      } else {
        maxTime = 0;
      }
      isActive = newActive;
    }

    void update(uint32_t dt, bool alwaysOn) {
      timer += dt;

      if(alwaysOn) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
        return;
      }

      if(timer > maxTime) {
        isActive = DISPLAY_RELAY_OFF;
      }
      pinMode(pin, OUTPUT);
      digitalWrite(pin, isActive);
    }
    
};