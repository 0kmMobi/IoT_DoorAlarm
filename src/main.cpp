#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "led_blink.h"
#include "wifi_web_server.h"
#include "eeprom_storage.h"
#include "wifi_station.h"
#include "firebase_manager.h"
#include "button.h"
#include "buzzer.h"
#include "display_relay.h"
#include "desk_mode.h"
#include "consts.h"


const uint8_t STATE_START = 0;
const uint8_t STATE_WIFI_CONNECT = 1;
const uint8_t STATE_NEW_DEVICE_SERVER = 2;
const uint8_t STATE_MAIN_WORK = 4;

uint8_t curState;



LedBlink *led;
Button *sensorPIR;
Button *btnFlash;
Button *btnBell;
Button *btnMode;
Button *btnDoor;

Buzzer *buzzer;
DisplayRelay *displayRelay;
DeskMode *deskMode;

WifiStation *wifiStation;
WifiWebServer *webServer;

FirebaseManager *firebaseManager;

bool iotActivationSendToDB = false;

uint32_t mainLoopTimer;

uint32_t updateFrameTime() {
  uint32_t delta = millis() - mainLoopTimer;
  mainLoopTimer = millis();
  return delta;
}


void changeState(uint8_t newState) {
  if(curState == newState)
    return;

  // Clearing after old state
  switch(curState) {
    case STATE_WIFI_CONNECT:
      delete led;
    break;
    case STATE_NEW_DEVICE_SERVER:
      delete led;
      delete webServer;
    break;
    case STATE_MAIN_WORK:
      delete led;
    break;
  }

  // Initializing for new state
  switch(newState) {
    case STATE_WIFI_CONNECT: {
      wifiStation = new WifiStation();
      wifiStation->initWiFiConnection();
      Phase tasks[] = {Phase(100, LOW), Phase(100, HIGH), Phase(100, LOW), Phase(100, HIGH), Phase(2000, LOW)};
      led = new LedBlink(PIN_LED_BUILTIN, tasks, 5, true);
      led->start();
      break;
    }
    case STATE_NEW_DEVICE_SERVER: {
      delete wifiStation;

      Serial.println("Init wifi web server.");
      Phase tasks[] = {Phase(50, LOW), Phase(50, HIGH)};
      led = new LedBlink(PIN_LED_BUILTIN, tasks, 2, true);
      led->start();
      webServer = new WifiWebServer();
      break;
    }
    case STATE_MAIN_WORK: {
      Serial.println("Init main mode.");

      Phase tasks[] = {Phase(2000, LOW), Phase(200, HIGH)};
      led = new LedBlink(PIN_LED_BUILTIN, tasks, 2, true);
      led->start();

      String mac = wifiStation->getCompressedMAC();
      Serial.printf("  MAC Address: %s\n", mac.c_str() );
      Serial.printf("  ChipId as a 32-bit integer: %08X\n", ESP.getChipId() );
      Serial.printf("  Flash chipId as a 32-bit integer: %08X\n", ESP.getFlashChipId() );
      Serial.printf("  Flash chip frequency: %d Hz\n", ESP.getFlashChipSpeed() );
      Serial.printf("  Flash chip size: %d bytes\n", ESP.getFlashChipSize() );
      Serial.printf("  Free heap size: %d bytes\n", ESP.getFreeHeap() );

      firebaseManager = new FirebaseManager(mac);
      firebaseManager->sendDeviceInfo();

      uint8_t sensorsNumber = 4;
      String *sensorsNames = new String[sensorsNumber];
        sensorsNames[0] = sensorPIR->name;
        sensorsNames[1] = btnBell->name;
        sensorsNames[2] = btnDoor->name;
        sensorsNames[3] = SIGNAL_NAME_IOT_ACTIVATE;

      firebaseManager->sendSensorsList(sensorsNames, sensorsNumber);

      delete []sensorsNames;
      Serial.println("After init Sensors and firebase");
      break;
    }
  }
  curState = newState;
}


void setup() {
  Serial.begin(115200);
  Serial.println("");

  buzzer = new Buzzer(PIN_BUZZER);
  displayRelay = new DisplayRelay(PIN_RELAY_DISPLAY);
  deskMode = new DeskMode(DESKMODE_STANDBY_WARN);

  sensorPIR = new Button(PIN_PIR_SENSOR, false, SIGNAL_NAME_PIR_SENSOR);
    sensorPIR->delayTimerMSec = 17000;
  btnFlash = new Button(PIN_BTN_FLASH, true, "");
  btnBell = new Button(PIN_BTN_BELL, true, SIGNAL_NAME_BTN_BELL);
  btnMode = new Button(PIN_BTN_MODE, true, "");
  btnDoor = new Button(PIN_BTN_DOOR, true, SIGNAL_NAME_BTN_DOOR);
    btnDoor->checkActive( 100 ); // Нужно пропустить 1й опрос значения, т.к. проверять будем на "неизменность"

  bool hasWiFiData = eepromStorage.read();
  Serial.printf("EEPROMStorage Read result: %s\n", hasWiFiData ? "true" : "false");

  if(hasWiFiData)
    changeState(STATE_WIFI_CONNECT);
  else
    changeState(STATE_NEW_DEVICE_SERVER);

  mainLoopTimer = millis();
}




void loop() {
  delay(10);
  uint32_t dt = updateFrameTime();

  switch(curState) {
    case STATE_WIFI_CONNECT: {
      led->update( dt );
      int8_t wifiConnectRes = wifiStation->updateWifiConnection( dt );
      switch(wifiConnectRes) {
        case WIFI_CONNECTION_SUCCESS:
          changeState(STATE_MAIN_WORK);
          break;
        case WIFI_CONNECTION_FAILURE: // Так никуда и не подключились. Теперь делаем web-server.
          changeState(STATE_NEW_DEVICE_SERVER);
          break;
      }

      if(btnFlash->checkActive( dt ) == STATUS_ACTIVE) {
        Serial.println("Flash button was pressed in 'Wifi connection' state.");
        changeState(STATE_NEW_DEVICE_SERVER);
      }
      break;
    }
    case STATE_NEW_DEVICE_SERVER: {
      led->update( dt );
      bool timeIsOut = webServer->updateAndGetTimeIsOut( dt );
      if(timeIsOut) {
        Serial.println("The web server is waiting too long for any action.");
        Serial.println("Restart now...\n");
        ESP.reset();
      }
      break;
    }
    case STATE_MAIN_WORK: {
      delay(10);
      led->update( dt );
      bool hasWiFiCon = wifiStation->wifiCheckConnected( dt );

      int8_t stat_sensPIR = sensorPIR->checkActive( dt );
      int8_t stat_btnBell = btnBell->checkActive( dt );
      int8_t stat_btnDoor = btnDoor->checkActive( dt );
   

      if(!iotActivationSendToDB || stat_sensPIR == STATUS_ACTIVE || stat_btnBell == STATUS_ACTIVE || stat_btnDoor != STATUS_WO_CHANGES) {
        FirebaseJson jsonToDB;

        if(!iotActivationSendToDB) {
          jsonToDB.set(SIGNAL_NAME_IOT_ACTIVATE, STATUS_ACTIVE);
          firebaseManager->sendNotification("Activation");
        }

        if(stat_sensPIR == STATUS_ACTIVE) {
          Serial.println(" * * * *  PIR SENSOR ACTIVATED  * * * *");
          jsonToDB.set(sensorPIR->name, STATUS_ACTIVE);
          buzzer->setMode(BUZZER_MODE_PIR_SENS);
          displayRelay->setMode(DISPLAY_RELAY_ON);

          firebaseManager->sendNotification("Motion");
        }

        if(stat_btnBell == STATUS_ACTIVE) {
          Serial.println(" * * * *  DOOR BELL ACTIVATED  * * * *");
          jsonToDB.set(btnBell->name, STATUS_ACTIVE);
          buzzer->setMode(BUZZER_MODE_BTN_BELL);
          displayRelay->setMode(DISPLAY_RELAY_ON);

          firebaseManager->sendNotification("Door bell");
        }

        if(stat_btnDoor != STATUS_WO_CHANGES) {
          Serial.println(" * * * *  DOOR IS OPEN  * * * *");
          jsonToDB.set(btnDoor->name, stat_btnDoor);
          buzzer->setMode(BUZZER_MODE_NONE);
          displayRelay->setMode(DISPLAY_RELAY_OFF);
        }

        // Отправляем данные
        if(hasWiFiCon && Firebase.ready()) {
          bool result = firebaseManager->sendDataToDB(jsonToDB);
          if(result)
            iotActivationSendToDB = true;
        }
      }

      if(btnMode->checkActive( dt ) == STATUS_ACTIVE) {
        deskMode->toNextMode();
      }

      if(btnFlash->checkActive( dt ) == STATUS_ACTIVE) {
        eepromStorage.clear();
        ESP.reset();
      }

      buzzer->update( dt, deskMode->buzzerMute);
      displayRelay->update( dt, deskMode->alwaysDisplay);
      firebaseManager->checkIfNeedSendPong();
      break;
    }
  }
}














