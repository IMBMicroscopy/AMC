class clPwrBoard {
    public:
      clPwrBoard(int pin,int pulse);
      void sendData(unsigned long Code, unsigned int length);
      void sendData(char* Code);  
      char* dec2bin(unsigned long dec, unsigned int length);
    private:
      int nPin;
     int pulseLength;
     void sendF();
     void send0();
     void send1();
     void sendSync();
     void setPin();
     void transmit(int nHighPulses, int nLowPulses);
     char* setBaseCode();
   };


clPwrBoard::clPwrBoard(int pin,int pulse ) {
  this->nPin =pin;// 10;
  this->pulseLength=pulse;//400; 
}

void clPwrBoard::setPin() {
  pinMode(this->nPin,OUTPUT);
}

void clPwrBoard::sendData(unsigned long Code, unsigned int length) {
  this->sendData( this->dec2bin(Code, length) );
}

void clPwrBoard::sendData(char* sCodeWord) {
  int i = 0;
  while (sCodeWord[i] != '\0')
  {
    switch (sCodeWord[i]) {
      case '0':
        send0();
        break;
      case '1':
        send1();
        break;
      case 'F':
        sendF();
        break;
      case 'S':
        sendSync();
    }
    i++;
  }  
}

void clPwrBoard::transmit(int nHighPulses, int nLowPulses) {
  if (nHighPulses != 0) {
    digitalWrite(this->nPin, HIGH);
    delayMicroseconds( this->pulseLength * nHighPulses);
  }
    digitalWrite(this->nPin, LOW);
    delayMicroseconds( this->pulseLength * nLowPulses);
}

/* Sends a "0" Bit
 *               _    
 * Waveform     | |___  1 short 2 long
 */

void clPwrBoard::send0() {
  this->transmit(1,2);
}

/* Sends a "1" Bit
 *            __  
 * Waveform  |  |_  2 short 1 long                 
 */

void clPwrBoard::send1() {
  this->transmit(2,1);
}

/*Sends a "F" Bit
 *          
 * Waveform: _______ 0 pulses 10 low pulses
 */

void clPwrBoard::sendF() {
  this->transmit(0,10);
}

/*Sends a "Sync" Bit
 *                       _   _  
 * Waveform Protocol 1: | |_| |_  1 short 1 long                    
 */

void clPwrBoard::sendSync() {
  this->transmit(6,1);
}

char* clPwrBoard::dec2bin(unsigned long Dec, unsigned int bitLength) {
  static char bin[64];
  unsigned int i=0;
  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }
  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  return bin;
}




