#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define PIN_RF_CE D2
#define PIN_RF_CSN D8

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
const uint64_t myPipe = 0xabababab01LL; 
const char * testMessage = "test message";

RF24 radio(PIN_RF_CE, PIN_RF_CSN);

void setup() {

  // init serial
  Serial.begin(115200);
  Serial.println("Initializing...");

  // init radio
  bool radioInit = radio.begin();
  Serial.print("Radio init ");
  Serial.print(radioInit ? "success" : "failure");
  Serial.println();
  
  radio.setChannel(55);
  radio.setPALevel(RF24_PA_HIGH);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
  radio.enableDynamicPayloads();
  //radio.setPayloadSize(12);

  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

  radio.openWritingPipe(myPipe);
  radio.openReadingPipe(1, pipes[1]);

  radio.printDetails();
}

void loop() {
//  bool written = radio.write(testMessage, 12);
  bool written = radio.write(testMessage, 12);

  Serial.print("Write ");
  Serial.print(written ? "successful" : "failed");
  Serial.println();
  delay(200);
}