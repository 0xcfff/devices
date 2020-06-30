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
#define INTERVAL_WATERPUMP_PING_MSEC (10 * 1000)

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
const uint64_t myPipe = 0xCCCCCCC1C0LL; 
//const char * testMessage = "test message";
uint8_t buffer[255];
int cyclesCount = 0;

RF24 radio(PIN_RF_CE, PIN_RF_CSN);

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
  Serial.println("Click Detected!!!");

  modeButtonPin.riseInterrupt();
  okButtonPin.riseInterrupt();
  cancelButtonPin.riseInterrupt();
  //ESP.restart();
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

    radio.openWritingPipe(myPipe);
    radio.openReadingPipe(1, pipes[1]);

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

void drawLogo(void)
{
    u8g2.setFontMode(1);	// Transparent
#ifdef MINI_LOGO

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(0, 22, "U");
    
    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb19_mn);
    u8g2.drawStr(14,8,"8");
    
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(36,22,"g");
    u8g2.drawStr(48,22,"\xb2");
    
    u8g2.drawHLine(2, 25, 34);
    u8g2.drawHLine(3, 26, 34);
    u8g2.drawVLine(32, 22, 12);
    u8g2.drawVLine(33, 23, 12);
#else

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(0, 30, "U");
    
    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb30_mn);
    u8g2.drawStr(21,8,"8");
        
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(51,30,"g");
    u8g2.drawStr(67,30,"\xb2");
    
    u8g2.drawHLine(2, 35, 47);
    u8g2.drawHLine(3, 36, 47);
    u8g2.drawVLine(45, 32, 12);
    u8g2.drawVLine(46, 33, 12);
    
#endif
}

void drawURL(void)
{
#ifndef MINI_LOGO
  u8g2.setFont(u8g2_font_4x6_tr);
  if ( u8g2.getDisplayHeight() < 59 )
  {
    u8g2.drawStr(89,20,"github.com");
    u8g2.drawStr(73,29,"/olikraus/u8g2");
  }
  else
  {
    u8g2.setFont(u8g2_font_4x6_t_cyrillic);
    //u8g2.drawStr(1,54,"github.com/olikraus/u8g2");
    u8g2.enableUTF8Print();
    u8g2.setCursor(1,54);
    u8g2.print("Привет!");

    //u8g2.drawXBM(96, 16, PumpDisabled_width, PumpDisabled_height, PumpDisabled_bits);        
  }
#endif
}

bool isHighLighted = false;

const char * main_list = "Living Room\nBedroom\nBedtime\nWatching TV\nGoing Out!";

//bool firstTime = true;

void loop() {
//  bool written = radio.write(testMessage, 12);
// bool written = radio.write(testMessage, 12);

//  int len = sprintf(buffer, "Cycle: %i, millis since start %i msec", cyclesCount++, millis());

    //detectDevices();

    //u8g2.clearBuffer();

    // if (firstTime) {
    //     u8g2.clearBuffer();
    //     drawLogo();
    //     drawURL();
    //     u8g2.sendBuffer();        
    //     firstTime = false;
    // }

    // uint8_t ios = pcf8574.read8();
    // Serial.printf("PCF State: %i\n", (int)ios);
    // if (ios > 0) {
    //     isHighLighted = !isHighLighted;
    //     digitalWrite(LED_BUILTIN, isHighLighted ? HIGH : LOW);

        

    //     if (ios & 1) {
    //         u8g2.userInterfaceSelectionList("Select Room", 2, main_list);
    //     }
    //     if (ios & 2) {
    //         u8g2.userInterfaceSelectionList("Select Room", 1, main_list);
    //     }
    //     if (ios & 4) {
    //         u8g2.noDisplay();
    //         radio.powerDown();
    //     }

    // }

    // u8g2.clearBuffer();
    // mainController.handle();
    // u8g2.sendBuffer();

    mainController.handle();

    if (display.isDirty()){
      display.flush();
    }

    // u8g2.clearBuffer();
    // drawLogo();
    // drawURL();
    // u8g2.sendBuffer();


    //controlButton.read();

    //if (controlButton.wasPressed()){

//    if ((counter++ %2)==0) {
    //     RfRequest * pReq = (RfRequest*)buffer;
    //     pReq->header.flags = 0;
    //     pReq->header.command = PUMP_FLIP;
    //     pReq->body.pumpStartOrFlip.durationSec = 10 * 60;
    //     bool written = radio.write(buffer, sizeof(RfRequestHeader) + sizeof(PumpControlStartOrFlipBody));

    //     Serial.printf("Write %s\n", written ? "successful" : "failed");
    //     Serial.printf("Size of ulong %i\n", sizeof(unsigned long));
    //     Serial.println();
    // }

     delay(200);
}