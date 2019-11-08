// Programmable-Air
// Author: tinkrmind
// https://github.com/orgs/Programmable-Air
//
// sketch by August Luhrs for Itch Mouse w/ Arnab Chakravarty
//
// PCB v0.3/v0.4

#include "programmable_air.h"
#include <Mouse.h>

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, neopixelPin, NEO_GRB + NEO_KHZ800);

#define DEBUG 1

int atmospheric_pressure = 410; // should be around 508, 405 when tested, 415 in mouse, need to calibrate
int threshold = 10; // 10 too much, flickers unless hard press
int itchRate = 20000; //every X000 seconds -- could later make it reduce gradually
//int itchCount;
int blowDuration = 5000; //how long to keep the pump on
int ventDuration = 1000; //how long to vent (test with suck?)
unsigned long startMillis;
unsigned long currentMillis;
unsigned long ventMillis;
bool ventIsOpen = false;

//neopixel jewel
#define JEWEL_PIN 8
#define LED_COUNT 4

Adafruit_NeoPixel strip(LED_COUNT, JEWEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  initializePins();

  pixels.begin();
  strip.begin();
  pixels.show();
  strip.show();

  // Uncomment code below to read atmospheric_pressure instead of using default value
//   vent();
//   delay(2500);
//   atmospheric_pressure = readPressure(0, 10);
//   Serial.println(atmospheric_pressure);

   //for itch timer
   startMillis = millis();

   //mouse functionality for click
//   Mouse.begin(); //moved to allow cursor movement while not clicking
    for (int i = 0; i < LED_COUNT; i++){
            strip.setPixelColor(i, 200, 255, 150); 
    }
    strip.show();
  //beginning inflation for 15 seconds
    blowAlil(15000);
}

void loop() {
  int pressure = readPressure(1, 3);
  Serial.println(pressure);

  int pressure_diff = pressure - atmospheric_pressure;

  if (pressure_diff > threshold) {
    setAllNeopixels(pixels.Color(pressure_diff/3, 0, pressure_diff/3));
//    vent(); //not working, gotta be a bad wire...
    if(!ventIsOpen){ //to keep from flickering
      setValve(3, OPEN);
      for (int i = 0; i < LED_COUNT; i++){
        strip.setPixelColor(i, 0, 255, 150); 
      }
      strip.show();
      ventIsOpen = true;
      ventMillis = millis();
//      Mouse.begin();
//      Mouse.click();
//      Mouse.end();
      
    }
//    delay(500);
//    setValve(3, CLOSE);
  }
  else{
    if (pressure_diff < - threshold) {
      setAllNeopixels(pixels.Color(0, -pressure_diff/3, -pressure_diff/3));
    }
    else {
      setAllNeopixels(pixels.Color(0, 0, 0));
//      setValve(3, CLOSE);
//      ventIsOpen = false;
    }
  }

  currentMillis = millis();
  if (currentMillis - startMillis >= itchRate){
//    blowAlil(5000);
    switchOnPumps();
    setValve(1, OPEN);
    startMillis = currentMillis;
  } else if (currentMillis - startMillis >= blowDuration){
    switchOffPumps();
    setValve(1, CLOSE);
  }
  if (ventIsOpen && currentMillis - ventMillis >= ventDuration){
    setValve(3, CLOSE);
    ventIsOpen = false;
    for (int i = 0; i < LED_COUNT; i++){
        strip.setPixelColor(i, 255, 55, 0); 
     }
     strip.show();
  }

  delay(50);
//  blowAlil(100000);

//WHY DOES THE ORDER GO 312?????
//  setValve(1, OPEN);
//  delay(500);
//  setValve(2, OPEN);
//  delay(500);
//  setValve(3, OPEN);
//  delay(500);
//  setValve(1, CLOSE);
//  delay(500);
//  setValve(2, CLOSE);
//  delay(500);
//  setValve(3, CLOSE);
//  delay(500);
//  Serial.println(atmospheric_pressure);
}

void setAllNeopixels(uint32_t c){
    for (int i = 0; i < 3; i++) {
      pixels.setPixelColor(i, c);
    }
    pixels.show();
}

void blowAlil(int d){
  switchOnPumps();
  setValve(1, OPEN);
//  setValve(3, OPEN);
  delay(d);
  switchOffPumps();
  setValve(1, CLOSE);
//  setValve(3, CLOSE);
}
