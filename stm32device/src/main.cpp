#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <rfchannel.h>
#include <macro-logs.h>


#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define DISPLY_I2C_ADDRESS 0x27
#define COLUMS 20
#define ROWS   4

#define PIN_RF_CE   PB0
//#define PIN_RF_CSN  A4
#define PIN_RF_CSN  PA4

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
const uint64_t myPipe = 0xabababab01LL; 
const char * testMessage = "test message";

uint8_t buff[32];


RF24 radio(PIN_RF_CE, PIN_RF_CSN);
RFChannel channel(&radio, 200, 10 * 1000);

void setup() {

    // init serial
    Serial.begin(115200);   

    LOG_INFOLN("Initializing...");

    Wire.begin();

    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);


    // LCD
    if (!lcd.begin(COLUMS, ROWS)) {
        LOG_ERRORLN("Error init LCD!");
    } else {
        LOG_INFOLN("LCD init success");
        lcd.clear();
        lcd.backlight();
    }

    //SPI.setMISO()

    // init radio
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);

    bool radioInit = radio.begin();
    delay(100);
    LOG_INFOF("Radio init %s\n", radioInit ? "success!" : "failure!");
    
    radio.setChannel(60);
    radio.setPALevel(RF24_PA_HIGH);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
    radio.setDataRate(RF24_250KBPS);
    radio.setAutoAck(1);                     // Ensure autoACK is enabled
    //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
    radio.enableDynamicPayloads();
    //radio.setPayloadSize(12);

    //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

    // Use pipes[1] as listening pipe
    radio.openWritingPipe(myPipe);
    radio.openReadingPipe(1, pipes[1]);

    //radio.stopListening();

    //radio.startListening();

    channel.begin();
    channel.openRedingPipe(1, pipes[1]);

    radio.printDetails();
    radio.startListening();
}

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


//bool isDisabled = true;

unsigned long lastReceived = 0;
unsigned long lastUpdated = 0;

void loop() {

    //detectDevices();

    // isDisabled = !isDisabled;
    // digitalWrite(LED_BUILTIN, isDisabled ? HIGH : LOW);
    // Serial.println(isDisabled ? "Off" : "On");
    // // LCD
    // lcd.clear();
    // lcd.setCursor(0,0);
    // lcd.print("LED State: ");
    // lcd.println(isDisabled ? "Off" : "On");
    // // put your main code here, to run repeatedly:

    // bool writtenlastUpdated = radio.write(testMessage, 12);
    // Serial.print("Write ");
    // Serial.print(written ? "successful" : "failed");
    // Serial.println();

    // Serial.println(radio.isChipConnected() ? "chip connected" : "no chip connected");

    // uint8_t divider = SPI_CLOreturnCK_DIV32;
    // _SPI.setClockDivider(divider);
    // SPI.setClockDivider(divider);

    // bool radioInit = radio.begin(); 
    // radio.openWritingPipe(myPipe);
    // radio.openReadingPipe(1, pipes[1]);
    // radio.setDataRate(RF24_250KBPS);
    // Serial.println(radioInit ? "init successful" : "init failed");
    //radio.printDetails();

    channel.handle();

    if (channel.getContentAvailable() != RFCHANNEL_CONTENT_NONE) {
        if (channel.getContentAvailable(1) == RFCHANNEL_CONTENT_DATA) {
            int msgSize = channel.getContentSize(1);
            Serial.printf("Received %i bytes\n", (int) msgSize);

            channel.receiveContent(1, buff, msgSize > 32 ? 32 : msgSize);
            if (msgSize >= sizeof(uint8_t) * 6) {


                uint8_t * values = (uint8_t*)(void*)buff;
                
                Serial.printf("Humidity: %i.%u %%\n", (int)*values, (int)*(values+1));
                Serial.printf("Temp: %i.%u °C\n", (int)*(values+2), (int)*(values+3));
                Serial.printf("Heat idx: %i.%u °C\n", (int)*(values+4), (int)*(values+5));

                lcd.clear();
                
                lcd.setCursor(0,0);
                lcd.printf("Humidity: %i.%u %%", (int)*values, (int)*(values+1));
                lcd.setCursor(0,1);
                lcd.printf("Temp: %i.%u °C", (int)*(values+2), (int)*(values+3));
                lcd.setCursor(0,2);
                lcd.printf("Heat idx: %i.%u °C", (int)*(values+4), (int)*(values+5));

                
                lastReceived = millis();
                lastUpdated = lastReceived - 2000; // to trigger redraw
            }

        } else {
            channel.clearContent(1);
        }

    } 
    
    if ( millis() - lastUpdated > 1000 ) {
        lcd.setCursor(0,3);
        lcd.printf("                    ");
        lcd.setCursor(0,3);
        lcd.printf("Upd: %lu sec ago", ((millis() - lastReceived) / 1000) );
        lastUpdated = millis();
    }

    Serial.printf("do");
    delay(300);
}