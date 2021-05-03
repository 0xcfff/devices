#include <Arduino.h>
//#include <SPI.h>
#include <RF24.h>

#define PIN_RF_CE D3
#define PIN_RF_CSN D8
#define PIN_LED LED_BUILTIN

//const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };  
//const uint64_t myPipe = 0xabababab01LL; 
//char * buffer = new char[255];

#define RF24_CHANNELS 128
#define SCAN_CYCLES 10

uint8_t channelStatus[RF24_CHANNELS];
uint8_t channelHistoricStatus[RF24_CHANNELS];

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

  // radio.setChannel(55);
  // radio.setPALevel(RF24_PA_HIGH);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
  // radio.setDataRate(RF24_250KBPS);
  // radio.setAutoAck(1);                     // Ensure autoACK is enabled
  //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
  //radio.enableDynamicPayloads();
  //radio.enableDynamicAck();
  //radio.setPayloadSize(12);
  // radio.enableDynamicPayloads();
  
  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

  //radio.openWritingPipe(pipes[1]);
  //radio.openReadingPipe(1,myPipe);

  radio.setAutoAck(false);
  
  radio.printDetails();
  radio.startListening();
  radio.stopListening();

  // Led
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  delay(100);
  digitalWrite(PIN_LED, HIGH);
  
  // clear historic values
  memset(channelHistoricStatus, 0, sizeof(uint8_t) * RF24_CHANNELS);
}

bool ledEnabled = false;
int loopsCount = 0;

void loop() {
  // Cleanup channel status array
  memset(channelStatus, 0, sizeof(uint8_t) * RF24_CHANNELS);

  for(int iCycle=0; iCycle<SCAN_CYCLES; iCycle++)
  {
    for (int iChannel = 0; iChannel < RF24_CHANNELS; iChannel++)
    {
      // select channel
      radio.setChannel(iChannel);

      // listen 40 msec
      radio.startListening();
      delayMicroseconds(100);
      radio.stopListening();

      // read RPD, update state
      auto signalFound = radio.testRPD();
      channelStatus[iChannel] += (signalFound ? 1 : 0);


      if (channelStatus[iChannel] > channelHistoricStatus[iChannel]) {
        channelHistoricStatus[iChannel] = channelStatus[iChannel];
      }
    }
  }

  int div = 4;
  int historyPrintPeriod = 4;

  // print historic data if needed
  if (loopsCount > 0 && (loopsCount % historyPrintPeriod) == 0) {
    Serial.printf("History: |");
    for (int iChannel = 0; iChannel < RF24_CHANNELS / div; iChannel++)
    {
      int aggregate = 0;
      for (int ch = iChannel * div; ch < iChannel * div + div; ch++)
      {
        aggregate += channelHistoricStatus[ch];
      }
      
      Serial.printf("%3i|", (int)aggregate);
    }
    Serial.printf("\n");
  }


  // print aggregated values
  Serial.printf("Latest:  |");
  for (int iChannel = 0; iChannel < RF24_CHANNELS / div; iChannel++)
  {
    int aggregate = 0;
    for (int ch = iChannel * div; ch < iChannel * div + div; ch++)
    {
      aggregate += channelStatus[ch];
    }
    
    Serial.printf("%3i|", (int)aggregate);
  }
  Serial.printf("\n");

  // print channels
  Serial.printf("Channel: |");
  for (int iChannel = 0; iChannel < RF24_CHANNELS / div; iChannel++)
  {
    Serial.printf("%3i|", iChannel * div);
  }
  Serial.printf("\n\n");
  loopsCount++;
  
  delay(5000);
}