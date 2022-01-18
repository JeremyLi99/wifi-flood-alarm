# ESP32 FireBeetle Wi-Fi Flood/Leak Email Alarm
## Overview
This project was done as a midterm assignment. The intent was to make a device suitable for remote purposes, meaning it has a long battery life, low power requirements, and a sleep mode. Additionally, it incorporates some data collection elements. Here, an [ESP32 FireBeetle](https://www.dfrobot.com/product-1590.html) was used alongside a [water level sensor](https://www.amazon.com/gp/product/B07THDH7Y4/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) to send information to [ThingSpeak](https://thingspeak.com/), an open-source data logger for IoT that has built-in MATLAB support and API functionality among other things, and then if the value is sufficiently high, an email alarm is sent, all done over Wi-Fi. However, this concept can be applied infinitely towards any number of other purposes, microcontrollers, and sensors.

The following non-standard Arduino libraries are used:
* [Mathwork's](https://github.com/mathworks) [ThingSpeak Arduino Library](https://github.com/mathworks/thingspeak-arduino)
* [mobitz's](https://github.com/mobizt) [ESP32 Mail Client Library](https://github.com/mobizt/ESP32-Mail-Client/blob/master/src/ESP32_MailClient.h)

In addition, these two videos were used to base the code off of: 
* https://randomnerdtutorials.com/esp32-email-alert-temperature-threshold/
* https://www.youtube.com/watch?v=F1fQ8m3S8-4

## Diagrams

## Code Explained
