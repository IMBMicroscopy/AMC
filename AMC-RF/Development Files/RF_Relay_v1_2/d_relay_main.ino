void setup() {
  WDT_Off();  //turn off the watchdog so it doesnt infinitely reset the board
  initPins();  //setup the various digital pins for LED's and relays etc
  Serial.begin(baudRate);  //initialise serial port
  inputString.reserve(200);  // reserve 200 bytes for the inputString
  delay(3000);  // wait for the serial port to finish opening
  Serial.flush();
  delay(1000);
  readEEPROM();  //read any stored timer values in the EEPROM chip
  initTimers(); //set current timer values
  relStartTime = startTime = millis();  //initialise the resettable current time
  wdt_enable(WDTO_8S);  //set watchdog reset timeout period to 2secs
  //updateBaseCodeEEPROM(); not required
  setupRemote();
  setBase();
  transmitSignal(remote.allOff);
  delay(500);
}

void loop() {
  mainStuff();
  switch(state) {
    
    case startMode:  // Enter start Mode (if the system has been restarted turn everything off and go into cooling mode)
      if(updateFlag) Serial.println(F("startMode"));
      powerPinFlag = false; powerUSBFlag = false; lampPinFlag = false; lampUSBFlag = false; coolPinFlag = false; beepFlag = false; flashFlag = false;
      ifReset();
      runTime = time() - relStartTime;
      Ram();
      break;
      
    case coolMode:  // Enter coolingMode (time = -ve) (the lamp will only turn on after coolingMode completes even if requested during coolingMode), 
    //(toggling power on/off doesnt affect the value of runtime)
      lampPinFlag = false; coolPinFlag = true; beepFlag = false; flashFlag = false;
      powerPinFlag = powerUSBFlag;  //toggle power on/off
      if(runTime >= -5) {  //if runTime ~0 then goto next mode
        coolPinFlag = false;  //turn off the cooling LED
        zeroTimer = onTime();  //start zeroTimer
        state = zeroMode;  //move to zeroTimeMode
        if(updateFlag) Serial.println(F("goto zeroMode"));
      }
      Ram();
      break;
      
    case zeroMode:  // Enter zeroTimeMode (stay at runTime = 0, until something is turned on)
      beepFlag = false; flashFlag = false; coolPinFlag = false; beepFlag = false; flashFlag = false;
      maxResetTime();
      relStartTime = time();  //keep resetting the runTimer to zero, until a relay is turned on
      if(powerPinFlag == true || lampPinFlag == true) {
        if(updateFlag) Serial.println(F("goto minMode"));
        zeroTimer = 0;
        state = minMode;  //if either relay is turned on, goto minRunMode
      }
      equaliseFlags();
      Ram();
      break;
      
    case minMode:  // Enter minRunMode (allow powerboard toggling (goto minRunMode if the lamp is off) allow lamp to turn on but not off, 
    //if you try to turn the lamp off, it will stay on until minTime, then go to logOffMode)
      beepFlag = false; flashFlag = false;
      gotoZeroTimeMode();
      gotoMinRunMode();
      gotoLogOffMode();
      if(runTime >= minTime && logOffFlag == false) {
        if(updateFlag) Serial.println(F("goto maxMode"));
        relStartTime = time() - minTime;
        state = maxMode;  //goto maxRunMode if minTime is exceeded
      }
      equaliseFlags();
      Ram();
      break; 
      
    case maxMode:  // Enter maxRunMode (allow powerboard toggling (goto minRunMode if the lamp is off, else goto maxRunMode),
   // allow lamp to turn on but not off(turning either power or lamp on when the lamp is already on, goes to maxRunMode), if the lamp needs to be shutdown then go to logOffMode)  
      beepFlag = false; flashFlag = false;
      gotoZeroTimeMode();
      gotoMinRunMode();
      gotoMaxRunMode();
      gotoLogOffMode();
      if(runTime >= maxTime) {
        if(updateFlag) Serial.println(F("goto logOffMode"));
        relStartTime = time() - maxTime;
        state = logOffMode; //goto beepingMode if maxTime is exceeded
      }
      equaliseFlags();
      Ram();
      break;
      
    case logOffMode:  // Enter logOffMode (allow powerboard toggling (goto minRunMode if the lamp is off, else goto maxRunMode), 
    //allow lamp to turn on but not off(turning either power or lamp on when the lamp is already on, goes to maxRunMode))  
      if(runTime <= beepTime) {beepFlag = true; flashFlag = true;} 
      else {beepFlag = false; flashFlag = true;}
      logOffFlag = false;
      gotoZeroTimeMode();
      gotoMinRunMode();
      gotoMaxRunMode();
      if(runTime >= offTime) { 
        if(updateFlag) Serial.println(F("goto offMode"));
        relStartTime = time() - offTime;        
        state = offMode; // goto OffMode
      }
      equaliseFlags();
      Ram();
      break;
      
    case offMode:  //Enter offMode (turn off the powerboard, if the lamp is on, turn it off then go into cooling mode, else go to zeroTimeMode)
      powerPinFlag = false; powerUSBFlag = false; lampUSBFlag = false; beepFlag = false; flashFlag = false;
      if(lampPinFlag == true) { 
        lampPinFlag = false;
        updateLampMins();  //update lampMins to eeprom when lamp turns off
        if(updateFlag == true) Serial.println(F("goto startMode")); 
        state = startMode;
      } else {
          if(updateFlag) Serial.println(F("goto zeroMode"));
          zeroTimer = onTime();
          state = zeroMode;
        }
        Ram();
      break; 
  }
  PinControl();  //Toggle Pins for LEDs and Relay as flagged above
  newCommand = false;   //reset new command counter to indicate if a new command is sent on the next loop
  newLampCommand = false;  //reset new command counter to indicate if a new command is sent on the next loop
  newPowerCommand = false;  //reset new command counter to indicate if a new command is sent on the next loop
}
