#ifndef CONSTS_H
#define CONSTS_H

  ///
  #define DOOR_ALARM_CONTROLLER 11
  #define TEMPERATURE_SENSORS_CONTROLLER 12
  ///
  #define IOT_TYPE DOOR_ALARM_CONTROLLER



  // Pins
  #define PIN_BTN_DOOR D1             // Установим кнопку открытия двери
  #define PIN_PIR_SENSOR D2           // 4 - Pink
  #define PIN_BTN_FLASH D3            // to WebServer mode for get WiFi info
  #define PIN_LED_BUILTIN LED_BUILTIN // D4
  #define PIN_BTN_BELL D5             // 3 - Green /// Дверной звонок
  #define PIN_BTN_MODE D6             // 2 - Yellow
  #define PIN_BUZZER D7               // 1 - Orange
  #define PIN_RELAY_DISPLAY D8        // 

  #define SIGNAL_NAME_PIR_SENSOR    "PIR" // Motion detected by PIR-sensor
  #define SIGNAL_NAME_BTN_BELL      "Bell" // Door bell
  #define SIGNAL_NAME_BTN_DOOR      "Door" // Door is opened
  #define SIGNAL_NAME_IOT_ACTIVATE  "Act" // Activate @TODO: Отправляется сразу после включения и появления коннекта


  // Wifi AP and Web-server
  #define WIFI_AP_SSID_BASE "iot_"
  #define WIFI_AP_PASSWORD "12345678"

  #define WIFI_AP_LOCALIP "192.168.4.1"
  #define WIFI_AP_GATEWAY "192.168.4.1"
  #define WIFI_AP_NETMASK "255.255.255.0"


  #define WEB_SERVER_PORT 80
  #define WEBSERVER_WAITING_TIMER_MSEC_MAX (5*60*1000)

  // Firebase
  #define DB_DEVICES_IDS_TYPES "/devices_list/"
  #define DB_DEVICES_DATA "/devices_data/"
  #define DB_SENSOR_NAMES "sensor_names/"
  #define DB_SENSOR_VALUES "/values/"

  // Firebase Cloub messaging
  const String FCM_NOTIF_TITLE = "Alert";
  const String FCM_TOPIC_NAME_PREFIX = "doorAlert_";
  const unsigned long ANTI_SPAN_MAX_TIME = 10000; // 10 sec

#endif /* CONSTS_H */
