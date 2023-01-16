#include <Arduino.h>

const uint8_t DESKMODE_STANDBY_WARN = 0;    // Включается экран и пищит, когда тревога <- начальный
const uint8_t DESKMODE_STANDBY_MUTE = 1;    // Включается экран, но НЕ пищит, когда тревога
const uint8_t DESKMODE_OBSERVE_WARN = 2;    // Экран всегда работает. Пищит, когда тревога
const uint8_t DESKMODE_OBSERVE_MUTE = 3;    // Экран всегда работает. НЕ пищит, когда тревога
const uint8_t NUM_DESKMODES = DESKMODE_OBSERVE_MUTE+1;

class DeskMode {
  public:
    bool buzzerMute;
    bool alwaysDisplay;

    uint8_t curMode;

    DeskMode(uint8_t initMode) {
      curMode = initMode;
      activateMode();
    }

    void toNextMode() {
      curMode = (curMode+1)%NUM_DESKMODES;
      activateMode();
    }

    void activateMode() {
      Serial.printf("\nIoT Mode: %d\n", curMode+1);
      switch(curMode) {
        case DESKMODE_STANDBY_WARN: {
          buzzerMute = false;
          alwaysDisplay = false;
          break;
        }
        case DESKMODE_STANDBY_MUTE: {
          buzzerMute = true;
          alwaysDisplay = false;
          break;
        }
        case DESKMODE_OBSERVE_WARN: {
          buzzerMute = false;
          alwaysDisplay = true;
          break;
        }
        case DESKMODE_OBSERVE_MUTE: {
          buzzerMute = true;
          alwaysDisplay = true;
          break;
        }
      }

      for(int i = 0; i < curMode+1; i ++) {
        delay(50);
        digitalWrite(PIN_BUZZER, HIGH);
        delay(50);
        digitalWrite(PIN_BUZZER, LOW);
      }
    }
};