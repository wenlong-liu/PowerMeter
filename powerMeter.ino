#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "SdFat.h"

SdFat SD;
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_INA219 ina219;
RTC_DS3231 RTC;

unsigned long interval = 1000;  // Reading interval is 1 second.
const int chipSelect = 10;
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power = 0;
float energy = 0;
DateTime current_time;
File Results;
uint32_t timestamp ;

void setup() {
  SD.begin(chipSelect);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ina219.begin();
  RTC.begin();
}

void loop() {
  current_time = RTC.now();
  timestamp = current_time.unixtime();
  ina219values();
  displaydata(); // Display voltage, current, power and energy consumption.
  writeToCSV();
  delay(interval);
  }
  
void writeToCSV(){
    Results = SD.open("results.csv", FILE_WRITE);
    if (Results) {
      Results.print(timestamp);
      Results.print(",");
      Results.print(loadvoltage);
      Results.print(",");
      Results.print(current_mA);
      Results.print(",");
      Results.print(power);
      Results.print(",");
      Results.print(energy);
      Results.print(",");
      Results.print("\n");
      Results.close();
    }
}

void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(loadvoltage);
  display.setCursor(35, 0);
  display.println("V");
  display.setCursor(50, 0);
  display.println(current_mA);
  display.setCursor(95, 0);
  display.println("mA");
  display.setCursor(0, 10);
  display.println(power);
  display.setCursor(35, 10);
  display.println("mW");
  display.setCursor(50, 10);
  display.println(energy);
  display.setCursor(95, 10);
  display.println("mWh");
  display.setCursor(0,20);
  display.println("ts:");
  display.setCursor(50, 20);
  display.println(timestamp);
  display.display();
}

void ina219values() {
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  power = loadvoltage * current_mA;
  energy = energy + power / 3600;
}
