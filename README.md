# ESP32 FireBeetle Wi-Fi Flood/Leak Email Alarm
## General Overview
This project was done as a midterm assignment. The intent was to make a device suitable for remote purposes, meaning it has a long battery life, low power requirements, and a sleep mode. Additionally, it incorporates some data collection elements. Here, an [ESP32 FireBeetle](https://www.dfrobot.com/product-1590.html) was used alongside a [water level sensor](https://www.amazon.com/gp/product/B07THDH7Y4/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) to send information to [ThingSpeak](https://thingspeak.com/), an open-source data logger for IoT that has built-in MATLAB support and API functionality among other things, and then if the value is sufficiently high, an email alarm is sent, all done over Wi-Fi. However, this concept can be applied infinitely towards any number of other purposes, microcontrollers, and sensors.

The following non-standard Arduino libraries are used:
* [Mathwork's](https://github.com/mathworks) [ThingSpeak Arduino Library](https://github.com/mathworks/thingspeak-arduino)
* [mobitz's](https://github.com/mobizt) [ESP32 Mail Client Library](https://github.com/mobizt/ESP32-Mail-Client/blob/master/src/ESP32_MailClient.h)

In addition, these two videos were used to base the code off of: 
* https://randomnerdtutorials.com/esp32-email-alert-temperature-threshold/
* https://www.youtube.com/watch?v=F1fQ8m3S8-4

## Future Considerations
* Finding a more secure way of sending emails
* Developing a mobile application

## Diagrams
![Banana for Ref](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Banana%20for%20Ref.jpg)
* Banana for reference

![Circuit Diagram](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Circuit%20Diagram.png)
![Block Diagram](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Block%20Diagram.png)
### Current Consumption & Battery Life
![4.8V Power Supply](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/4.8V%20Power%20Supply.jpg)
* Using 5V consumes 10 uA

![Power Supply Circuit](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Power%20Supply%20Circuit.png)
![Battery Life](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Battery%20Life.PNG)

## Code Overview
### Libraries
![Slide 1](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%201.PNG)
"ThingSpeak.h" and "ESP32_MailClient.h" are available here:
* https://github.com/mathworks/thingspeak-arduino
* https://github.com/mobizt/ESP32-Mail-Client/blob/master/src/ESP32_MailClient.h

<esp_wifi.h> and <esp_bt.h> are inherent to ESP-based microcontrollers.
### Definitions
![Slide 2](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20BLock%202.PNG)
* This implementation uses a Google account and Gmail, however, it is possible to use other email hosts. The way it was done requires enabling "Less secure app access" so making a throwaway account is recommended. 
* You would replace the macros in "WIFI_NETWORK", "THINGSPEAK", and "EMAIL" with your own information.

![Slide 3](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%203.PNG)
* Change the email strings as necessary.

### Setup
![Slide 4](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%204.PNG)
### Loop and other Functions
![Slide 5](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%205.PNG)
* ConnectToWiFi(), ConnectToThingSpeak(int x, int y), and sendEmail(int x, int y) are all called in the loop function after turning off the sensor (and before checking if counter >= 23+1)

![Slide 6](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%206.PNG)
![Slide 7](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%207.PNG)
![Example ThingSpeak Fields](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Example%20ThingSpeak.PNG)

![Slide 8](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%208.PNG)
![Example Email 1](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Example%20Email%201.PNG)

![Slide 9](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%209.PNG)
![Example Email 2](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Example%20Email%202.PNG)

![Slide 10](https://github.com/JeremyLi99/wifi-flood-alarm/blob/main/slides/Code%20Block%2010.PNG)
