void readEEPROM() {
  int j=0;
  for(int i=0; i<=12; i++) {
    eeprom_read_block((void*)&eepromValues[i], (void*)j, sizeof(eepromValues[i]));
    j=j+4;
    delay(10);  //wait for the eeprom to read
  }
  coolTime = constrain(eepromValues[0], 0, 3600); minTime = constrain(eepromValues[1], 0, 7200); maxTime = constrain(eepromValues[2], 0, 2147483);  //update all of the variables with values from the eeprom
  beepTime = constrain(eepromValues[3], 0, 1800); offTime = constrain(eepromValues[4], 0, 1800); 
  resetTime = constrain(eepromValues[5], 30, 2147483);
  beepLength = constrain(eepromValues[6], 0, 1000); flashLength = constrain(eepromValues[7], 0, 1000); echoFlag = constrain(eepromValues[8], false, true); 
  updateFlag = constrain(eepromValues[9], false, true); programFlag = constrain(eepromValues[10], false, true); baseCode = constrain(eepromValues[11],1,50); lampMins = constrain(eepromValues[12],0,2147483);
  eeprom_read_block((void*)&wdtFlag, (void*)60, sizeof(wdtFlag));
  delay(10);  //wait for the eeprom to read

// set default values in new arduino board if the program flag != 1  
 if(eepromValues[10] != 1) {  
   long eepromDefaults[] = {300, 900, 5400, 15, 300, 86400, 20, 500, 0, 0, 1, 1,0};
   for(int i=0; i<=12; i++) { 
     eepromValues[i] = eepromDefaults[i];
     eeprom_update_block((void*)&eepromValues[i], (void*)(i*4), sizeof(eepromValues[i]));  //write the value to the eeprom in the correct spot for that variable.
     delay(10);
   }
   resetFlag = true;
 } 
}

void printEEPROM() {
  for(int i=0; i<=12; i++) {
    long a = 0; long b = 0; String c ="";
    if(i == 0) {a = 0; b = 3600; c = "s";}
    if(i == 1) {a = 1; b = 7200; c = "s";}
    if(i == 2) {a = 1; b = 2147483; c = "s";}
    if(i == 3) {a = 0; b = 1800; c = "s";}
    if(i == 4) {a = 0; b = 1800; c = "s";}
    if(i == 5) {a = 30; b = 2147483; c = "s";}
    if(i == 6 || i == 7) {a = 0; b = 1000; c = "ms";}
    if(i == 8 || i == 9 || i == 10) {a = 0; b = 1; c = "";}
    if(i == 11) {a = 1; b = 255; c = "";}
    if(i == 12) {a = 0; b = 2147483; c = "min";}
    String allTimers = String(eepromNames[i]) + " = " + String(eepromValues[i]) + " (" + String(a) + " - " + String(b) + String(c) + ")";
    Serial.println(allTimers);
    allTimers = "";
    a = 0;  b = 0;  c ="";
  }
  Serial.println(F(""));
}

long writeEEPROM() {
  char strConvert[10];
  int index = inputString.indexOf("=")+1;  //get index of the "=" in incoming USB string
  int index2 = -1;
  long a = 0; long b = 0;
  String keyword = inputString.substring(4, (index-1));  // get the keyword from incoming string
  keyword.trim();  //trim any white spaces from the keyword
  for(int i=0; i<=12; i++) {
      if(keyword == eepromNames[i]) { //scan eepromNames array for the keyword, if it's in the array, then mark it's position in the array via index2
        index2 = i;
      }
  }
  if(index2 == 0) {a = 0; b = 3600;}
  if(index2 == 1) {a = 0; b = 7200;}
  if(index2 == 2) {a = 0; b = 2147483;}
  if(index2 == 3) {a = 0; b = 1800;}
  if(index2 == 4) {a = 0; b = 1800;}
  if(index2 == 5) {a = 30; b = 2147483;}
  if(index2 == 6 || index2 == 7) {a = 0; b = 1000;}
  if(index2 == 8 || index2 == 9 || index2 == 10) {a = 0; b = 1;}
  if(index2 == 11) {a = 1; b = 255;}
  if(index2 == 12) {a = 0; b = 2147483;}
  inputString.substring(index).toCharArray(strConvert,10);  // convert the number in the incoming USB string to a character array
  eepromValues[index2] = atol(strConvert);  //convert the character array to a LONG integer and put it into the correct variable in eepromValues
  eepromValues[index2] = constrain(eepromValues[index2], a, b);
  eeprom_update_block((void*)&eepromValues[index2], (void*)(index2*4), sizeof(eepromValues[index2]));  //write the value to the eeprom in the correct spot for that variable.
  Serial.println(String(keyword) + " = " + String(eepromValues[index2]));
  oldUpdateFlag = updateFlag; oldLampMins = lampMins;
  readEEPROM();
  initTimers();
  updateFlag = oldUpdateFlag;
  if(keyword != lampMins) {oldLampMins = lampMins;}  //if keyword is lampMins, then update the active lampMins value
  delay(10);  //wait for the eeprom to write
  Serial.println(F(""));
}
