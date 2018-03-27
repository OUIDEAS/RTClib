// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#define Serial SerialUSB
RTC_RV1805 rtc;

#define _I2C_WRITE write
#define _I2C_READ  read

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static void write_i2c_register(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire._I2C_WRITE((byte)reg);
  Wire._I2C_WRITE((byte)val);
  Wire.endTransmission();
}

static uint8_t read_i2c_register(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire._I2C_WRITE((byte)reg);
  Wire.endTransmission();

  Wire.requestFrom(addr, (byte)1);
  return Wire._I2C_READ();
}

void RTCISR() {
  Serial.println("----------------");
  uint8_t interuptFlags = read_i2c_register(RV1805_ADDRESS, 0x0F);
  Serial.print("interupt flags: ");
  Serial.println(interuptFlags, BIN);
  if ((interuptFlags >> 3) & 1 > 0) { // check for countdown timer interupt
    Serial.println("Timer triggered, resetting timer and clearing flags");
    rtc.stopTimer();
    write_i2c_register(RV1805_ADDRESS, 0x0F, B11110110 & interuptFlags);
    rtc.setTimer(15, 2);
    interuptFlags = read_i2c_register(RV1805_ADDRESS, 0x0F);
  }
  if ((interuptFlags >> 2) & 1 > 0) { // check for alarm interupt
    Serial.println("Alarm triggered, clearing flags");
    write_i2c_register(RV1805_ADDRESS, 0x0F, B11111010 & interuptFlags);

    //set alarm for 10 seconds from now
    DateTime alarm = rtc.now() + TimeSpan(0, 0, 0, 10);
    rtc.setAlarm(alarm);
    interuptFlags = read_i2c_register(RV1805_ADDRESS, 0x0F);
  }
  Serial.print("interupt flags: ");
  Serial.println(interuptFlags, BIN);
}

void setup () {
  while (!Serial) {
    delay(1);  // for Leonardo/Micro/Zero
  }

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  rtc.setTimer(3, 2);

  rtc.enableAlarm();
  DateTime alarm = rtc.now() + TimeSpan(0, 0, 0, 10);
  rtc.setAlarm(alarm);

  attachInterrupt(digitalPinToInterrupt(7), RTCISR, LOW);
}

void loop () {
  //  Serial.println(rtc.getTimer());
  delay(1000);
  //  if (rtc.getTimer() < 8) {
  //    rtc.stopTimer();
  //  }
}

