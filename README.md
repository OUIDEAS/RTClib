This is a fork of Adafruit's fork of JeeLab's fantastic real time clock library for Arduino which can be found [**here**](https://github.com/adafruit/RTClib).

This version of the library adds support for the RV1805 RTC chip. In addition to the standard API the Library exposes We add a countdown timer with interupts. The functions to control the timer are:

## Added functions

#### void setTimer(uint8_t time);
This function sets and starts the timer at a given number of seconds. The timer will continue to count down until it reaches 0. Then the CLK/INT pin will be pulled low.
#### uint8_t time getTimer();
Returns the current number of seconds left on the timer.
#### void startTimer();
starts the timer counting down after a stop. Note, this does not need to be called after setTimer.
#### void stopTimer();
Halts the timer at the current value, to restart the timer call startTimer.

## How to handle timer Interupts

- read the interupt flags
- check that the 4th bit is 1 indicating that the countdown timer is the interupt
- stop the timer
- write 0 to interupt flag
- *optional*: write new time to timer.
- *optional*: start timer

#### Here is an example of resetting the timer after an interupt  

	void RTC_ISR(){
      uint8_t interuptFlags = read_i2c_register(RV1805_ADDRESS, 0x0F);
      Serial.println(interuptFlags, BIN);
      if ((interuptFlags >> 3) & 1 > 0) { // check for countdown timer interupt
          Serial.println("Timer triggered, resetting timer and clearing flags");
          rtc.stopTimer();
          write_i2c_register(RV1805_ADDRESS, 0x0F, B11110110 & interuptFlags);
          rtc.setTimer(10);
       }
     }
