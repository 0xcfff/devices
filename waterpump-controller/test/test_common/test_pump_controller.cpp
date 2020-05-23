#include "unity.h"

#include "pump_controller.h"

#ifdef UNIT_TEST

#define PIN_RELAY 4

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_isWorking_beforeBeginWhenOffByDefault_shouldReturnFalse(void) {
    PumpController waterPump(PIN_RELAY, PUMPCTL_ENABLE_HIGH | PUMPCTL_START_OFF);
    TEST_ASSERT_EQUAL(false, waterPump.isWorking());
}
void test_isWorking_afterBeginWhenOffByDefault_shouldReturnFalse(void) {
    PumpController waterPump(PIN_RELAY, PUMPCTL_ENABLE_HIGH | PUMPCTL_START_OFF);
    waterPump.begin();
    TEST_ASSERT_EQUAL(false, waterPump.isWorking());
}

void test_isWorking_beforeBeginWhenOnByDefault_shouldReturnFalse(void) {
    PumpController waterPump(PIN_RELAY, PUMPCTL_ENABLE_HIGH | PUMPCTL_START_ON);
    TEST_ASSERT_EQUAL(false, waterPump.isWorking());
}
void test_isWorking_afterBeginWhenOffByDefault_shouldReturnTrue(void) {
    PumpController waterPump(PIN_RELAY, PUMPCTL_ENABLE_HIGH | PUMPCTL_START_ON);
    waterPump.begin();
    TEST_ASSERT_EQUAL(true, waterPump.isWorking());
}

void test_startPump_beforeBegin_shouldReturnFalse(void) {
    PumpController waterPump(PIN_RELAY, PUMPCTL_ENABLE_HIGH | PUMPCTL_START_OFF);
    TEST_ASSERT_EQUAL(false, waterPump.startPump(30));
}

void test_startPump_faterBegin_shouldReturnTrue(void) {
    PumpController waterPump(PIN_RELAY, PUMPCTL_ENABLE_HIGH | PUMPCTL_START_OFF);
    waterPump.begin();
    TEST_ASSERT_EQUAL(true, waterPump.startPump(30));
    delay(500);
}


void process() {
    UNITY_BEGIN();
    RUN_TEST(test_isWorking_beforeBeginWhenOffByDefault_shouldReturnFalse);
    RUN_TEST(test_isWorking_afterBeginWhenOffByDefault_shouldReturnFalse);
    RUN_TEST(test_isWorking_beforeBeginWhenOnByDefault_shouldReturnFalse);
    RUN_TEST(test_isWorking_afterBeginWhenOffByDefault_shouldReturnTrue);
    RUN_TEST(test_startPump_beforeBegin_shouldReturnFalse);
    RUN_TEST(test_startPump_faterBegin_shouldReturnTrue);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
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

#endif

#endif