#ifndef UNIT_TEST

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <LittleFS.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <xxtea-lib.h>
#include <macro-logs.h>

#include <JC_Button.h>

#include "led.h"
#include "relay.h"
#include "wifi-ap.h"
#include "ota-updater.h"
#include "commands_processor.h"
#include "button_processor.h"
#include "web_processor.h"
#include "dacha1_network.h"

#define PIN_RADIO_CE            D3
#define PIN_RADIO_CSN           D8
#define PIN_LED_INDICATOR       D1
#define PIN_RELAY_WATERPUMP     D2
#define PIN_BUTTON_CONTROL      D4

#define CONTROLBUTTON_LONGPRESS_DELAY             (3 * 1000)
#define OTAMODE_AUTODISABLE_DURATION_SEC          (1 * 60)
#define OTAMODE_INDICATORLED_BLINKINTERVAL_MSEC   200
#define WATERPUMP_MAXIMUM_ENABLEDDURATION_SEC     (30 * 60)

#define WATERPUMP_WIFI_SSID     "esp8266-wpump"
#define WATERPUMP_WIFI_PSWD     "11111111"

#define DNS_PORT                53

#define RADIO_CHANNEL           100
#define RADIO_DATARATE          RF24_250KBPS
#define RADIO_PALEVEL           RF24_PA_MAX

// Components
ESP8266WebServer server(80);
DNSServer dnsServer;

WiFiAP wifiAp(WATERPUMP_WIFI_SSID, WATERPUMP_WIFI_PSWD);
OtaUpdater ota;
RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);
Button controlButton(PIN_BUTTON_CONTROL);
Led indicatorLed(PIN_LED_INDICATOR, LEDCFG_ENABLE_HIGH | LEDCFG_START_OFF);
Relay waterPumpRelay(PIN_RELAY_WATERPUMP, RELAYCFG_ENABLE_HIGH | RELAYCFG_START_OFF, WATERPUMP_MAXIMUM_ENABLEDDURATION_SEC);

// Input processors
CommandsProcessor radioCommandsProcessor(radio, waterPumpRelay);
ControlButtonProcessor controlButtonProcessor(controlButton, ota, wifiAp, indicatorLed, OTAMODE_AUTODISABLE_DURATION_SEC, CONTROLBUTTON_LONGPRESS_DELAY, OTAMODE_INDICATORLED_BLINKINTERVAL_MSEC);
WebProcessor webProcessor(&server, &radio, &waterPumpRelay, &controlButtonProcessor);

///////////// TODO ////////////////////
// TODO: Implement Security on top of nrf24l01 (sign messages, keys exchange (preshared or something else), etc)


void setup() {
    // init serial
    Serial.begin(115200);
    Serial.println("Initializing...");

    // Init FS
    LittleFS.begin();

    // init radio
    radio.begin();

    radio.setChannel(RADIO_CHANNEL);
    radio.setPALevel(RADIO_PALEVEL);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
    radio.setDataRate(RADIO_DATARATE);
    //radio.setAutoAck(1);                     // Ensure autoACK is enabled
    //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
    //radio.enableDynamicPayloads();
    //radio.enableDynamicAck();
    //radio.setPayloadSize(12);
    radio.enableDynamicPayloads();
    
    //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

    //radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, RF_L3NET_P2P_ADDR(RF_NETWORK_DACHA1, RF_DEVICEID_CONTROLLER, RF_DEVICEID_WATERPUMP));
    radio.openReadingPipe(2, RF_L3NET_P2P_ADDR(RF_NETWORK_DACHA1, RF_DEVICEID_WATERPUMP, RF_DEVICEID_WATERPUMP));
    radio.openReadingPipe(3, RF_L3NET_P2P_ADDR(RF_NETWORK_DACHA1, RF_DEVICEID_BANYA, RF_DEVICEID_WATERPUMP));
    radio.openReadingPipe(4, RF_L3NET_P2P_ADDR(RF_NETWORK_DACHA1, RF_DEVICEID_UNKNOWN4, RF_DEVICEID_WATERPUMP));
    radio.openReadingPipe(5, RF_L3NET_P2P_ADDR(RF_NETWORK_DACHA1, RF_DEVICEID_UNKNOWN5, RF_DEVICEID_WATERPUMP));

    // init Commands Processor
    radioCommandsProcessor.begin();
    controlButtonProcessor.begin();

    radio.printDetails();

    // init WiFi
    wifiAp.begin();
    wifiAp.turnOn();

    // Web Processor
    webProcessor.begin();

    // DNS Capacitive Portal
    dnsServer.start(DNS_PORT, "*", wifiAp.getOwnIP());

    LOG_INFOLN("Initialization completed.");
}

int counter = 0;

void loop() {

    controlButtonProcessor.handle();
    radioCommandsProcessor.handle();
    waterPumpRelay.handle();
    webProcessor.handle();
    dnsServer.processNextRequest();

    if ((counter++ % 5) == 0){
        LOG_DEBUGLN("Tick...");

        if (radio.failureDetected 
          || radio.getChannel() != RADIO_CHANNEL
          || radio.getDataRate() != RADIO_DATARATE
          || radio.getPALevel() != RADIO_PALEVEL){
              LOG_FATALLN("*******************************");
              LOG_FATALLN("RF24 Hardware Failure detected!");
              LOG_FATALF("Failure Detected: %s (expected NO)\n", radio.failureDetected ? "YES" : "NO");
              LOG_FATALF("Channel: %i (expected %i)\n", radio.getChannel(), RADIO_CHANNEL);
              LOG_FATALF("DataRate: %i (expected %i)\n", radio.getDataRate(), RADIO_DATARATE);
              LOG_FATALF("PALevel: %i (expected %i)\n", radio.getPALevel(), RADIO_PALEVEL);

              if (waterPumpRelay.getState() == false && ota.isEnabled() == false) {
                  LOG_FATALLN("*******************************");
                  LOG_FATALLN("Restarting ESP module...");
                  ESP.restart();
              }
          }
    }

    delay(200);
}

#endif