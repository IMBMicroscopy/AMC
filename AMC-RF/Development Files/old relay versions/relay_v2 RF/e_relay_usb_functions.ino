// read data at the USB port and convert to a string if terminated by a carriage return
void readUSB() {
  if (Serial.available() > 0) {  // if there are bytes at the serial port
    char inChar = (char)Serial.read();   // get the new byte and convert to a character type
    inputString += inChar;  // add character to the inputString
    if (inChar == '\r') {    // if the incoming character is a carriage return, set a flag so the main loop can do something about it:
      inputString.trim();
      stringComplete = true;
    } 
  } else {
    stringComplete = false;
    newCommand = false;
  }
}

//do something if a recognised command is sent from the PC
void setFlags(){
  if (stringComplete){
    if(echoFlag == true && inputString != "ID?" && Serial) {
      Serial.println(inputString);
    }
    if(inputString == "ID?" && Serial) {
      Serial.println("Scope_Relay_Unit");
      newCommand = true;
    }

    else if(inputString == "powerOn") {
      powerUSBFlag = true;
      newCommand = true;
      newPowerCommand = true;
    }
    else if(inputString == "powerOff") {
      powerUSBFlag = false;
      newCommand = true;
      newPowerCommand = true;
    }
    else if(inputString == "lampOn") {
      lampUSBFlag = true;
      newCommand = true;
      newLampCommand = true;
    }
    else if(inputString == "lampOff") {
      lampUSBFlag = false;
      newCommand = true;
      newLampCommand = true;
    } 
    else if(inputString == "restart") {
      state = startMode;
      newCommand = true;
      powerUSBFlag = false;
      lampUSBFlag = false;
    }
    else if(inputString == "allOff") {
      powerUSBFlag = false;
      lampUSBFlag = false;
      newCommand = true;
      newLampCommand = true;
      newPowerCommand = true;
    }
    else if(inputString == "updateOn") {
      updateFlag = true;
      newCommand = true;
    }
    else if(inputString == "updateOff") {
      updateFlag = false;
      newCommand = true;
    }
    else if(inputString == "cpuReset") {
      cpuReset();
      newCommand = true;
    }
    else if(inputString == "getAll") {
      printEEPROM(); 
      newCommand = true;
    }
    else if(inputString == "beep") {
      testBeep();
      newCommand = true;
    }
    else if(inputString == "help") {
      printCommands();
      newCommand = true;
    }
    else if(inputString.startsWith("set ")) {
      writeEEPROM();
      newCommand = true;
    }
    else if(inputString == "getRam") {
      Serial.println("RAM = " + String(freeRam()));
      Serial.flush();
      newCommand = true;
      delay(1);
    }
    else if(inputString == "getTime") {
      String values1 = "t=" + String(runTime/1000) +"s, " + String(runTime) + "ms" + ", onT=" + String(onTime()) +"ms" + ", sT=" + String(startTime) + "ms" + ", rT=" + String(relStartTime) + "ms" +", mode=" + String(modeNames[state]);
      Serial.println(values1);
      values1 = "";
      Serial.flush();
      newCommand = true;
      delay(1);
    }
    else {
      newCommand = false;
      newPowerCommand = false;
      newLampCommand = false;
    }
    inputString = "";
    Serial.flush();
    stringComplete = false;
  }
}

void testBeep() {
    for(int i = 0; i <= 2; i++) {
      digitalWrite(beepPin, HIGH);
      delay(beepLength);
      digitalWrite(beepPin, LOW);
      delay(1000-beepLength);
    } 
}

//print out timer values and all Flags in 1s increments, if updateFlag == true.
void printTime() {
  if(updateFlag) {
    if((onTime()>(lastOnTime + 999)) && (onTime () <=(lastOnTime + 1002))  || newCommand == true){
      lastOnTime = onTime();
      String values1 = "T=" + String(runTime/1000) +"s, " + String(runTime) + "ms" + ", onT=" + String(onTime()) +"ms" + ", sT=" + String(startTime) + "ms" + ", rT=" + String(relStartTime) + "ms" ;
      String values2 = ", mode=" + String(modeNames[state]) + ", cool=" + String(coolPinFlag) + ", pUSB=" + String(powerUSBFlag) + ", newP=" + String(newPowerCommand) + ", pPin=" + String(powerPinFlag);
      String values3 = ", lUSB=" + String(lampUSBFlag) + ", newL=" + String(newLampCommand) + ", lPin=" + String(lampPinFlag) + ", beep=" + String(beepFlag) + ", flash=" + String(flashFlag);
      String values4 = ", LogOff=" + String(logOffFlag) + ", beepT=" + String(lastBeepTime) + ", FlashT=" + String(lastFlashTime) + ", zT=" + String(zeroTimer);
      String values5 = ", newUSB=" + String(newCommand) + ", Ram=" + String(freeRam());
      Serial.print(values1);
      values1 = "";
      Serial.print(values2);
      values2 = "";
      Serial.print(values3);
      values3 = "";
      Serial.print(values4);
      values4 = "";
      Serial.println(values5);
      values5 = "";
      Serial.flush();
      delay(1);
     }
  }
}

void printCommands() {
  Serial.println(F("Baud rate = 9600, all commands must end with a carriage return:"));
  Serial.println(F(""));
  Serial.println(F("ID? = returns 'USB_Relay_unit', used by the software to find the correct com port of the USB relay device"));
  Serial.println(F("powerOn and powerOff = cycles power to the accessories"));
  Serial.println(F("lampOn and lampOff = cycles power to the fluorescence lamp, however the lamp will only turn off once minTime is reached"));
  Serial.println(F("allOff = turn off the accessories instantly, wait until minTime is reached then enter beeping mode, then enter cooldown mode (~5 mins) then shutdown the lamp"));
  Serial.println(F("")); 
  
  Serial.println(F("updateOn and updateOff = turn on/off status mode to readout timer and flag values over the serial port"));
    Serial.println(F("  T:               runTime = current operating time, runtime ranges from -coolTime to logOffTime, runTime = 0 means the power and lamp are off."));
    Serial.println(F("  onT:             onTime = time since power cycling or resetting the device"));
    Serial.println(F("  sT:              startTime = time when the main loop of code began"));
    Serial.println(F("  rT:              relativeStartTime = reference time which is reset each time the unit goes back to offMode, used to calculate runTime"));
    Serial.println(F("  mode:            current operating mode of the device (startup, cooling, zeroTime, minRun, maxRun, logOff, off)"));
    Serial.println(F("  cool:            coolPin = is the cooling LED on/off"));
    Serial.println(F("  pUSB:            powerUSBFlag = USB command to turn on the power T/F"));
    Serial.println(F("  newP:            newPowerFlag = is the current power USB command new T/F"));
    Serial.println(F("  pPin:            powerPinFlag = is the power ON T/F"));
    Serial.println(F("  lUSB:            lampUSBFLag = USB command to turn on the lamp T/F"));
    Serial.println(F("  newL:            newLampFlag = is the current lamp USB command new T/F"));
    Serial.println(F("  lPin:            lampPinFlag = is the lamp ON T/F"));
    Serial.println(F("  beep:            beepFlag = is the piezo set to beep T/F"));
    Serial.println(F("  flash:           flashFlag = is the cooling LED set to flash T/F"));
    Serial.println(F("  LogOff:          logOffFlag = is the lamp set to go straight from minRunMode to logOffMode T/F"));
    Serial.println(F("  BeepT:           lastBeepTime = value of timer used to determine if a beep is required (value in msec)"));
    Serial.println(F("  FlashT:          lastFlashTime = value of timers used to determine if a flash is required (value in msec)"));
    Serial.println(F("  zT:              zeroTimer = time when unit went into zeroMode and used to determine is a reset is required (value in msec)"));
    Serial.println(F("  newUSB:          newCommand = has a new command been sent over USB T/F"));
    Serial.println(F("  Ram:             how much Ram is left"));
  
  Serial.println(F(""));
  Serial.println(F("getAll = get current maximum timer values and flags etc"));
    Serial.println(F("  the value range of each of these timers is 0 - 2,147,483 seconds or ~24 days.  However, (minTime + maxTime + beepTime + offTime must be in total less than 2147483 seconds)"));
    Serial.println(F("  coolTime:        the time the hot lamp stays in cooling mode, preventing lamp start (powerboard may be turned on/off though) (cooling LED on)"));
    Serial.println(F("  minTime:         the mininum time the lamp may run for once it's turned on."));
    Serial.println(F("  maxTime:         the maximum time that the power or lamp will run for before the unit starts beeping to prompt user intervention (maxTime + minTime = total possible runtime)"));
    Serial.println(F("  beepTime:        the time the beeper will beep for to warn the user to double click the icon on the desktop (beepLength beeps with 1 second interval)"));
    Serial.println(F("  offTime:         the time the cooling LED will flash for to allow a user to continue using the scope before going into cooling mode (flashLength flashes with 1 second interval)"));
    Serial.println(F("  resetTime:       time to reset the device if it's not being used, to try and prevent software/comms lockups"));
    Serial.println(F("  beepLength:      the time in milliseconds that the piezo beeps for each second ( used to control volume, try 20msec)"));
    Serial.println(F("  flashLength:     the time in milliseconds that the cooling LED flashes for each second (try 500msec)"));
    Serial.println(F("  echo:            echo commands sent over the USB port, 0=OFF, 1=ON"));
    Serial.println(F("  update:          send all timer values, operating mode and flag values over the USB port, 0=OFF, 1=ON"));
    Serial.println(F("  program:         indicates if the unit has ever been programmed, if the value is not equal to 1, then the default timer values {300, 900, 5400, 15, 300, 1800, 20, 500, 0, 0, 1} will be applied and the unit performs a cpuReset"));
  
  Serial.println(F(""));  
  Serial.println(F("set = set one of the 'getAll' values  eg: set coolTime=300 or set echo = 0(false) or 1(true), to apply changes you must use the cpuReset command"));
  
  Serial.println(F("")); 
  Serial.println(F("getTime = get current running timer values and operating mode"));
  Serial.println(F("getRam = get currently available free RAM value"));
    
  Serial.println(F("cpuReset = resets the hardware for a full restart and applies any timer changes etc"));
  Serial.println(F("restart = send unit into startup mode, but doesnt perform a hardware reset"));

  Serial.println(F("beep = test the beeper volume with 3 beeps"));

  Serial.println(F("help = get list of serial commands"));
  
  Serial.println(F(""));

  Serial.flush();
  delay(1);
}
  







