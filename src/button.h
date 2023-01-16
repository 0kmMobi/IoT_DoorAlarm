#include <Arduino.h>

const int8_t STATUS_ACTIVE = 1;
const int8_t STATUS_DISACTIVE = 0;
const int8_t STATUS_WO_CHANGES = -1;

const uint32_t CONTACT_BOUNCE_MAX_TIME = 500;

/**
 * Это и Кнопка и любой другой бинарный датчик, например, PIR-сенсор
 */
class Button {
    uint8_t pin;
    bool pullUp;
    bool status; // Кнопка должна быть опущена, прежде чем нажаться вновь

  public:
    int32_t delayTimerMSec = 5000; // Отложенный старт. Например, PIR-сенсор после включения питания в течении нескольких секунд несколько раз срабатывает 
    String name;

    Button(uint8_t _pin, bool _pullUp, String _name) {
      pin = _pin;
      pullUp = _pullUp;
      name = _name;
      status = false;
      pinMode(pin, pullUp?INPUT_PULLUP:INPUT);
    }

    /**
     * Отлавливаем только Новые нажатия. Т.е. если кнопка была Нажата и в прошлой проверке, то возвращаем false
     * Т.е. возвращаем true только тогда, когда кнопка только нажалась
     */
    int8_t checkActive(uint32_t dt) {
      if(delayTimerMSec > 0) {
        //Serial.printf("Pin: %d Start delay= %d\n", pin, delayStartTimerMSec);
        delayTimerMSec -= dt;
        if(delayTimerMSec <= 0) {
//          pinMode(pin, pullUp?INPUT_PULLUP:INPUT);
          // Serial.printf("Activate Pin: %d\n", pin);
        }
        return STATUS_WO_CHANGES;
      }

      bool value = digitalRead(pin) == HIGH;
      bool newStatus = pullUp? !value: value;

      if(status != newStatus) {
        status = newStatus;
        delayTimerMSec = CONTACT_BOUNCE_MAX_TIME;
        return status? STATUS_ACTIVE: STATUS_DISACTIVE;
      }
      return STATUS_WO_CHANGES;
    }
};
