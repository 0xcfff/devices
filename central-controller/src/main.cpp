#include <Arduino.h>

#include <JC_Button.h>

#include "log_macroses.h"
#include "dacha1_network.h"
#include "pump_messages.h"

#define PIN_RF_CE D2
#define PIN_RF_CSN D8

#define PIN_CONTROL_BUTTON D1

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
const uint64_t myPipe = 0xCCCCCCC1C0LL; 
//const char * testMessage = "test message";
uint8_t buffer[255];
int cyclesCount = 0;

RF24 radio(PIN_RF_CE, PIN_RF_CSN);
Button controlButton(PIN_CONTROL_BUTTON);


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
}

void loop() {
  // put your main code here, to run repeatedly:
  //  bool written = radio.write(testMessage, 12);
// bool written = radio.write(testMessage, 12);

//  int len = sprintf(buffer, "Cycle: %i, millis since start %i msec", cyclesCount++, millis());

    controlButton.read();

    if (controlButton.wasPressed()){

//    if ((counter++ %2)==0) {
        RfRequest * pReq = (RfRequest*)buffer;
        pReq->header.flags = 0;
        pReq->header.command = PUMP_FLIP;
        pReq->body.pumpStartOrFlip.durationSec = 30;
        bool written = radio.write(buffer, sizeof(RfRequestHeader) + sizeof(PumpControlStartOrFlipBody));

        Serial.printf("Write %s\n", written ? "successful" : "failed");
        Serial.printf("Size of ulong %i\n", sizeof(unsigned long));
        Serial.println();
    }

    delay(200);
}