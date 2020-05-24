#include "unity.h"

#include "relay.h"

#ifdef ARDUINO

#include <Arduino.h>

#define PIN_RELAY D2

#else

#include <chrono>
#include <thread>

#define PIN_RELAY 4

#define delay(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#endif

//#ifdef UNIT_TEST


// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_getState_beforeBeginWhenOffByDefault_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
    TEST_ASSERT_EQUAL(false, relay.getState());
}
void test_getState_afterBeginWhenOffByDefault_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
    relay.begin();
    TEST_ASSERT_EQUAL(false, relay.getState());
}

void test_getState_beforeBeginWhenOnByDefault_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_ON);
    TEST_ASSERT_EQUAL(false, relay.getState());
}
void test_getState_afterBeginWhenOffByDefault_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_ON);
    relay.begin();
    TEST_ASSERT_EQUAL(true, relay.getState());
}

void test_turnOn_beforeBegin_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
    TEST_ASSERT_EQUAL(false, relay.turnOn(30));
}

void test_autoTurnOff_whenTurnOnWorkDurationSet_shouldTurnOffRelayWhenDurationEnds(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
    relay.begin();
    relay.turnOn(30);
    delay(60);
    TEST_ASSERT_EQUAL(false, relay.getState());
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_getState_beforeBeginWhenOffByDefault_shouldReturnFalse);
    RUN_TEST(test_getState_afterBeginWhenOffByDefault_shouldReturnFalse);
    RUN_TEST(test_getState_beforeBeginWhenOnByDefault_shouldReturnFalse);
    RUN_TEST(test_getState_afterBeginWhenOffByDefault_shouldReturnTrue);
    RUN_TEST(test_turnOn_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_autoTurnOff_whenTurnOnWorkDurationSet_shouldTurnOffRelayWhenDurationEnds);
    UNITY_END();
}

#ifdef ARDUINO

void setup() {
    process();
}

void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

//#endif

#endif