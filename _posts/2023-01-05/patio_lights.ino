// These examples are from the Electronics Cookbook by Simon Monk
//https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
// mod by Little_S@tan

#include <ELECHOUSE_CC1101_SRC_DRV.h>

#define CCGDO0 20 //GPIO20
#define CCGDO2 21 //GPIO21
#define REPEAT 5 // Number of times to repeat the signal
#define PULSE_LEN 268 // Pulse Length from TE: 268

// *** COLORS ***                                 REMOTE BUTTON LAYOUT (RxC)
#define WHITE_BTN 0x036487                        // W
#define RED_BTN 0x036400                          // R
#define ORANGE_BTN 0x03648A                       // 1x1
#define GOLD_BTN 0x036409                         // 2x1
#define YELLOW_BTN 0x036488                       // 3x1
#define CHARTREUSE_BTN 0x036499                   // 4x1
#define GREEN_BTN 0x036405                        // G
#define AQUAMARINE_BTN 0x03648E                   // 1x2
#define CYAN_BTN 0x03648D                         // 2x2
#define SKYBLUE_BTN 0x03640C                      // 3x2
#define CERULEAN_BTN 0x03641A                     // 4x2
#define BLUE_BTN 0x036493                         // B
#define INDIGO_BTN 0x036412                       // 1x3
#define VIOLET_BTN 0x036411                       // 2x3
#define MAGENTA_BTN 0x036490                      // 3x3
#define FUCHSIA_BTN 0x03641B                      // 4x3

// *** CONTROLS ***
#define OFF_BTN 0x036403
#define ON_BTN 0x03648B
#define FADE_BTN 0x036414
#define FLASH_BTN 0x036496
#define SMOOTH_BTN 0x03649C
#define STROBE_BTN 0x036495
#define BRIGHT_UP_BTN 0x036418
#define BRIGHT_DN_BTN 0x036484

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
  sendCode(GREEN_BTN, 24); // This forces a green starting color
  delay(500);
  sendCode(FADE_BTN, 24);  // Fades from off-color-wheel to Red to Green
  delay(8000);
}

// Generic sendBit - accepts 0 or 1 and sends the respective (1000, 1110) signal
void sendBit(int bit) {
  digitalWrite(CCGDO0, 1);
  delayMicroseconds( (1 + (2*bit)) * PULSE_LEN); // delay ==> 1+2*1=3 for High, 1+2*0=1 for Low
  digitalWrite(CCGDO0, 0);
  delayMicroseconds( (3 - (2*bit)) * PULSE_LEN); // delay ==> 3-2*1=1 for High, 3-2*0=3 for Low 
}

// Sends a hex code (max 32 bits) via MSB first (repeats REPEAT times)
void sendCode(int code, int numBits) {
  for(int j = 0; j < REPEAT; j++) {
    for(int i = numBits - 1; i >= 0; i--) { // Protocol sends most significant bits first, so we get to go backwards!
      int onOff = bitRead(code, i);
      sendBit(onOff);
    }
    sendBit(0); // stop transmitting (and sends one last 1000 signal while doing so)
    delayMicroseconds(40 * PULSE_LEN);
  }
}
