#include <Adafruit_NeoPixel.h>
#include <arduino.h>
#include "LedControl.h"

#define NUMPIXELS 24
#define PIN 2
#define DIN 12
#define CS  10
#define CLK 13
LedControl max7219 = LedControl(DIN, CLK, CS, 1);
uint32_t background = 0x00010101;
uint32_t secHand = 0x000000ff;
uint32_t minHand = 0x0000ff00;
uint32_t hourHand = 0x00ff0000;
uint32_t quaterMark = 0x000f0202;
uint32_t fiveMark = 0x00395C62;
uint32_t time = 45343;
uint16_t secct; //pocitadlo pro generator vterin
uint8_t last_millis;
uint16_t serialTimeout;
uint8_t serialState;
uint32_t serialTime;





Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void timeInSec(uint32_t time) {
    int sec,min,hour;
    sec = time % 60;
    time = time / 60;
    min = time % 60;
    time = time / 60;
    hour = time % 24 ;
    showDigitalTime(sec,min,hour);
    showHourTime(hour);
}
void timeInHour(uint32_t time) {
    int sec,min,hour;
    sec = time % 60;
    time = time / 60;
    min = time % 60;
    time = time / 60;
    hour = time % 24 ;
    showHourTime(hour);
}

void showtime(uint8_t sec,uint8_t min,uint8_t hour){
    for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, background);
  }
  pixels.setPixelColor(0, quaterMark);
  pixels.setPixelColor(15, quaterMark);
  pixels.setPixelColor(30, quaterMark);
  pixels.setPixelColor(45, quaterMark);
  pixels.setPixelColor(60, quaterMark);
  pixels.setPixelColor(66, quaterMark);
  pixels.setPixelColor(72, quaterMark);
  pixels.setPixelColor(78, quaterMark);
  pixels.setPixelColor(5, fiveMark);
  pixels.setPixelColor(10, fiveMark);
  pixels.setPixelColor(20, fiveMark);
  pixels.setPixelColor(25, fiveMark);
  pixels.setPixelColor(35, fiveMark);
  pixels.setPixelColor(40, fiveMark);
  pixels.setPixelColor(50, fiveMark);
  pixels.setPixelColor(55, fiveMark);
  pixels.setPixelColor(sec, secHand);
  pixels.setPixelColor(min, minHand);
  if (hour >= 12){
    hour -= 12;
  }
  pixels.setPixelColor((2 * hour), hourHand);
  pixels.show();
}
void showHourTime(uint8_t hour){
      for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, background);
  }
  pixels.setPixelColor(0, quaterMark);
  pixels.setPixelColor(6, quaterMark);
  pixels.setPixelColor(12, quaterMark);
  pixels.setPixelColor(18, quaterMark);
  pixels.setPixelColor(hour,hourHand);
}
void showDigitalTime(uint8_t sec,uint8_t min,uint8_t hour){
  char c;
  max7219.setDigit(0, 7, hour / 10, false);
  max7219.setDigit(0, 6, hour % 10, false);
  max7219.setDigit(0, 4, min / 10, false);
  max7219.setDigit(0, 3, min % 10, false);
  max7219.setDigit(0, 1, sec / 10, false);
  max7219.setDigit(0, 0, sec % 10, false);
  c = ' ';
  if ((sec & 1) == 0 ){
    c = '-'; 
  }
  max7219.setChar(0, 2, c, false);
  max7219.setChar(0, 5, c, false); 
}
void serialMachine(uint8_t addedMillis){
  int32_t c;
  c = Serial.read();
  if (c < 0){
    serialTimeout += addedMillis;
    if (serialTimeout >= 5000){
      serialState = 0;
      serialTime = 0;
    }
    return;
  }
  serialTimeout = 0;
  if ((c < '0')||(c > '9')){
    return;
  }
  c -= '0';
  switch(serialState++){
    case 0:
      serialTime = c * 10 * 3600;
      break;
    case 1:
      serialTime += c * 3600;
      break;
    case 2:
      serialTime += c * 10 * 60;
      break;
    case 3:
      serialTime += c * 60;
      break;
    case 4:
      serialTime += c * 10;
      break;
    case 5:
      serialTime += c;
      time = serialTime;
      serialState = 0;
      break;
    default:
      serialState = 0;
      break;
  }

}

void setup() {
  max7219.shutdown(0, false);
  // nastavení intenzity na přibližnou polovinu (0-15)
  max7219.setIntensity(0, 1);
  // vypnutí svitu všech znaků
  max7219.clearDisplay(0);
  pixels.begin();
  Serial.begin(9600);
  Serial.println("arduino hodiny pro chajdu :)");
  last_millis=(uint8_t)millis(); //inicializace casovani

}

void loop() {
  uint8_t ms;
  pixels.clear();
  timeInSec(time);
  pixels.show();
  ms = ((uint8_t)millis())-last_millis;
  if (ms != 0) {
    last_millis += ms;
    serialMachine(ms);
    secct += ms; // prihodime k pocitadlu vteriny
    if (secct >= 1000) {// pokud mame vterinu
      secct -= 1000; // odzitou vterinu zahodime a jedeme dal
      time += 1;
    }
  }
}
