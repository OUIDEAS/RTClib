This is a fork of Adafruit's fork of JeeLab's fantastic real time clock library for Arduino which can be found [**here**](https://github.com/adafruit/RTClib).

This version of the library adds support for the RV1805 RTC chip. In addition to the standard API the Library exposes We add a countdown timer with interupts. The functions to control the timer are:

## New functions:

#### void setTimer(uint8_t time)
This function sets and starts the timer at a given number of seconds. The timer will continue to count down until it reaches 0. Then the CLK/INT pin will be pulled low.
#### uint8_t time getTimer()
Returns the current number of seconds left on the timer.
#### void startTimer()
starts the timer counting down after a stop. Note, this does not need to be called after setTimer.
#### void stopTimer()
Halts the timer at the current value, to restart the timer call startTimer.
#### void enableAlarm()
Enables Alarm interupts
#### void disableAlarm()
Disables Alarm interupts
#### void setAlarm(DateTime time)
sets the date and time that the alarm should trigger.
#### DateTime getAlarm()
returns the time that the alarm is currently set to

## How to handle timer Interupts

- read the interupt flags
- check that the 4th bit is 1 indicating that the countdown timer is the type of interupt
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

## How to handle alarm Interupts

- read the interupt flags
- check that the 3rd bit is 1 indicating that the Alarm is the type of interupt
- write 0 to 3rd interupt flag
- *optional*: set new alarm.

#### Here is an example of resetting the alarm after an interupt  
    void RTCISR() {
      uint8_t interuptFlags = read_i2c_register(RV1805_ADDRESS, 0x0F);
      Serial.println(interuptFlags, BIN);
      if ((interuptFlags >> 2) & 1 > 0) { // check for alarm interupt
        Serial.println("Alarm triggered, clearing flags");
        write_i2c_register(RV1805_ADDRESS, 0x0F, B11111010 & interuptFlags);

        // set alarm for 10 seconds from now
        DateTime alarm = rtc.now() + TimeSpan(0, 0, 0, 10);
        rtc.setAlarm(alarm);
	
	// clear flags
        interuptFlags = read_i2c_register(RV1805_ADDRESS, 0x0F);
      }
    }
