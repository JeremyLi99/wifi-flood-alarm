//******************** DOCUMENTATION ********************//
/* CHANGELOG
   0.1 - Water Level Sensor (WLS) with 1 second delay and MCU is constantly awake + powered via USB
   0.2 - WLS now controlled via GPIO pin D2 with it turning on and off every 3 seconds.
   0.3 - Deep Sleep functionality implemented. There is no need for a 3 second delay of turning on and off the pins so that amount goes to the sleep duration instead. For real implementation, the sleep duration will last an hour.
   0.35 - Implemented counter variable for daily wellness check email stored in RTC memory. For real implementation, the counter will reset after 24 iterations.
   0.4 - Implemented WiFi + ThingSpeak connectivity. The testing sleep duration has been changed to 10 seconds.
   0.5 - Implemented email alarms for both flood detection and daily wellness checks. The resting sleep duration has been changed to 1 min and counter variable
         resets after 3 iterations. For real implementation, the sleep duration will be 1 hour and the counter will reset after 24 iterations.
   0.6 - Added "driver/adc", "esp_wifi", and "esp_bt" libraries for additional sleep mode functionality to control wireless connectivity & ADC. Set CPU freq to 80 MHz to save battery life. Fixed typo in HTML string.
   0.7 - Changed MCU from regular ESP32 to FireBeetle ESP32, thus WLS pins were changed accordingly in code. Turned off LED_BUILTIN to save battery life. Added min water level & insurance for if counter > 24 condition to connect to Wi-Fi + send email.
   RELEASE CANDIDATE - Removed redundant conditionals for functions. Now using numbers for real implementation.
*/


//******************** LIBRARIES ********************//
#include "WiFi.h" // included for WiFi functionality
#include "ThingSpeak.h" // included for ThingSpeak functionality 
#include "ESP32_MailClient.h" // included for email functionality

// included for sleep mode functionality
#include <Arduino.h>
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

//******************** MACROS ********************//

// WATER LEVEL SENSOR
#define WATER_LEVEL_VCC 25 // defines pin 25 (D2) as Water Level Sensor's VCC Pin
#define WATER_LEVEL_PIN 34 // defines pin 34 (A2) as Water Level Sensor's Signal Pin

// WIFI NETWORK
#define WIFI_NETWORK "My Network" // be sure to replace with your own information
#define WIFI_PASSWORD "password123"
#define WIFI_TIMEOUT_MS 5000

// THINGSPEAK
#define CHANNEL_ID 1234
#define CHANNEL_API_KEY "ABCD"

/* EMAIL
   To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
   YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
   Using a throwaway email would be a good idea
*/
#define EMAIL_SENDER_ACCOUNT "sender@gmail.com"
#define EMAIL_SENDER_PASSWORD "mypassword"

#define EMAIL_RECIPIENT  "receiver@gmail.com"

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_SERVER_PORT 465


//******************** VARIABLES & OBJECTS ********************//

// WATER LEVEL SENSOR
int waterLevel; // Analog Water Level Sensor value (0-4095 as GPIO outputs 3.3V)

// ESP32 DEEP SLEEP
RTC_DATA_ATTR unsigned int counter = 0; // counter to trigger daily wellness check email (0-23)

unsigned long DEEP_SLEEP_TIME = 1 * 60 * 60 * 1000 * 1000; //Deep Sleep timer counts in us; here, the timer is set to 1 hr [1 hr * 60 min/hr * 60 sec/min * 1000 sec/ms * 1000 us/ms]

// THINGSPEAK
WiFiClient client; // Object that handles HTTP Client Request

// EMAIL
SMTPData smtpData; // Object that contains configuration and data to send email

String floodMessageStart = "<p>The Water Level Sensor has determined there is a high chance that a flood has occurred.<br /> waterLevel (x &gt; 50): ";
String floodMessageEnd = "<br />Please check on the device and location to confirm.</p>";

String dailyWellnessCheck = "<p>This is a daily wellness check for the WiFi Flood Alarm. If you do not see this message tomorrow, then the device was either unable to connect to WiFi or the batteries have died. <br />Have a good day!</p>";


//******************** MAIN FUNCTIONS ********************//

void goToDeepSleep() {
  // Function to put ESP32 to sleep
  Serial.println ("Going to Deep Sleep...");

  WiFi.disconnect(true); // disconnects from Wi-Fi
  WiFi.mode(WIFI_OFF); // turns off Wi-Fi mode
  btStop(); // stops Bluetooth

  adc_power_off(); // turns off ADC
  esp_wifi_stop(); // stops Wi-Fi
  esp_bt_controller_disable(); // disables Bluetooth

  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME);
  esp_deep_sleep_start(); // ESP32 enters Deep Sleep Mode

  adc_power_on(); // turns on ADC after waking up
}

void connectToWiFi() {
  // Function to connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    // Attempting to connect to WiFi
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED) {
    // If ESP32 fails to connect to WiFi
    Serial.println(" Connection Failed!");

    goToDeepSleep(); //go back to Sleep
  }
  else {
    // ESP32 Connected to WiFi
    Serial.println(" Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
}

void connectToThingSpeak(int x, int y) { // x = Analog Water Level Sensor value, y = WiFi Signal Strength
  // Function to connect to ThingSpeak
  ThingSpeak.begin(client);
  Serial.println("Connecting to ThingSpeak...");

  ThingSpeak.setField(1, x); // ThingSpeak Channel Field 1: Analog Water Level Sensor Value
  Serial.print("Field 1 (Analog Water Level Sensor Value): ");
  Serial.println(x);

  ThingSpeak.setField(2, y); // ThingSpeak Channel Field 2: WiFi Signal Strength
  Serial.print("Field 2 (WiFi Signal Strength): ");
  Serial.println(y);

  ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY); // sends HTTP call to ThingSpeak
}

void sendCallback(SendStatus info); // Callback function to get Email sending status

void sendEmail(int x, int y) { // x = Analog Water Level Sensor value, y = counter
  // Function to send Email

  if (x >= 50) {
    // Sensor detects water
    Serial.println("Preparing to send Email...");

    smtpData.setLogin(SMTP_SERVER, SMTP_SERVER_PORT, EMAIL_SENDER_ACCOUNT, EMAIL_SENDER_PASSWORD); // Sets SMTP Server with Email host, port, account, and password
    smtpData.setSender("Wi-Fi Flood Alarm", EMAIL_SENDER_ACCOUNT); // Sets the sender name and email
    smtpData.setPriority("High"); // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
    smtpData.setSubject("[Wi-Fi Flood Alarm] Likely Flood Detected!");
    smtpData.setMessage(floodMessageStart + String(x) + floodMessageEnd, true); // first parameter is String message, second parameter is boolean where true is HTML and false is raw text
    smtpData.addRecipient(EMAIL_RECIPIENT); // sets the recipient's email

    if (!MailClient.sendMail(smtpData)) {
      Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
    }
    smtpData.empty(); // empties smptData object to free up memory
  }
  else {
    // Sensor does not detect water
    Serial.println("Sensor does not detect water. Email will not be sent.");
  }
  if (y >= 24) { // After 24 iterations
    // One day has passed (24 hours). Send Daily Wellness Check Email
    Serial.println("Also sending Daily Wellness Check Email...");

    smtpData.setLogin(SMTP_SERVER, SMTP_SERVER_PORT, EMAIL_SENDER_ACCOUNT, EMAIL_SENDER_PASSWORD); // Sets SMTP Server with Email host, port, account, and password
    smtpData.setSender("Wi-Fi Flood Alarm", EMAIL_SENDER_ACCOUNT); // Sets the sender name and email
    smtpData.setPriority("High"); // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
    smtpData.setSubject("[Wi-Fi Flood Alarm] Daily Wellness Check");
    smtpData.setMessage(dailyWellnessCheck, true); // first parameter is String message, second parameter is boolean where true is HTML and false is raw text
    smtpData.addRecipient(EMAIL_RECIPIENT); // sets the recipient's email

    if (!MailClient.sendMail(smtpData)) {
      Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
    }
    smtpData.empty(); // empties smptData object to free up memory
  }

}


//******************** DEFAULT FUNCTIONS ********************//

void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(80); // lowers clock frequency from 240 MHz to 80 MHz to save battery

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW to save battery

  pinMode(WATER_LEVEL_VCC, OUTPUT);
  delay(500); // small delay of 0.5 seconds before reading RTC memory to ensure bug where after a random number of wakeups, the ESP32 will never wake up again does not occur.
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print("Counter = ");
  Serial.print(++counter); // increments then prints; this is done first vs. at the end so counter does not get stuck if device is unable to connect to WiFi.
  Serial.println(" of 24 (for each hour in a day)"); // counter goes from 0-23. Total of 24 iterations to cover each hour in a day

  digitalWrite(WATER_LEVEL_VCC, HIGH); // turns on Water Level Sensor

  waterLevel = analogRead(WATER_LEVEL_PIN); // reads + stores analog pin value
  Serial.print("Analog Water Level Sensor Value: ");
  Serial.println(waterLevel);
  delay(50);

  digitalWrite(WATER_LEVEL_VCC, LOW); // turns off Water Level Sensor

  connectToWiFi(); // connects to WiFi; if no connection, device goes to sleep
  connectToThingSpeak(waterLevel, WiFi.RSSI()); // connects to ThingSpeak
  sendEmail(waterLevel, counter); // sends Email

  if (counter >= 24) { // After 24 iterations or 1 day
    Serial.println("Resetting counter...");
    counter = 0;
  }

  goToDeepSleep(); // enters Deep Sleep Mode
}
