//initialise the digital pins to startup settings
void initPins() {
 // transmitSignal(remote.allOff);
  pinMode(powerPin, OUTPUT); // initialize the powerPin as an output
  digitalWrite(powerPin, LOW);  //set pin LOW 
  pinMode(lampPin,OUTPUT); // initialize the lampPin as an output
  digitalWrite(lampPin, LOW);  //set pin LOW 
  pinMode(lampLED,OUTPUT); // initialize the lampLED pin as output
  digitalWrite(lampLED, LOW); //set pin LOW 
  pinMode(powerLED,OUTPUT); // initialize the powerLED pin as output
  digitalWrite(powerLED, LOW);  //set pin LOW 
  pinMode(coolLED,OUTPUT); // initialize the CoolLED pin as output
  digitalWrite(coolLED, LOW);  //set pin LOW 
  pinMode(beepPin,OUTPUT); // initialize the Beeper pin as output
  digitalWrite(beepPin, LOW);  //set pin LOW 
}

void initTimers() {
  coolTime = coolTime * 1000;  // the time the hot lamp stays in cooling mode, preventing lamp start (powerboard may be turned on/off though) (cooling LED on)
  minTime = minTime * 1000;  // the mininum time the lamp may run for once it's turned on.
  maxTime = (maxTime * 1000) + minTime;   // the maximum time that the power or lamp will run for before the unit starts beeping to prompt user intervention 
  beepTime = (beepTime * 1000) + maxTime;  // the time the beeper will beep for to warn the user to double click the icon on the desktop (1 second beeps with 1 second interval)
  offTime = (offTime * 1000) + beepTime;  // the time to allow a user to continue using the scope before going into cooling mode (cooling light flashes)
  resetTime = resetTime * 1000;  //time to reset the device if it's not being used
  relStartTime = startTime = millis();  //initialise the resettable current time
}

void mainStuff() {
  wdt_reset();  //reset the watchdog timer on each loop, otherwise it will reset the chip after the timeout period specified in setup
  if(resetFlag == true) {
    cpuReset();
  }  //reset the unit if a reset was called during initialisation
  readUSB();  //read USB port
  setFlags();  //toggle flags on/off after reading commands sent over USB port
  runTime = time() - relStartTime;  //calculate the relative runTime
  printTime();  //print out the relative runTime
  Ram();
}

unsigned long time() {
  if (millis() < startTime) {  //if the internal millisecond timer rolled over to zero, reset the timers
    cpuReset();
    delay(1000);
  } 
  return millis();
}

long onTime() {  //calculate time since the unit was powered up
  return time() - startTime;
}

void PinControl() {  //turn on/off the powerboard LED and relay as required
  //==========lamp section========================
   if(powerPinFlag == true) {  //Turn on Power Pin and LED
    digitalWrite(powerLED, HIGH);
    digitalWrite(powerPin, HIGH);
  }
  if(powerPinFlag == false) {  //Turn off Power Pin and LED
    digitalWrite(powerLED, LOW);
    digitalWrite(powerPin, LOW);
  }
  if(lampPinFlag == true) {  //Turn on lamp Pin and LED
    digitalWrite(lampLED, HIGH);
    digitalWrite(lampPin, HIGH);
  }
  if(lampPinFlag == false) {  //Turn off lamp Pin and LED
    digitalWrite(lampLED, LOW);
    digitalWrite(lampPin, LOW);
  }
  //========end of remote code
  
  if(flashFlag == false && testBeepFlag == false) {  //if the coolLED isnt flashing turn it on 
    if(coolPinFlag == true) {
      digitalWrite(coolLED, HIGH);
    } 
    else digitalWrite(coolLED, LOW);
  }
  
  if(beepFlag == true || testBeepFlag == true) {  //beep the piezo beeper
      if((onTime() - lastBeepTime) >= 1000) {
      digitalWrite(beepPin, HIGH);
      lastBeepTime = onTime();
    }
   if(onTime() >= (beepLength + lastBeepTime)) {
      digitalWrite(beepPin, LOW);
    }
  } else {
    digitalWrite(beepPin, LOW);
    lastBeepTime = 0;
    }
 
  if(coolPinFlag != true) {
  if(flashFlag == true || testBeepFlag == true) {  //Flash the coolLED
    if((onTime() - lastFlashTime) >= 1000) {
      digitalWrite(coolLED, HIGH);
      lastFlashTime = onTime();
    }
    if(onTime() >= (flashLength + lastFlashTime)) {
      digitalWrite(coolLED, LOW);
    }
  } else {
      digitalWrite(coolLED, LOW);
      lastFlashTime = 0;
    }
  }
}
void gotoZeroTimeMode() {
  //if power and lamp are both off goto zeroTimeMode
  if(powerPinFlag == false && lampPinFlag == false) {
    if(updateFlag) Serial.println(F("goto zeroMode"));
    zeroTimer = onTime();
    state = zeroMode;  //if the power is turned off and the lamp is off, go back to zeroTimeMode
  }
}

void gotoMinRunMode() {
  //if the power or lamp is turned on and the lamp was off goto minRunMode
  if((lampUSBFlag == true && lampPinFlag == false && newLampCommand == true) || (powerUSBFlag == true && lampPinFlag == false && newPowerCommand == true)) {  
    if(updateFlag) Serial.println(F("goto minMode"));
    relStartTime = time();  //  reset the runTime to zero
    state = minMode;  // goto minRunMode
  }
}

void gotoMaxRunMode() {
  //if the lamp was on and the unit receives a lampOn or powerOn command goto maxRunMode
  if((lampUSBFlag == true && lampPinFlag == true && newLampCommand == true)  || (powerUSBFlag == true && lampPinFlag == true && newPowerCommand == true)) { 
    if(updateFlag) Serial.println(F("goto maxMode"));
    relStartTime = time() - minTime;  // goto maxRunMode
    state = maxMode;  // goto maxRunMode
  }
}

void gotoLogOffMode() {
  //if the flags are set correctly goto logOffMode
  //if the user tries to turn off the lamp during minMode, wait til minMode is over then goto logOffMode
  if(lampUSBFlag == false && lampPinFlag == true && newLampCommand == true) logOffFlag = true;  
  if(runTime >= minTime) {
    if(logOffFlag == true) {
      if(updateFlag) Serial.println(F("goto logOffMode"));
      relStartTime = time() - maxTime;
      state = logOffMode; //goto beepingMode if lamp is turned off during minTime
    }
  }
}

void equaliseFlags() {  //set Pin Flags to equal USB flags
  powerPinFlag = powerUSBFlag;  //turn on/off power as requested from USB
  if(lampUSBFlag == true) lampPinFlag = true;  //turn on lamp (but not off) as requested from USB 
}

// watchdog timer reset the cpu if the software locks up
void cpuReset() {
  //transmitSignal(remote.allOff);
  Serial.println(F("System is about to Reset, restart your terminal"));
  delay(1000);  
  cli();               //disable interupts
  MCUSR &= ~(1<<WDRF);        //raise Watch Dog Reset Flag
  WDTCSR|=(1<<WDCE)|(1<<WDE);         // enable changes WDCE and then enable WDE
  WDTCSR = (1<<WDE) | (1<<WDP2) | (1<<WDP0);        // set prescaler to 64K cycles= 500 msec .
  sei();   //enable interupts
  delay(1000);
}

// turn off the watchdog timer reset 
void WDT_Off() {  
  cli();
  wdt_reset();
  /* Clear WDRF in MCUSR */
  MCUSR &= ~(1<<WDRF);
  /* Write logical one to WDCE and WDE */
  /* Keep old prescaler setting to prevent unintentional time-out */
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* Turn off WDT */
  WDTCSR = 0x00;
  sei();
}

void ifReset() {
  if(wdtFlag == 1) {  //if the unit was reset whilst in zeroTimeMode, go straight to zeroTimeMode
    if(updateFlag) Serial.println(F("unit was reset")); 
    wdtFlag = 255;
    eeprom_write_block((void*)&wdtFlag, (void*)60, sizeof(wdtFlag));
    delay(10);
    if(updateFlag) Serial.println("set WDT=" + String(wdtFlag));
    if(updateFlag) Serial.println(F("goto zeroMode"));
    zeroTimer = onTime();
    relStartTime = time();
    state = zeroMode;
  } 
  else {
    if(updateFlag) Serial.println(F("goto coolMode"));
    relStartTime = time() + coolTime;
    state = coolMode;  //else goto to coolingMode
  }
}

void maxResetTime() {
  if((onTime() - zeroTimer) >= resetTime) {  // reset the unit if it's not being used
    wdtFlag = 1;
    eeprom_write_block((void*)&wdtFlag, (void*)60, sizeof(wdtFlag));  //set EEPROM flag to indicate a watchdog reset was performed
    delay(10);
    if(updateFlag) Serial.println("set WDT=" + String(wdtFlag));
    delay(10);
    if(updateFlag) Serial.println(F("unit will now reset"));
    delay(10);
    cpuReset();  
    delay(1000);  //give the watchdog time to reset the unit
  }
}

int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void Ram() {
  if(freeRam() <= 100) {
    Serial.println(F("running out of RAM: unit will now reset"));
    delay(1000);
    cpuReset();
  }
}
