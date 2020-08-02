#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <TimeLib.h>
#include <WidgetRTC.h>

#include <TM1638.h>

// change this:
static char auth[] = "blynkappauthkeything";
static char ssid[] = "yourWifi";
static char pass[] = "yourp4a55w0rd";
#define BLYNK_SERVER "your.blynk.server"
// end change

BlynkTimer timer;
WidgetRTC rtc;
TM1638 module(D5, D6, D7);

unsigned long last_poll = 0;
byte reading;

bool toggle_date_time = false;

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

typedef enum {
  BTN1 = 1,
  BTN2 = 2u,
  BTN3 = 4u,
  BTN4 = 8u,
  BTN5 = 16u,
  BTN6 = 32u,
  BTN7 = 64u,
  BTN8 = 128u  
} BTN;

void get_pressed(const byte &state, const BTN check, bool &pressed) {
    byte btn_state = (state & check);
    pressed = btn_state / check;
}

void displayClock() {

  size_t buf_size = 9;
  char buf[buf_size];
  if(toggle_date_time) {
    snprintf(buf, buf_size, "%02i %02i %02i", hour(), minute(), second());
  } else {
    snprintf(buf, buf_size, "%02i%02i%04i", day(), month(), year());
  }

  module.setDisplayToString(buf);
}

void setup() {
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass, BLYNK_SERVER, 8442);
  Serial.print("Hello from turais.de");
  setSyncInterval(10 * 60);
  timer.setInterval(1000L, displayClock);
}

void loop() {
  Blynk.run();
  timer.run();

  uint32_t now = millis();
  if((now - last_poll) > 50) {
    last_poll = now;
    byte act_reading = module.getButtons();
    if(act_reading != reading) {
      Serial.print("reading: ");
      Serial.println(act_reading, HEX);
      reading = act_reading;
      
      bool btn2_pressed;
      get_pressed(act_reading, BTN2, btn2_pressed);
      if(btn2_pressed) {
        Serial.println("BTN2 pressed.");
        toggle_date_time = !toggle_date_time;
        displayClock();
      }
    }
  }

}