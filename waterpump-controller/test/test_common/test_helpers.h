#ifndef _TEST_HELPERS_H_
#define _TEST_HELPERS_H_

#ifdef ARDUINO

#include <Arduino.h>

int digitalReadOutputPin(uint8_t pin)
{
 uint8_t bit = digitalPinToBitMask(pin);
 uint8_t port = digitalPinToPort(pin);
 if (port == NOT_A_PIN)
   return LOW;

 return (*portOutputRegister(port) & bit) ? HIGH : LOW;
}

#else

#include <chrono>
#include <thread>

#define delay(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

// define digitalReadOutputPin

#endif

#define TEST_ASSERT_OUTPUT_PIN_IS(expected, pin) TEST_ASSERT_EQUAL(expected, digitalReadOutputPin(pin))
#define TEST_ASSERT_OUTPUT_PIN_IS_HIGH (pin) TEST_ASSERT_OUTPIN_IS(HIGH, pin)
#define TEST_ASSERT_OUTPUT_PIN_IS_LOW (pin) TEST_ASSERT_OUTPIN_IS(LOW, pin)

#endif