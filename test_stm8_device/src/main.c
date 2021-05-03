//#include <Arduino.h>

#include "stm8s.h"
#include "stm8s_it.h"    /* SDCC patch: required by SDCC for interrupts */

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}


void main(void)
{

  GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  while (1)
  {
    /* Toggles LEDs */
    GPIO_WriteReverse(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PINS);
    Delay(0xFFFF);
  }

}