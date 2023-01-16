# The 'Door alert' IoT controller

This 'Door alert' controller is part of smart home infrastructure.

[To manage the IoT devices of this smart home system, there is a special mobile cross-platform application](https://github.com/0kmMobi/iots_manager).

## Description

<img height="512" width="1024" src="/_readmi-res/elements.png">

The main features of the IoT controller:
1. Based on ESP8266/ESP32. It connects via Wifi to a router for Internet access;
2. To initialization Wifi network parameters (SSID and Passkey), the device switches to Wifi-AP mode and starts Web server. The user, through a mobile application, connects to the WiFi controller and transfers the Wifi network parameters to it to access the Internet;
3. The Ping-Pong scheme allows the user to know if the device is online.
4. When receiving a event from one of the sensors, the data is sent to the Firebase Realtime Database. 
5. Also it send message through Firebase Cloud Messaging with defined topic.
6. The device manages the local control point. In the event of an alarm, the control point display turns on along with an audible alert. There is a button for switching local notification modes at the local control point.




