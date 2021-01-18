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
      Serial.println(F("Scope_Relay_Unit"));
      newCommand = true;
    }
    else if(inputString == "getVer") {
      Serial.println("Software Version = " + softVer);
      newCommand = true;
    }
    else if(inputString == "getSer") {
      Serial.println("Serial Number = " + serialNumber);
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
      Serial.println(F("To apply changes you must use the 'set' command followed by the 'cpuReset' command"));
      Serial.println(F(""));
      printEEPROM(); 
      newCommand = true;
    }
    else if(inputString == "beepOn") {
      testBeepFlag = true;
      newCommand = true;
    }
    else if(inputString == "beepOff") {
      testBeepFlag = false;
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
      //String values1 = "t=" + String(runTime/1000) +"s, " + String(runTime) + "ms" + ", onT=" + String(onTime()) +"ms" + ", sT=" + String(startTime) + "ms" + ", rT=" + String(relStartTime) + "ms" +", mode=" + String(modeNames[state]) + ", RAM=" + String(freeRam());
      String values1 = "T=" + String(runTime/1000) +"s, " + String(runTime) + "ms, onT=" + String(onTime()) +", LoT=" + String(storeLastOnTime) + ", sT=" + String(startTime) + ", relST=" + String(relStartTime);
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
      delay(1);
      Serial.flush();
      newCommand = true;
      delay(1);
    }
    else if(inputString == "getPins") {
      //print pin values
      Serial.println("Board=" + boardType + ": " + "lampPin=" + String(lampPin) + ", " + "powerPin=" + String(powerPin) + ", " + "beepPin=" + String(beepPin) + ", " + "lampLED=" + String(lampLED) + ", " + "powerLED=" + String(powerLED) + ", " + "coolLED=" + String(coolLED));
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



//print out timer values and all Flags in 1s increments, if updateFlag == true.
void printTime() {
  if(updateFlag) {
    
               //if((onTime()>(lastOnTime + 999)) && (onTime () <=(lastOnTime + 1002)) || newCommand == true)  old code before RF code was used
   
      if(((onTime() - lastOnTime) <= ((printIndex*1000) + 10)) || newCommand == true)  {
        if(((onTime() - lastOnTime) >= (printIndex*1000)) || newCommand == true)  {
          long storeLastOnTime = lastOnTime;
          lastOnTime = onTime();
          String values1 = "T=" + String(runTime/1000) +"s, " + String(runTime) + "ms, onT=" + String(onTime()) +", LoT=" + String(storeLastOnTime) + ", sT=" + String(startTime) + ", relST=" + String(relStartTime);
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
          delay(1);
          printIndex = 1;
        }
    } else printIndex++;  
  }
}


void printCommands() {
  Serial.print(F("Baud = "));
  Serial.print(baudRate);
  Serial.println(F(", commands end with CR:"));
  Serial.println(F(""));
  Serial.println(F("ID? = returns 'USB_Relay_unit'"));
  Serial.println(F("powerOn and powerOff = cycles power to the microscope and accessories"));
  Serial.println(F("lampOn and lampOff = cycles power to the lamp, the lamp will only turn on outside of cooling mode and off once minTime is reached"));
  Serial.println(F("allOff = turn off the microscope and accessories instantly, wait until minTime is reached then enter beeping mode, then enter cooldown mode (~5 mins) then shutdown the lamp"));
  Serial.println(F("")); 
  
  Serial.println(F("getAll = get current timer values etc (Range of each timer is 0 - 2,147,483s or ~24 days. (minTime + maxTime + beepTime + offTime must be < than 2147483s)"));
  Serial.println(F(""));
  Serial.println(F("  coolTime:        Time in seconds the hot lamp is in cooling mode, preventing lamp start (accessories may be turned on/off) (cooling LED is on)"));
  Serial.println(F("  minTime:         Time in seconds the lamp may run for once it's turned on."));
  Serial.println(F("  maxTime:         Time in seconds that the accessories or lamp will run for before the unit starts beeping to prompt user intervention (maxTime + minTime = total possible runtime)"));
  Serial.println(F("  beepTime:        Time in seconds the piezo will beep to warn the user to click the icon on the desktop"));
  Serial.println(F("  offTime:         Time in seconds the cooling LED will flash to warn the user to click the icon on the desktop otherwise it will enter cooling mode"));
  Serial.println(F("  resetTime:       Time in seconds before the device will reset if it's not being used"));
  Serial.println(F("  beepLength:      Time in milliseconds that the piezo beeps for each second ( used to control volume, try 20msec)"));
  Serial.println(F("  flashLength:     Time in milliseconds that the cooling LED flashes for each second (try 500msec)"));
  Serial.println(F("  echo:            Echo USB commands, 0=OFF, 1=ON"));
  Serial.println(F("  update:          Update all timer, mode and flag values over the USB port, 0=OFF, 1=ON"));
  Serial.println(F("  program:         Has the unit ever been programmed, if the value is not equal to 1, then the default timer values {300, 900, 5400, 15, 300, 1800, 20, 500, 0, 0, 1} will be applied and the unit performs a cpuReset"));
  Serial.println(F("  baseCode:        baseCode used to communicate with matching powerboard, 1..255"));
  Serial.println(F(""));
  
  Serial.println(F("set = set one of the 'getAll' values  eg: set coolTime=300 or set echo = 0(false) or 1(true), to apply changes you must use the cpuReset command"));
  Serial.println(F("")); 
  Serial.println(F("getTime = get current running timer values and operating mode"));
  Serial.println(F("getRam = get currently available free RAM value"));
  Serial.println(F("getVer = get installed software version"));
  Serial.println(F("getSer = get Serial Number of hardware"));
  Serial.println(F("getPins = get list of pinouts on board"));
  Serial.println(F("cpuReset = resets the hardware for a full restart and applies any timer changes etc"));
  Serial.println(F("restart = send unit into startup mode, but doesnt perform a hardware reset"));
  Serial.println(F("beepOn and beepOff = test the beeper volume and Cool LED flash length"));
  Serial.println(F("help = get list of serial commands"));
  Serial.println(F(""));
  
  Serial.println(F("updateOn and updateOff = turn on/off status mode to readout timer and flag values over the serial port"));
  Serial.println(F("  T:               runTime = current operating time, runtime ranges from -coolTime to logOffTime, runTime = 0 means the power and lamp are off."));
  Serial.println(F("  onT:             onTime = time since power cycling or resetting the device"));
  Serial.println(F("  LoT:             lastOnTime = last time that the serial port updated the time values"));
  Serial.println(F("  sT:              startTime = time when the main loop of code began"));
  Serial.println(F("  relST:           relativeStartTime = reference time which is reset each time the unit goes back to offMode, used to calculate runTime"));
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
  
  Serial.flush();
  delay(1);
}
  
