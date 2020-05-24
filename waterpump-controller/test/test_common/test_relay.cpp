#include "unity.h"

#include "relay.h"

#ifdef ARDUINO

#include <Arduino.h>

#else

#include <chrono>
#include <thread>

#define delay(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#endif

#define PIN_RELAY 4


//#ifdef UNIT_TEST


// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

// relay state reading
void test_getState_beforeBegin_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.turnOn();
    TEST_ASSERT_EQUAL(false, relay.getState());
    // TODO: Add assertion of pin state using digitalRead and its fake analog for native plantorm
}
void test_getState_afterBeginWhenTurnedOff_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOff();
    TEST_ASSERT_EQUAL(false, relay.getState());
}
void test_getState_afterBeginWhenTurnedOn_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOn();
    TEST_ASSERT_EQUAL(true, relay.getState());
}

// turnOn
void test_turnOn_beforeBegin_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    TEST_ASSERT_EQUAL(false, relay.turnOn());
}

void test_turnOn_afterBegin_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    TEST_ASSERT_EQUAL(true, relay.turnOn());
}
void test_turnOn_whenCalledSecondTime_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOn();
    delay(30);
    TEST_ASSERT_EQUAL(true, relay.turnOn());
}

// turnOff
void test_turnOff_beforeBegin_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.turnOn();
    TEST_ASSERT_EQUAL(false, relay.turnOff());
}
void test_turnOff_afterBegin_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOn();
    delay(30);
    TEST_ASSERT_EQUAL(true, relay.turnOff());
}
void test_turnOff_whenCalledSecondTime_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOn();
    delay(30);
    relay.turnOff();
    delay(30);
    TEST_ASSERT_EQUAL(true, relay.turnOff());
}

// flip
void test_flip_beforeBegin_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    TEST_ASSERT_EQUAL(false, relay.flip());
}
void test_flip_afterBegin_shouldSwithcStateAndReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOff();
    delay(30);
    TEST_ASSERT_EQUAL(true, relay.flip());
    TEST_ASSERT_EQUAL(true, relay.getState());
}
void test_flip_whenCalledSecondTime_shouldSwithStateBackAndReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOff();
    delay(30);
    relay.flip();
    delay(30);    
    TEST_ASSERT_EQUAL(true, relay.flip());
    TEST_ASSERT_EQUAL(false, relay.getState());
}

// setState
void test_setState_beforeBegin_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);    
    TEST_ASSERT_EQUAL(false, relay.setState(true));
}
void test_setState_afterBegin_shouldUpdateStateAndReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    TEST_ASSERT_EQUAL(true, relay.setState(true));
    TEST_ASSERT_EQUAL(true, relay.getState());
}
void test_setState_whenCalledSecondTime_shouldUpdateStateAndReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.setState(true);
    delay(30);
    TEST_ASSERT_EQUAL(true, relay.setState(false));
    TEST_ASSERT_EQUAL(false, relay.getState());
}

// automatic relay state set on begin
void test_autoInitRelayState_beforeBeginWhenOffByDefault_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
    TEST_ASSERT_EQUAL(false, relay.getState());
}
void test_autoInitRelayState_beforeBeginWhenOnByDefault_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_ON);
    TEST_ASSERT_EQUAL(false, relay.getState());
}
void test_autoInitRelayState_afterBeginWhenOffByDefault_shouldReturnFalse(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_OFF);
    relay.begin();
    TEST_ASSERT_EQUAL(false, relay.getState());
}
void test_autoInitRelayState_afterBeginWhenOnByDefault_shouldReturnTrue(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH | RELAYCTL_START_ON);
    relay.begin();
    TEST_ASSERT_EQUAL(true, relay.getState());
}

// automatic turnOff
void test_autoTurnOff_whenTurnOnWorkDurationSet_shouldTurnOffRelayWhenDurationEnds(void) {
    Relay relay(PIN_RELAY, RELAYCTL_ENABLE_HIGH);
    relay.begin();
    relay.turnOn(30);
    delay(60);
    TEST_ASSERT_EQUAL(false, relay.getState());
}

void process() {
    UNITY_BEGIN();
    // getState
    RUN_TEST(test_getState_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_getState_afterBeginWhenTurnedOff_shouldReturnFalse);
    RUN_TEST(test_getState_afterBeginWhenTurnedOn_shouldReturnTrue);
    // turnOn
    RUN_TEST(test_turnOn_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_turnOn_afterBegin_shouldReturnTrue);
    RUN_TEST(test_turnOn_whenCalledSecondTime_shouldReturnTrue);
    // turnOff
    RUN_TEST(test_turnOff_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_turnOff_afterBegin_shouldReturnTrue);
    RUN_TEST(test_turnOff_whenCalledSecondTime_shouldReturnTrue);
    // flip
    RUN_TEST(test_flip_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_flip_afterBegin_shouldSwithcStateAndReturnTrue);
    RUN_TEST(test_flip_whenCalledSecondTime_shouldSwithStateBackAndReturnTrue);
    // setState
    RUN_TEST(test_setState_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_setState_afterBegin_shouldUpdateStateAndReturnTrue);
    RUN_TEST(test_setState_whenCalledSecondTime_shouldUpdateStateAndReturnTrue);
    // auto init relay
    RUN_TEST(test_autoInitRelayState_beforeBeginWhenOffByDefault_shouldReturnFalse);
    RUN_TEST(test_autoInitRelayState_beforeBeginWhenOnByDefault_shouldReturnFalse);
    RUN_TEST(test_autoInitRelayState_afterBeginWhenOffByDefault_shouldReturnFalse);
    RUN_TEST(test_autoInitRelayState_afterBeginWhenOnByDefault_shouldReturnTrue);
    // auto turn off
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