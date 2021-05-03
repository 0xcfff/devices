#include <Arduino.h>
#include <EEPROM.h>

//char * passwd = "qwerty1";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(512);

  //  auto eepromData = EEPROM.getDataPtr();
  //  strcpy((char *)eepromData, passwd);
  //  EEPROM.commit();

  //  EEPROM.begin(512);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.printf("Password: '%s' \n", (const char *)EEPROM.getDataPtr());

  delay(300);
}