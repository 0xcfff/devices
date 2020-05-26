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

#define PIN_RADIO_CE          D3
#define PIN_RADIO_CSN         D8
#define PIN_LED_INDICATOR D1
#define PIN_RELAY_WATERPUMP D2
#define PIN_BUTTON_CONTROL D4

#define CONTROLBUTTON_LONGPRESS_DELAY             (3 * 1000)
#define OTAMODE_AUTODISABLE_DURATION_SEC          (1 * 60)
#define OTAMODE_INDICATORLED_BLINKINTERVAL_MSEC   200
#define WATERPUMP_MAXIMUM_ENABLEDDURATION_SEC     (30 * 60)

const uint64_t pipes[2] = { 0xAC0001DD01LL, 0x544d52687CLL };  
const uint64_t myPipe = 0xAC0001DD01LL; 
char * buffer = new char[255];

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);


// Components
Relay waterPumpRelay(PIN_RELAY_WATERPUMP, RELAYCFG_ENABLE_HIGH | RELAYCFG_START_OFF, WATERPUMP_MAXIMUM_ENABLEDDURATION_SEC);
WiFiAP wifiAp;
Button controlButton(PIN_BUTTON_CONTROL);
OtaUpdater ota;
Led indicatorLed(PIN_LED_INDICATOR, LEDCFG_ENABLE_HIGH | LEDCFG_START_OFF);

// Input processors
CommandsProcessor radioCommandsProcessor(radio, waterPumpRelay);
ControlButtonProcessor controlButtonProcessor(controlButton, ota, wifiAp, indicatorLed, OTAMODE_AUTODISABLE_DURATION_SEC, CONTROLBUTTON_LONGPRESS_DELAY, OTAMODE_INDICATORLED_BLINKINTERVAL_MSEC);


bool controlPressed = false;
bool controlChanged = false;
unsigned long  controlClickDurationMillis = 0;
unsigned long clickAt = 0;

ICACHE_RAM_ATTR void controlButtonChanged() {
  bool isPressed = digitalRead(PIN_BUTTON_CONTROL) == LOW;
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
// Implement Security on top of nrf24l01 (sign messages, keys exchange (preshared or something else), etc)


void setup() {
  // init serial
  Serial.begin(115200);
  Serial.println("Initializing...");

  radio.begin();

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

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, myPipe);

  radio.printDetails();
  //radio.startListening(); // - called in processor

  

  // init Commands Processor
  radioCommandsProcessor.begin();
  controlButtonProcessor.begin();

  radio.printDetails();

  // init Control Button
  //pinMode(PIN_BUTTON_CONTROL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_CONTROL, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_CONTROL), controlButtonChanged, CHANGE);
}

bool ledValue = false;
bool pumpEnabled = false;

unsigned long milliz = 0;
bool relayState = false;

void loop() {

  controlButtonProcessor.handle();
  radioCommandsProcessor.handle();
  waterPumpRelay.handle();

  Serial.printf("Relay state: %i\n", (int)waterPumpRelay.getState());
  // if (waterPumpRelay.getState()) {
  //   waterPumpRelay.flip();
  // }
  
  unsigned long curMillis = millis();
  if (curMillis - milliz > 5000 ) {
     Serial.write("Swap relay");
     waterPumpRelay.flip();
     milliz = curMillis;
  }

  Serial.println("tick...");

  delay(200);
}

#endif