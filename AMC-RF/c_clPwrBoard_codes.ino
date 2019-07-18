/* clPwrBoard.h
 * This code is for Click 4 channel power board
 * Decoded by John Srnka 2015
 * First you need to include  clPwrBoard.h file
 * Then you create a new instance of the class with RF pin and pulse length :
 *  clPwrBoard newBoard = clPwrBoard(pin,pulselength);*/

clPwrBoard newBoard = clPwrBoard(10, 400);  //define power board witn pin 10, pulselength 400usec
/*two Macro functions for eeprom */
uint32_t  B_ADDR = 100;        // base address to store eeprom data
int lmpFlg, pwrFlg = 0;
uint32_t remBaseCode;
long remCodes[43] {};             //Array to store key values loaded from memory 42 (6 x number of base codes) values plus 0,  (Update this if you add more codes)

//uint32_t baseCode=3;  //initialise the remote base code

       
struct REMOTE
    {
      int lamp;// = 1;    // lamp = 1,7,13,19,25
      int allOff ;//= 6;     //  allOff = 6,12,18,24,30
      int acc[3]{};//{2,3,4};   //  acc = ((2,3,4)(8,9,10)(14,15,16)(20,21,22)(26,27,28))
    }remote;
    
#define update_data(address)    eeprom_update_block((long*)&remoteCodes, (uint32_t*)(address + 64), sizeof(remoteCodes));
#define read_data(address)        eeprom_read_block((long*)&remCodes, (uint16_t*)(address + 64), sizeof(remCodes));                  //loading key codes from eeprom into remCodes[30]

//************************************************************************************************
//#define UPDATE_EEPROM  //uncoment this to update eeprom data with new key values
//#define ERASE_EEPROM    //uncomment to erase eeprom data !!!
//*************************************************************************************************

    
//functions:
//void setBase();
//void updateBaseCodeEEPROM();
//void setupRemote();
//void transmitSignal(int);

//=================Functions body=======================

void setupRemote() {
  #ifdef UPDATE_EEPROM
  updateBaseCodeEEPROM();                                                                                                               // updating new data if needed
  #endif

  #ifdef ERASE_EEPROM
  for (int x = 0; x < 1000; x++) {
    eeprom_update_byte((uint8_t*)x, (uint8_t)0xFF);
    delay(10);
  }
  #endif
  read_data(B_ADDR);
  delay(1000);
}

void setBase() {
  remBaseCode  = eeprom_read_dword((uint32_t*)(B_ADDR+((baseCode-1)*5)));            
  remote.lamp = (uint32_t)((baseCode*6)-5);    // lamp = 1,7,13,19,25
  remote.allOff = (uint32_t)(baseCode*6);      //  allOff = 6,12,18,24,30
  remote.acc[0] = ((uint32_t)(baseCode*6)-4);
  remote.acc[1]=((uint32_t)(baseCode*6)-3);
  remote.acc[2]=((uint32_t)(baseCode*6)-2);   //  acc = ((2,3,4)(8,9,10)(14,15,16)(20,21,22)(26,27,28))
  //Serial.println(String(remote.lamp)+" "+String(remote.allOff)+" "+String(remote.acc[0])+" "+String(remote.acc[1])+" "+String(remote.acc[2]));
  //Serial.println(String(remBaseCode));
  delay(10);
}

void transmitSignal(int keyNumber) {
  char buff[40] {
  };   // storage for our complete remote signal
  int i = 0;
  sprintf(buff, "SF%s", (char*)(newBoard.dec2bin((unsigned long)remBaseCode, 24))); //,(char*) newBoard.dec2bin((unsigned long)remCodes[keyNumber], 13));
  strcat(buff,  newBoard.dec2bin(remCodes[keyNumber], 13));
  while ( i < 3) {                                                                   //repeat sending code 3x times
  newBoard.sendData((char*)buff);
  i++; 
  delay(10);
  }
  i=0;
  //Serial.println((char*)buff);
}


//====================EEPROM SECTION================

void updateBaseCodeEEPROM()
{
  if (eeprom_is_ready()) {
    //===================BASE CODES==========================
    eeprom_update_dword (( uint32_t *)B_ADDR  , 7224832 ); //store remote1 base code
    delay(10);
    eeprom_update_dword (( uint32_t *) (B_ADDR + 5), 1767936 ); //store remote2 base code
    delay(10);
    eeprom_update_dword (( uint32_t *) (B_ADDR + 10), 2852352); //store remote3 base code
    delay(10);
    eeprom_update_dword (( uint32_t *) (B_ADDR + 15), 16528896); //store remote4 base code
    delay(10);
    eeprom_update_dword (( uint32_t *) (B_ADDR + 20), 8451328); //store remote5 base code
    delay(10);
    eeprom_update_dword (( uint32_t *) (B_ADDR + 25), 11400448); //store remote6 base code
    delay(10);
    eeprom_update_dword (( uint32_t *) (B_ADDR + 30), 1981952); //store remote7 base code  
    delay(10);
    //=============KEY VALUES=================================
    long remoteCodes[] {
      0, //dummy
      4771, 2749, 6829, 1717, 5797, 3769, //Remote 1
      4608, 2591, 6671, 1559, 5639, 3611, //Remote 2
      4673, 2654, 6734, 1622, 5702, 3674, //Remote 3
      4656, 2592, 6719, 1583, 5687, 3623, //Remote 4
      4832, 2815, 6895, 1783, 5863, 3835, //Remote 5
      4810, 2770, 6850, 1756, 5836, 3796, //Remote6
      4803, 2781, 6861, 1749, 5829, 3801 //Remote7 
      };     
    update_data(B_ADDR);  //macro
    //==============5 remotes x 6 buttons 30 in total=========================
    //=========================================================
  }
  delay(200);
}
