// These examples are from the Electronics Cookbook by Simon Monk
// https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
// mod by Little_S@tan
// Usage & Configuration by Simon Dankelmann
// https://github.com/simondankelmann/Esp32-SubGhz/blob/main/Esp32/Esp32-SubGhz/Esp32-SubGhz.ino

#include <ELECHOUSE_CC1101_SRC_DRV.h>

#define CCGDO0 20 //GPIO20
#define CCGDO2 21 //GPIO21
#define REPEAT 5 // Number of times to repeat the signal
#define PULSE_LEN 268 // Pulse Length from TE: 268
// New Sample Array
int btn1x1code [50] = {1, -3, 1, -3, 1, -3, 1, -3, 1, -3, 1, -3, 3, -1, 3, -1, 1, -3, 3, -1, 3, -1, 1, -3, 1, -3, 3, -1, 1, -3, 1, -3, 3, -1, 1, -3, 1, -3, 1, -3, 3, -1, 1, -3, 3, -1, 1, -3, 1, -40}

void initCC1101(){
  ELECHOUSE_cc1101.setSpiPin(18, 16, 19, 17); // (SCK, MISO, MOSI, CSN); 
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setGDO(CCGDO0, CCGDO2);
  ELECHOUSE_cc1101.setMHZ(433.92);        // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setPA(12);
  ELECHOUSE_cc1101.SetTx();               // set Transmit on
  ELECHOUSE_cc1101.setModulation(2);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  ELECHOUSE_cc1101.setDRate(512);         // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
  ELECHOUSE_cc1101.setPktFormat(3);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 
                                          // 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 
                                          // 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 
                                          // 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
  if(!ELECHOUSE_cc1101.getCC1101()){       // Check the CC1101 Spi connection.
    Serial.println("CC1101 Connection Error");
  }
}

void setup() {
    Serial.begin(9600);
    initCC1101();
}

void loop() {
  sendSamples(btn1x1code, 50); // Only 50 samples now instead of 512
  delay(2000);
}

void sendSamples(int samples[], int samplesLength) {
  for(int j=0; j < REPEAT; j++) { // Added the full-signal repeat loop
    int totalDelay = 0;
    unsigned long time;
    byte n = 0;

    for (int i=0; i < samplesLength; i++) {
      // TRANSMIT
      n = 1;
      
      totalDelay = samples[i]+0;
      if(totalDelay < 0){
        // DONT TRANSMIT
        totalDelay = totalDelay * -1;
        n = 0;
      }

      digitalWrite(CCGDO0,n);
      delayMicroseconds(totalDelay*PULSE_LEN); // Included the PULSE_LEN multiplier in the delay
    }
  }
  // STOP TRANSMITTING
  digitalWrite(CCGDO0,0);
}