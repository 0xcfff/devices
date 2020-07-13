#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <pcf8574_esp.h>
#include <macro-logs.h>


#include <U8g2lib.h>
//#include <JC_Button.h>

#include "arduino-pins.h"
#include "pcf8574-pins.h"
#include "button.h"


#include "dacha1_network.h"
#include "pump_messages.h"

#include "display.h"
#include "keyboard.h"
#include "idle-processor.h"
#include "navigation-controller.h"
#include "waterpump-controller.h"
#include "idle-controller.h"
#include "ota-controller.h"

#include "modes-images.h"

#include "rfframe.h"
#include "rfcommand.h"
#include "rfchannel.h"

#define PIN_RF_CE D3
#define PIN_RF_CSN D8

#define PIN_CONTROL_BUTTON D1 // TODO remove

#define PIN_PCF8574_BUTTON_MODE     0
#define PIN_PCF8574_BUTTON_OK       1
#define PIN_PCF8574_BUTTON_CANCEL   2
#define PIN_PCF8574_INTERRUPT       D4

#define I2C_ADDRESS_DISPLAY 0x3C
#define I2C_ADDRESS_PCF8574 0x20

#define IDLE_SPLASHDISPLAY_MSEC 100

#define INTERVAL_OTA_WAITCONNECTION_MSEC (120 * 1000)
#define INTERVAL_WATERPUMP_PING_MSEC (2 * 1000)
#define INTERWAL_MAIN_DEVICESLEEPING 100
#define INTERWAL_MAIN_DEVICESACTIVE 30

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0xCCCCCCC0C0LL }; 
const uint64_t myPipe = 0xCCCCCCC1C0LL; 
//const char * testMessage = "test message";

const uint64_t TEMP_MY_NETWORK_ADDR         = 0x10C8FFFFFFLL;
const uint64_t TEMP_MY_BROADCAST_ADDR       = 0x10C8551900LL;
const uint64_t TEMP_TARGET_ADDR             = 0xCCCCCCC1C0LL;

uint8_t buffer[255];
int cyclesCount = 0;

RF24 radio(PIN_RF_CE, PIN_RF_CSN);
RFChannel channel(&radio, 1024, 10*1000);

PCF857x pcf8574(I2C_ADDRESS_PCF8574, &Wire);

// Pins
PCF8574DigitalPin modeButtonPin(&pcf8574, PIN_PCF8574_BUTTON_MODE, PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE);
PCF8574DigitalPin okButtonPin(&pcf8574, PIN_PCF8574_BUTTON_OK, PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE);
PCF8574DigitalPin cancelButtonPin(&pcf8574, PIN_PCF8574_BUTTON_CANCEL, PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE);
ArduinoDigitalPin pcf8574InterruptPin(PIN_PCF8574_INTERRUPT, ARDUINODIGPINMODE_UNDEFINED, ARDUINODIGPINCFG_HARDWAREINTERRUPTS_ENABLE);

// Buttons
Button modeButton(&modeButtonPin, BUTTONCFG_ENABLE_HIGH | BUTTONCFG_ATTACHPININTERRUPT);
Button okButton(&okButtonPin, BUTTONCFG_ENABLE_HIGH | BUTTONCFG_ATTACHPININTERRUPT);
Button cancelButton(&cancelButtonPin, BUTTONCFG_ENABLE_HIGH | BUTTONCFG_ATTACHPININTERRUPT);

//Button controlButton(PIN_CONTROL_BUTTON, BUTTONCFG_ENABLE_HIGH);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);


Display display(&u8g2);
Keyboard keyboard(&pcf8574, PIN_PCF8574_BUTTON_MODE, PIN_PCF8574_BUTTON_OK, PIN_PCF8574_BUTTON_CANCEL, PIN_PCF8574_INTERRUPT);
IdleProcessor idleProcessor(display);
NavigationView modeSelectionPresenter(&display);
MainController mainController(&modeButton, &okButton, &cancelButton, &modeSelectionPresenter);

WaterPumpView waterPumpView(&display);
WaterPumpController waterPumpController(&waterPumpView, &radio, INTERVAL_WATERPUMP_PING_MSEC);
NavigationTargetDescriptor waterPumpControlMode = {
    .modeName = "Water Pump",
    .flags = MODEDESCR_FLAG_DEFAULTMODE,
    .splashScreenXBM = Pump_bits,
    .splashWidth = Pump_width,
    .splashHeight = Pump_height
};

IdleController idleController(&display, IDLE_SPLASHDISPLAY_MSEC);
NavigationTargetDescriptor idleControllerMode = {
    .modeName = "Sleep",
    .flags = MODEDESCR_FLAG_NONE,
    .splashScreenXBM = Sleep_bits,
    .splashWidth = Sleep_width,
    .splashHeight = Sleep_height
};

WiFiAP wifiAp;
OtaUpdater otaUpdater;

OtaView otaView(&display);
OtaController otaController(&otaUpdater, &wifiAp, INTERVAL_OTA_WAITCONNECTION_MSEC, &otaView);
NavigationTargetDescriptor otaControllerMode = {
    .modeName = "Update Firmware",
    .flags = MODEDESCR_FLAG_NONE,
    .splashScreenXBM = OTA_bits,
    .splashWidth = OTA_width,
    .splashHeight = OTA_height
};


ICACHE_RAM_ATTR void detectsButtons(DigitalPin* pin) {
    modeButtonPin.riseInterrupt();
    okButtonPin.riseInterrupt();
    cancelButtonPin.riseInterrupt();
}

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

    radio.openReadingPipe(0, TEMP_MY_NETWORK_ADDR);
    radio.openReadingPipe(1, TEMP_MY_BROADCAST_ADDR);
    //radio.openWritingPipe(myPipe);

    //radio.startListening();

    radio.printDetails();

    //controlButton.begin();

    Wire.begin();
    Wire.setClock(100000L);
    pcf8574.begin();
    pcf8574.write8(0);
    //u8g2.begin();

    modeButton.begin();
    okButton.begin();
    cancelButton.begin();

    std::function<void(DigitalPin*)> handler = detectsButtons;
    pcf8574InterruptPin.attachInterruptHandler(handler, CHANGE);

    //attachInterrupt(digitalPinToInterrupt(PIN_PCF8574_INTERRUPT), detectsButtons, CHANGE);

    display.begin();
    mainController.addChildModeController(&waterPumpControlMode, &waterPumpController);
    mainController.addChildModeController(&idleControllerMode, &idleController);
    mainController.addChildModeController(&otaControllerMode, &otaController);

    keyboard.begin();
    otaUpdater.begin();
    wifiAp.begin();
    
    // blink
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
}

int counter = 0;

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


//bool firstTime = true;

void loop() {

    // if (firstTime) {
    //       RfRequest * pReq = (RfRequest*)buffer;
    //       pReq->header.flags = 0;
    //       //pReq->header.command = PUMP_FLIP;
    //       pReq->header.command = PUMP_STATE;
    //       pReq->body.pumpStartOrFlip.durationSec = 10;
    //       bool written = radio.write(buffer, sizeof(RfRequestHeader) + sizeof(PumpControlStartOrFlipBody));

    //       Serial.printf("Write %s\n", written ? "successful" : "failed");
    //       Serial.printf("Size of ulong %i\n", sizeof(unsigned long));
    //       Serial.println();
    //       firstTime = false;
    // }

    // uint8_t ppp;
    // if (radio.available(&ppp)) {
    //     LOG_DEBUGF("Data available at %i\n", (int)ppp);
    // }

    // if (firstTime) {
    //     uint8_t buff[RFFRAME_MAXSIZE];

    //     RFFrameHeader h;
    //     h.flags = RFFRAME_FLAG_COMMAND | RFFRAME_FLAG_SEQFIRSTFRAME | RFFRAME_FLAG_SEQLASTFRAME;
    //     h.fromAddress = TEMP_MY_BROADCAST_ADDR;
    //     h.toAddress = TEMP_TARGET_ADDR;
    //     h.sequenceId = 1;

    //     auto hlen = encodeRFHeader(buff, RFFRAME_MAXSIZE, &h);
    //     uint8_t * byteStream = buff;
    //     byteStream += hlen;
    //     *byteStream = RFCOMMAND_PING;

    //     bool res = false;

    //     RfRequestHeader pumpMessage = {
    //         .command = PUMP_STATE
    //     };

    //     radio.stopListening();

    //     res = channel.sendData(TEMP_MY_BROADCAST_ADDR, TEMP_TARGET_ADDR, 0, &pumpMessage, sizeof(RfRequestHeader), true);
    //     LOG_INFOF("Sending is %s\n", res ? "successful" : "failed");

    //     //bool res = radio.write(buff, hlen+1);
    //     uint8_t cmd = RFCOMMAND_PING;
    //     //bool res = channel.sendFrame(&h, &cmd, 1, true);
    //     res = channel.sendCommand(TEMP_MY_BROADCAST_ADDR, TEMP_TARGET_ADDR, 0, RFCOMMAND_PING, nullptr, 0, false);
    //     LOG_INFOF("Sending is %s\n", res ? "successful" : "failed");
    //     res = channel.sendCommand(TEMP_MY_BROADCAST_ADDR, TEMP_TARGET_ADDR, 0, RFCOMMAND_PING, nullptr, 0, true);
    //     LOG_INFOF("Sending is %s\n", res ? "successful" : "failed");

    //     //radio.startListening();

    //     //radio.openReadingPipe(0, TEMP_MY_NETWORK_ADDR);

    //     radio.printDetails();
        
    //     firstTime = false;
    // }

    mainController.handle();

    if (display.isDirty()){
        display.flush();
    }

    uint16_t sleepInterval = mainController.isSleeping()
        ? INTERWAL_MAIN_DEVICESLEEPING
        : INTERWAL_MAIN_DEVICESACTIVE;

    delay(sleepInterval);
}