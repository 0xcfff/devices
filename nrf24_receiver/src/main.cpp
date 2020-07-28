#include <Arduino.h>
//#include <SPI.h>
#include <RF24.h>

#define PIN_RF_CE D3
#define PIN_RF_CSN D8
#define PIN_LED LED_BUILTIN

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };  
const uint64_t myPipe = 0xabababab01LL; 
char * buffer = new char[255];

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
  //radio.enableDynamicPayloads();
  //radio.enableDynamicAck();
  //radio.setPayloadSize(12);
  radio.enableDynamicPayloads();
  
  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,myPipe);

  radio.printDetails();
  radio.startListening();

  // Led
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
}

bool ledEnabled = false;

void loop() {
//  bool written = radio.write(testMessage, 12);
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
    ledEnabled = !ledEnabled;
    digitalWrite(PIN_LED, ledEnabled ? LOW : HIGH);
    radio.writeAckPayload(1, "OK", 3);
  }
} else {
    Serial.println("No data received");
  }
  delay(200);
}