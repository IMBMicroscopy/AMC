/*
 23/8/21 - Added Lamp Minutes recording 
  The relay now records lamp minutes used, and saves them to eeprom when they hit 60min increments, at cpuReset or at the start of offMode

 11/11/21 - Added case statement in relay_initialisation to deal with various hardware pinouts on different AMCs
 
 */
