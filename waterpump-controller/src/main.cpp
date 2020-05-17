#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <pump_messages.h>

#define PIN_RF_CE D2
#define PIN_RF_CSN D8
#define PIN_EXT_LED D1
#define PIN_RELAY D2
#define PIN_CONTROL D4

#define CONTROL_FLASH_DELAY (3 * 1000)
#define WIFI_AUTO_DISABLE (1 * 60 * 1000)


const uint64_t pipes[2] = { 0xAC0001DD01LL, 0x544d52687CLL };  
const uint64_t myPipe = 0xAC0001DD01LL; 
char * buffer = new char[255];

RF24 radio(PIN_RF_CE, PIN_RF_CSN);


bool wifiEnabled = false;
bool wifiClickHandled = false;
unsigned long wifiLastUsedAt = 0;

bool enableWiFi(bool enable) {
  if (enable) {
    WiFi.mode(WIFI_AP);
    char apName[15];
    sprintf(apName, "esp8266-%06x", ESP.getChipId());
    return WiFi.softAP(apName);
  } else {
    return WiFi.enableAP(false);
  }
}

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

void onStartOTA(){
  wifiLastUsedAt = millis();
}

void onErrorOTA(ota_error_t error){
  const char * errorDescr = NULL;
  switch (error)
  {
  case OTA_AUTH_ERROR:
    errorDescr = "auth";
    break;
  case OTA_BEGIN_ERROR:
    errorDescr = "begin";
    break;
  case OTA_END_ERROR:
    errorDescr = "end";
    break;
  case OTA_RECEIVE_ERROR:
    errorDescr = "receive";
    break;
  default:
    errorDescr = "unknown";
    break;
  }
  Serial.printf("OTA error: %s", errorDescr);
}

void onProgressOTA(unsigned int progress, unsigned int total){
  int progressPercent = (int)(progress * (100 / (float)total));
  Serial.printf("OTA progress: %i%%\n", progressPercent);
  wifiLastUsedAt = millis();
}

void onEndOTA(){
  wifiLastUsedAt = millis();
  Serial.printf("OTA completed, restarting...");
  ESP.restart();
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
  radio.startListening();

  // init LED
  pinMode(PIN_EXT_LED, OUTPUT);

  // init Relay
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  // init Control Button
  pinMode(PIN_CONTROL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_CONTROL), controlButtonChanged, CHANGE);

  // init OTA
  ArduinoOTA.onStart(onStartOTA);
  ArduinoOTA.onError(onErrorOTA);
  ArduinoOTA.onProgress(onProgressOTA);
  ArduinoOTA.onEnd(onEndOTA);
}

bool ledValue = false;
bool pumpEnabled = false;

void loop() {

  if (wifiEnabled){
    ArduinoOTA.handle();
  }

  // Check control button state
  if (controlChanged && !controlPressed){
    controlChanged = false;
    wifiClickHandled = false;
    if (controlClickDurationMillis <= CONTROL_FLASH_DELAY) {
      Serial.println("ESP restart requested! Restarting....");
      ESP.restart();
    } 
  }

  // WiFi long press control
  if (controlPressed && !wifiClickHandled) {
    if (millis() - clickAt > CONTROL_FLASH_DELAY) {      
      if (!wifiEnabled) {
        Serial.println("Enabling WiFi AP...");
        wifiClickHandled = enableWiFi(true);
        wifiEnabled = wifiClickHandled;
        wifiLastUsedAt = millis();
        Serial.println(wifiClickHandled ? "Success" : "Failure");

        if (wifiEnabled) {
          IPAddress ip = WiFi.softAPIP();
          Serial.print("Local IP: ");
          Serial.print(ip);
          Serial.println();

          // Activate OTA mode
          ArduinoOTA.begin();
          Serial.println("OTA enabled...");
        }

      } else {
        Serial.println("Disabling WiFi AP...");
        wifiClickHandled = enableWiFi(false);
        wifiEnabled = !wifiClickHandled;
        Serial.println(wifiClickHandled ? "Success" : "Failure");
      }
    }
  }

  // Auto disable WiFi if usage time has expired
  if (wifiEnabled) {
    if (wifiLastUsedAt + WIFI_AUTO_DISABLE < millis()) {
      Serial.println("Automatic WiFi AP disabling...");
      wifiEnabled = !enableWiFi(false);
      Serial.println(!wifiEnabled ? "Success" : "Failure");
    }

    // update led
    ledValue = !ledValue;
    digitalWrite(PIN_EXT_LED, ledValue && wifiEnabled ? HIGH : LOW);
  }
  
  // ledValue = !ledValue;
  // digitalWrite(PIN_RELAY, ledValue ? HIGH : LOW);
  // digitalWrite(PIN_EXT_LED, ledValue ? HIGH : LOW);
  // Serial.println(ledValue);

  // TODO: radio command should be blocked if OTA is started, active commands should be stopped prior to start OTA
  // Check if any data received via radio
  if (radio.available()) {
    int paySize = radio.getDynamicPayloadSize();
    if (paySize > 255) {
      Serial.println("Too big payload received, skipping...");
    } else {
      radio.read(buffer, paySize);
      buffer[paySize] = 0;
      Serial.print("Received (");
      Serial.print(paySize);
      Serial.println(")");
      Serial.print(buffer);
      Serial.println();
    }
  } else {
    Serial.println("No data received...");
  }


  delay(200);
}