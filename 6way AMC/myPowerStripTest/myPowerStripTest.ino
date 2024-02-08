#ifdef __AVR__
#include <avr/power.h>
#endif
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
//***********define pixel strip pin**********

#define pixelPin A1
#define NumberOfLeds 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NumberOfLeds, pixelPin, NEO_GRB + NEO_KHZ800);

//******** define pins*****************

#define lampPin   9
#define powerPin1 8
#define powerPin2 7
#define powerPin3 6
#define powerPin4 5
#define powerPin5 4
//RTC connected to SDA, SCL
#define buzzer A5

//********define LED's*************

#define lampLed    0       //lamp LED pin
#define powerLed1  1      //Power board LED pin
#define powerLed2  2
#define powerLed3  3
#define powerLed4  4
#define powerLed5  5

uint32_t prog_start;
uint32_t lamp_start;
uint32_t lamp_now, lamp_stop;
uint32_t tm, s, m, h, d, sp, se;
String tim, st, startString, stopString;

//=================SETUP===================================
void setup() {
  //Serial.begin(9600);//start communicatio
  strip.begin();
  strip.setBrightness(50);
  randomSeed(analogRead(A4));
  pinMode(lampPin, OUTPUT);
  pinMode(powerPin1, OUTPUT);
  pinMode(powerPin2, OUTPUT);
  pinMode(powerPin3, OUTPUT);
  pinMode(powerPin4, OUTPUT);
  pinMode(powerPin5, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(powerPin1, !digitalRead(powerPin1));
  strip.setPixelColor(powerLed1, strip.Color(255, 0, 255));
  digitalWrite(powerPin2, !digitalRead(powerPin2));
  strip.setPixelColor(powerLed2, strip.Color(0, 0, 255));
  digitalWrite(powerPin3, !digitalRead(powerPin3));
  strip.setPixelColor(powerLed3, strip.Color(0, 255, 0));
  digitalWrite(powerPin4, !digitalRead(powerPin4));
  strip.setPixelColor(powerLed4, strip.Color(0, 255, 255));
  strip.show();
}
//=============================LOOP===========================
void loop() {
  strip.clear();    
  for (int y = 0; y < 6; y++) {
    strip.setPixelColor(y, 0);
  }
  strip.show();
  delay(1000);
  digitalWrite(lampPin,HIGH);
  strip.setPixelColor(lampLed, strip.Color(0, 0, 255));
  strip.show();
  delay(5000);//155000);
  digitalWrite(lampPin,LOW);
  strip.setPixelColor(lampLed, 0);
  strip.show();
  
  for (int z = 1; z < 20; z++) {
    strip.setPixelColor(powerLed5, strip.Color(random(255), z+10,0)); //digitalWrite(coolingLed, !digitalRead(coolingLed));
    strip.show();
    delay(500);
    // strip.setPixelColor(powerLed5,strip.Color(0, 0,0));  strip.show();
  }
  delay(1000);
  for (int y = 0; y < 6; y++) {
    strip.setPixelColor(y, strip.Color(255,255,255));
  }
  strip.show();
  delay(1250);

}

String  getTime(uint32_t tm) {   // function to decode time in milliseconds to correct day:hour:minute:second format and return string
  s = tm / 1000;
  m = s / 60;
  h = s / 3600;
  d = s / 86400;
  s = s - m * 60;
  m = m - h * 60;
  h = h - d * 24;
  String total = (String)h + ":" + (String)m + ":" + (String)s;

  return ((String)total);
}
//**************************************************************
//  digitalWrite(lampPin, !digitalRead(lampPin));
//  strip.setPixelColor(lampLed, strip.Color(255, 0, 0));
//  digitalWrite(powerPin1, !digitalRead(powerPin1));
//  strip.setPixelColor(powerLed1, strip.Color(255, 0, 255));
//  digitalWrite(powerPin2, !digitalRead(powerPin2));
//  strip.setPixelColor(powerLed2, strip.Color(0, 0, 255));
//  digitalWrite(powerPin3, !digitalRead(powerPin3));
//  strip.setPixelColor(powerLed3, strip.Color(0, 255, 0));
//  digitalWrite(powerPin4, !digitalRead(powerPin4));
//  strip.setPixelColor(powerLed4, strip.Color(0, 255, 255));
//  digitalWrite(powerPin5, !digitalRead(powerPin5));
//  strip.setPixelColor(powerLed5, strip.Color(255, 255, 0));
//  strip.show();

  //delay(3000);//115000);
 // y = millis() - x;
//  Serial.println(" Power time " + getTime(y)); //+String(y/1000));
//  for (int a = 0; a < 6; a++) {
//    strip.setPixelColor(a, random(256), random(256), random(256));
//  }
//  strip.show();
