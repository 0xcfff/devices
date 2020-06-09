#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <pcf8574_esp.h>
#include <tnlog.h>

#define U8G2_REF_MAN_PIC

#include <U8g2lib.h>
#include <JC_Button.h>

#include "dacha1_network.h"
#include "pump_messages.h"

#define PIN_RF_CE D2
#define PIN_RF_CSN D8

#define PIN_CONTROL_BUTTON D1 // TODO remove

#define PIN_PCF8574_BUTTON_MODE     1
#define PIN_PCF8574_BUTTON_OK       2
#define PIN_PCF8574_BUTTON_CANCEL   4

#define I2C_ADDRESS_DISPLAY 0x3C
#define I2C_ADDRESS_PCF8574 0x20

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
const uint64_t myPipe = 0xCCCCCCC1C0LL; 
//const char * testMessage = "test message";
uint8_t buffer[255];
int cyclesCount = 0;

RF24 radio(PIN_RF_CE, PIN_RF_CSN);
Button controlButton(PIN_CONTROL_BUTTON);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

PCF857x pcf8574(I2C_ADDRESS_PCF8574, &Wire);

void setup() {

    // init serial
    Serial.begin(115200);
    Serial.println("Initializing...");

    // init radio
    bool radioInit = radio.begin();
    Serial.print("Radio init ");
    Serial.print(radioInit ? "success" : "failure");
    Serial.println();
    
    radio.setChannel(100);
    radio.setPALevel(RF24_PA_HIGH);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
    radio.setDataRate(RF24_250KBPS);
    //radio.setAutoAck(1);                     // Ensure autoACK is enabled
    //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
    //radio.enableDynamicPayloads();
    radio.enableDynamicPayloads();

    //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

    radio.openWritingPipe(myPipe);
    radio.openReadingPipe(1, pipes[1]);

    radio.printDetails();

    controlButton.begin();

    Wire.begin();
    Wire.setClock(100000L);
    pcf8574.begin();
    pcf8574.write8(0);
    u8g2.begin();
    
    // blink
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
}

int counter = 0;

void detectDevices()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
}

void drawLogo(void)
{
    u8g2.setFontMode(1);	// Transparent
#ifdef MINI_LOGO

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(0, 22, "U");
    
    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb19_mn);
    u8g2.drawStr(14,8,"8");
    
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(36,22,"g");
    u8g2.drawStr(48,22,"\xb2");
    
    u8g2.drawHLine(2, 25, 34);
    u8g2.drawHLine(3, 26, 34);
    u8g2.drawVLine(32, 22, 12);
    u8g2.drawVLine(33, 23, 12);
#else

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(0, 30, "U");
    
    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb30_mn);
    u8g2.drawStr(21,8,"8");
        
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(51,30,"g");
    u8g2.drawStr(67,30,"\xb2");
    
    u8g2.drawHLine(2, 35, 47);
    u8g2.drawHLine(3, 36, 47);
    u8g2.drawVLine(45, 32, 12);
    u8g2.drawVLine(46, 33, 12);
    
#endif
}

void drawURL(void)
{
#ifndef MINI_LOGO
  u8g2.setFont(u8g2_font_4x6_tr);
  if ( u8g2.getDisplayHeight() < 59 )
  {
    u8g2.drawStr(89,20,"github.com");
    u8g2.drawStr(73,29,"/olikraus/u8g2");
  }
  else
  {
    u8g2.setFont(u8g2_font_4x6_t_cyrillic);
    //u8g2.drawStr(1,54,"github.com/olikraus/u8g2");
    u8g2.enableUTF8Print();
    u8g2.setCursor(1,54);
    u8g2.print("Привет!");
    //u8g2.drawStr(1,54,"Привет!");
  }
#endif
}

bool isHighLighted = false;

const char * main_list = "Living Room\nBedroom\nBedtime\nWatching TV\nGoing Out!";

void loop() {
//  bool written = radio.write(testMessage, 12);
// bool written = radio.write(testMessage, 12);

//  int len = sprintf(buffer, "Cycle: %i, millis since start %i msec", cyclesCount++, millis());

    //detectDevices();

    //u8g2.clearBuffer();

    uint8_t ios = pcf8574.read8();
    Serial.printf("PCF State: %i\n", (int)ios);
    if (ios > 0) {
        isHighLighted = !isHighLighted;
        digitalWrite(LED_BUILTIN, isHighLighted ? HIGH : LOW);

        

        if (ios & 1) {
            u8g2.userInterfaceSelectionList("Select Room", 1, main_list);
        }
        if (ios & 2) {
            u8g2.userInterfaceSelectionList("Select Room", 1, main_list);
        }
        if (ios & 4) {
            u8g2.userInterfaceSelectionList("Select Room", 1, main_list);
        }

    }

    

    // u8g2.clearBuffer();
    // drawLogo();
    // drawURL();
    u8g2.sendBuffer();


    controlButton.read();

    if (controlButton.wasPressed()){

//    if ((counter++ %2)==0) {
        RfRequest * pReq = (RfRequest*)buffer;
        pReq->header.flags = 0;
        pReq->header.command = PUMP_FLIP;
        pReq->body.pumpStartOrFlip.durationSec = 10 * 60;
        bool written = radio.write(buffer, sizeof(RfRequestHeader) + sizeof(PumpControlStartOrFlipBody));

        Serial.printf("Write %s\n", written ? "successful" : "failed");
        Serial.printf("Size of ulong %i\n", sizeof(unsigned long));
        Serial.println();
    }

    delay(200);
}