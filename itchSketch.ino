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

int atmospheric_pressure = 415; // should be around 508, 405 when tested, 415 in mouse, need to calibrate
int threshold = 3; // 10 too much, flickers unless hard press
int itchRate = 20000; //every X000 seconds -- could later make it reduce gradually
//int itchCount;
int blowDuration = 2000; //how long to keep the pump on
int ventDuration = 1000; //how long to vent (test with suck?)
int safetyVentTimer = 60000; //vent every min?
unsigned long startMillis;
unsigned long currentMillis;
unsigned long ventMillis;
unsigned long lastClick;
int inflateCount = 0;
bool ventIsOpen = false;
bool newAtmosphere = true;
bool isInflating = false;
//neopixel jewel
#define JEWEL_PIN 8
#define LED_COUNT 4

//mouse hack
//#define MOUSE_PIN 7

Adafruit_NeoPixel strip(LED_COUNT, JEWEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  initializePins();

  pixels.begin();
  strip.begin();
  pixels.show();
  strip.show();

//  pinMode(MOUSE_PIN, OUTPUT);

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
    blowAlil(7000);
    delay(100);
//    digitalWrite(MOUSE_PIN, HIGH);
    atmospheric_pressure = readPressure(1, 3);
    if (atmospheric_pressure < 405){
        atmospheric_pressure = 405;
    }
    Serial.println("atmosphere");
    Serial.println(atmospheric_pressure);
}

void loop() {
  int pressure = readPressure(1, 3);
  Serial.println(pressure);
  if (pressure < 400){
    pressure = 400;
  }

  int pressure_diff = pressure - atmospheric_pressure;
  /*
  digitalWrite(MOUSE_PIN, HIGH);
  if (pressure_diff > threshold) {
    setAllNeopixels(pixels.Color(pressure_diff/3, 0, pressure_diff/3));
//    vent(); //not working, gotta be a bad wire...
    if(!ventIsOpen){ //to keep from flickering
      setValve(3, OPEN);
      //green when click
      for (int i = 0; i < LED_COUNT; i++){
        strip.setPixelColor(i, 0, 255, 50); 
      }
      strip.show();
      ventIsOpen = true;
      ventMillis = millis();
//      lastClick = millis();
//      Mouse.begin();
//      Mouse.click();
//      Mouse.end();
      //hack to send click to other board
      digitalWrite(MOUSE_PIN, HIGH);
      delay(10);
      digitalWrite(MOUSE_PIN, LOW);
      
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
  */
  currentMillis = millis();
//  if (currentMillis - lastClick >= safetyVentTimer){ // if no click for a min
   if (inflateCount >= 8){ //
    if(!ventIsOpen){ //to keep from flickering
        setValve(3, OPEN);
        //red when resetting
        for (int i = 0; i < LED_COUNT; i++){
          strip.setPixelColor(i, 255, 0, 0); 
        }
        strip.show();
        ventIsOpen = true;
        ventMillis = millis();
        ventDuration = 15000;
        Serial.println("safety vent");
        inflateCount = 0;
    }
  } else if (pressure_diff > threshold && !isInflating) { //click
      if(!ventIsOpen){ //to keep from flickering
        setValve(3, OPEN);
        //green when click
        for (int i = 0; i < LED_COUNT; i++){
          strip.setPixelColor(i, 0, 255, 50); 
        }
        strip.show();
        ventIsOpen = true;
        ventMillis = millis();
        ventDuration = 1000;
        currentMillis = lastClick;
      }
  } else if (ventIsOpen && currentMillis - ventMillis >= ventDuration){ //finish vent
    setValve(3, CLOSE);
    ventIsOpen = false;
    //after, blue main
    for (int i = 0; i < LED_COUNT; i++){
        strip.setPixelColor(i, 0, 105, 255); 
     }
     strip.show();
  } else if (currentMillis - startMillis >= itchRate && !ventIsOpen){ // inflate ever 15 seconds
//    blowAlil(5000);
    //when inflating, yellow
    isInflating = true;
    for (int i = 0; i < LED_COUNT; i++){
        strip.setPixelColor(i, 255, 255, 0); 
     }
     strip.show();
    switchOnPumps();
    setValve(1, OPEN);
    startMillis = currentMillis;
    newAtmosphere = true;
    inflateCount++;
    Serial.println(inflateCount);
//    if (newAtmosphere){
//      atmospheric_pressure = readPressure(1, 3);
//      if (atmospheric_pressure < 405){
//        atmospheric_pressure = 405;
//      }
//      Serial.println("atmosphere");
//      Serial.println(atmospheric_pressure);
//      newAtmosphere = false;
//    }
  } else if (currentMillis - startMillis >= blowDuration){ //but only inflate for 5 seconds
    switchOffPumps();
    setValve(1, CLOSE);
    isInflating = false;
    
    delay(500);
    if (newAtmosphere){
      atmospheric_pressure = readPressure(1, 3);
      if (atmospheric_pressure < 405){
        atmospheric_pressure = 405;
      }
      Serial.println("atmosphere");
      Serial.println(atmospheric_pressure);
      newAtmosphere = false;
    }
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
