#ifndef UNIT_TEST

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <xxtea-lib.h>

#include <JC_Button.h>

#include "led.h"
#include "relay.h"
#include "wifi_ap.h"
#include "ota_updater.h"
#include "commands_processor.h"
#include "button_processor.h"

#define PIN_RF_CE D2
#define PIN_RF_CSN D8
#define PIN_EXT_LED D1
#define PIN_RELAY D2
#define PIN_CONTROL D4

#define CONTROL_FLASH_DELAY (3 * 1000)
#define WIFI_AUTO_DISABLE (1 * 60)
#define FLASH_MODE_BLINK_INTERVAL 200


const uint64_t pipes[2] = { 0xAC0001DD01LL, 0x544d52687CLL };  
const uint64_t myPipe = 0xAC0001DD01LL; 
char * buffer = new char[255];

RF24 radio(PIN_RF_CE, PIN_RF_CSN);


// Components
Relay waterPumpRelay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
WiFiAP wifiAp;
Button controlButton(PIN_CONTROL);
OtaUpdater ota;
Led indicatorLed(PIN_EXT_LED, LEDCFG_ENABLE_HIGH | LEDCFG_START_OFF);

// Input processors
CommandsProcessor radioCommandsProcessor(radio, waterPumpRelay);
ControlButtonProcessor controlButtonProcessor(controlButton, ota, wifiAp, indicatorLed, WIFI_AUTO_DISABLE, CONTROL_FLASH_DELAY, FLASH_MODE_BLINK_INTERVAL);


bool controlPressed = false;
bool controlChanged = false;
unsigned long  controlClickDurationMillis = 0;
unsigned long clickAt = 0;

ICACHE_RAM_ATTR void controlButtonChanged() {
  bool isPressed = digitalRead(PIN_CONTROL) == LOW;
  if (isPressed != controlPressed) {
    controlPressed = isPressed;
    if (isPressed) {
      clickAt = millis();
      controlClickDurationMillis = 0;
    } else {
      controlClickDurationMillis = millis() - clickAt;
    }
    controlChanged = true;
  }
}


///////////// TODO ////////////////////
// Implement OTA based on ArduinoOTA sample
// When OTA will work, prolong WiFi channel being enabled by updating wifiLastUsedAt
// Implement Security on top of nrf24l01 (sign messages, keys exchange (preshared or something else), etc)


void setup() {
  // init serial
  Serial.begin(115200);
  Serial.println("Initializing...");

  radio.setChannel(100);
  radio.setPALevel(RF24_PA_HIGH);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
  radio.setDataRate(RF24_250KBPS);
  //radio.setAutoAck(1);                     // Ensure autoACK is enabled
  //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
  //radio.enableDynamicPayloads();
  //radio.enableDynamicAck();
  //radio.setPayloadSize(12);
  radio.enableDynamicPayloads();
  
  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

  //radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, myPipe);

  radio.printDetails();
  //radio.startListening(); // - called in processor

  // init Commands Processor
  radioCommandsProcessor.begin();
  controlButtonProcessor.begin();

  // init Control Button
  pinMode(PIN_CONTROL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_CONTROL), controlButtonChanged, CHANGE);
}

bool ledValue = false;
bool pumpEnabled = false;

void loop() {

  controlButtonProcessor.handle();
  radioCommandsProcessor.handle();
  waterPumpRelay.handle();

  Serial.printf("Relay state: %i\n", (int)waterPumpRelay.getState());
  if (waterPumpRelay.getState()) {
    waterPumpRelay.flip();
  }

  Serial.println("tick...");

  delay(200);
}

#endif