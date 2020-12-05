#include <Arduino.h>
#include <SPI.h>
//#include <nRF24L01.h>
//#include <RF24.h>
//#include <rfchannel.h>
#include <macro-logs.h>
#include "stm32_def.h"


//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>


#define DISPLY_I2C_ADDRESS 0x27
#define COLUMS 20
#define ROWS   4

#define PIN_RF_CE   PB0
//#define PIN_RF_CSN  A4
#define PIN_RF_CSN  PA4

// LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
// const uint64_t myPipe = 0xabababab01LL; 
// const char * testMessage = "test message";

// uint8_t buff[32];


// RF24 radio(PIN_RF_CE, PIN_RF_CSN);
// RFChannel channel(&radio, 200, 10 * 1000);

void setup() {

    // init serial
    Serial.begin(115200);   

    LOG_INFOLN("Initializing...");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

uint8_t ledOn = false;

void loop() {

    ledOn = !ledOn;

    uint32_t idCode = DBGMCU->IDCODE;
    uint32_t cr = DBGMCU->CR;

    digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);

    delay(300);
}