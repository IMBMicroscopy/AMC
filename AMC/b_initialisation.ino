#include <avr/wdt.h>  //watchdog library
#include <avr/eeprom.h>  //read/write to internal EEPROM chip

String softVer = "RF_Relay_v1_2 18/2/16";  //Software version 
String serialNumber = "0012";

// define the different operating modes of the relay unit
#define startMode     0
#define coolMode      1 
#define zeroMode      2
#define minMode       3
#define maxMode       4
#define logOffMode    5
#define offMode       6
int state = startMode;  //set default operating mode at startup
char* modeNames[] = {"start", "cool", "zero", "min", "max", "logOff", "off"};
    
//Baud Rate for USB communications
long baudRate = 9600;  //USB port Baud Rate

//Declare LED and Relay PINS
const byte lampPin = 10;  // Mercury Lamp Relay Pin, Leonardo = 7
const byte powerPin = 11; // Powerboard Relay Pin, Leonardo = 8
const byte beepPin = 9; // Alarm on Logoff Pin (10 on leonardo, 3 on uno)
const byte lampLED = 14; // Mercury Lamp On indicator LED
const byte powerLED = 15; // Powerboard On indicator LED
const byte coolLED = 16; // Cooling Mode indicator LED, Leonardo = 9

//  Default Timer values (unit = seconds), modify for each device as required via terminal commands
long coolTime = 300;   // the time the hot lamp stays in cooling mode, preventing lamp start (powerboard may be turned on/off though) (cooling LED on)
long minTime = 1800;  // the mininum time the lamp may run for once it's turned on.
long maxTime = 3600;   // the maximum time that the power or lamp will run after minTime completes before the unit starts beeping to prompt user intervention 
long beepTime = 10;  // the time the beeper will beep for to warn the user to double click the icon on the desktop (1 second beeps with 1 second interval)
long offTime = 110;  // the time to allow a user to continue using the scope before going into cooling mode (cooling light flashes)
long resetTime = 1800;  //time to reset the device if it's not being used
int beepLength = 100;  //length of the beep in milliseconds
int flashLength = 100;  //length of the coolLED flash in milliseconds
int baseCode ;  //default baseCode for powerboard

//initialise flags
boolean stringComplete = false;  // flag to indicate if the USB read string is complete
boolean logOffFlag = false;  //flag to indicate if lampUSB was set to off when in minRunMode, so go straight to logOffMode when minTime is reached.
boolean beepFlag  = false;  //flags to toggle beeper on/off
boolean testBeepFlag = false;  //flags used to test beeper
boolean flashFlag = false;  //flags to toggle LEDs on/off
boolean coolPinFlag = false;  //flags used to monitor cooling state
boolean lampPinFlag = false;  //flags used to monitor lamp state
boolean powerPinFlag = false;  //flags used to monitor power state
boolean lampUSBFlag = false; //flag used to store USB command
boolean powerUSBFlag = false;  //flag used to store USB command
boolean newCommand = false;  
boolean newPowerCommand = false;  //indicates if a new command has been sent over USB in the current program loop
boolean newLampCommand = false;  //indicates if a new command has been sent over USB in the current program loop
boolean updateFlag = false;  //used to turn on/off auto serial print of time and flag values
boolean echoFlag = false;  //used for USB comms handshaking of commands
boolean programFlag = false;  //used to set default timer values if the unit hasnt been programmed before
boolean resetFlag = false;  //used to reset the unit if called during initialisation
boolean ramFlag = false;  //used to return the current free memory available

//initialise timers
long lastFlashTime = 0, lastBeepTime = 0;  //initialise the timers used to record the last beep/flashes
long startTime = 0, lastOnTime = 0, storeLastOnTime = 0;  // initialise startTime in unixtime format
long runTime = 0;  //initialise the current timer
long zeroTimer = 0;  //used to determine how long the unit is in zeroTimeMode
long relStartTime = 0; // initialise the relative time counter (used if the unit goes from for ex: minTime straight to beepMode)

String inputString = "";  // initialise string to store serial data in.
int wdtFlag = 1; // initialise flag for watchdog 

//timers and flag names and their corresponding values in an array for reading writing to eeprom
long eepromValues[] = {coolTime, minTime, maxTime, beepTime, offTime, resetTime, beepLength, flashLength, echoFlag, updateFlag, programFlag, baseCode};
String eepromNames[] = {"coolTime", "minTime", "maxTime", "beepTime", "offTime", "resetTime", "beepLength", "flashLength", "echo", "update", "program", "baseCode"};

int printIndex = 1;  //initialise index used for printing out timer values once per second
