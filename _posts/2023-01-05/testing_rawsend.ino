// These examples are from the Electronics Cookbook by Simon Monk
// https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
// mod by Little_S@tan
// Usage & Configuration by Simon Dankelmann
// https://github.com/simondankelmann/Esp32-SubGhz/blob/main/Esp32/Esp32-SubGhz/Esp32-SubGhz.ino

#include <ELECHOUSE_CC1101_SRC_DRV.h>

#define CCGDO0 20 //GPIO20
#define CCGDO2 21 //GPIO21
int btn1x1code [512] = {60824, -66, 40307, -64, 19889, -100, 1757, -66, 1089, -66, 12747, -100, 11829, -68, 32647, -66, 7033, -17380, 67, -1260, 199, -132, 65, -198, 131, -100, 131, -132, 297, -134, 67, -630, 199, -66, 233, -132, 399, -134, 399, -130, 165, -396, 14485, -100, 4061, -66, 2219, -66, 23345, -66, 72679, -100, 33233, -66, 62597, -66, 44711, -66, 12647, -66, 11759, -98, 10387, -66, 3897, -68, 529, -98, 889, -66, 50761, -66, 229, -68, 17053, -64, 21881, -2366, 249, -808, 283, -804, 247, -828, 255, -824, 225, -840, 233, -830, 835, -240, 845, -240, 279, -800, 833, -276, 809, -276, 243, -804, 271, -796, 837, -276, 245, -802, 269, -798, 835, -276, 245, -808, 269, -796, 271, -802, 837, -276, 243, -806, 833, -240, 277, -804, 271, -10862, 267, -798, 269, -800, 271, -800, 271, -802, 269, -800, 271, -802, 835, -276, 813, -240, 279, -804, 833, -240, 845, -240, 279, -804, 271, -798, 833, -276, 245, -804, 269, -798, 835, -276, 245, -806, 269, -796, 271, -804, 835, -274, 245, -804, 835, -240, 279, -802, 269, -10846, 275, -806, 267, -800, 271, -804, 271, -800, 269, -800, 271, -802, 835, -276, 815, -240, 279, -800, 835, -240, 845, -274, 245, -804, 269, -798, 833, -276, 245, -802, 271, -800, 835, -276, 245, -804, 269, -798, 269, -806, 833, -276, 245, -804, 833, -276, 243, -806, 269, -10862, 269, -794, 271, -800, 269, -800, 271, -800, 271, -804, 269, -802, 835, -276, 813, -240, 279, -802, 833, -276, 811, -242, 279, -802, 269, -798, 835, -276, 245, -806, 269, -798, 837, -274, 245, -804, 271, -798, 269, -800, 835, -276, 245, -806, 831, -276, 245, -806, 269, -10856, 269, -798, 271, -800, 271, -802, 271, -802, 271, -798, 271, -804, 837, -276, 811, -274, 245, -802, 833, -276, 811, -276, 243, -802, 269, -798, 835, -276, 245, -808, 269, -796, 835, -276, 245, -806, 267, -798, 271, -806, 835, -276, 245, -802, 833, -276, 243, -806, 269, -10858, 267, -796, 269, -802, 269, -804, 271, -800, 271, -800, 271, -804, 833, -276, 811, -274, 245, -802, 833, -276, 811, -274, 243, -806, 269, -796, 835, -276, 243, -808, 269, -796, 835, -274, 247, -804, 269, -798, 269, -804, 837, -274, 245, -804, 833, -242, 277, -804, 267, -162736, 99, -1118, 363, -100, 195, -264, 229, -360, 97, -200, 755, -66, 6811, -100, 5675, -66, 6373, -98, 13705, -66, 14751, -68, 54237, -66, 32119, -66, 1531, -66, 5249, -68, 6397, -66, 44515, -100, 15541, -66, 10517, -68, 27293, -68, 20539, -66, 13969, -66, 657, -66, 3239, -98, 20847, -68, 14209, -66, 14423, -100, 42891, -100, 4619, -100, 24811, -66, 4341, -66, 10131, -66, 34427, -66, 13497, -2410, 231, -826, 269, -798, 269, -802, 269, -802, 271, -802, 271, -800, 835, -274, 811, -276, 243, -804, 837, -276, 777, -308, 243, -802, 271, -798, 837, -276, 243, -802, 269, -800, 833, -276, 245, -806, 269, -800, 271, -802, 833, -276, 243, -804, 837, -276, 243, -802, 271, -10832, 299, -796, 269, -802, 269, -800, 269, -802, 271, -804, 271, -800, 835, -276, 813, -240}

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
  sendSamples(btn1x1code, 512);
  delay(2000);
}

void sendSamples(int samples[], int samplesLength) {
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
    delayMicroseconds(totalDelay);
  }
  // STOP TRANSMITTING
  digitalWrite(CCGDO0,0);
}