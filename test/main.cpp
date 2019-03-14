#include <unity.h>
#include <Arduino.h>

#include <led.hpp>

hackPSU::led* foo;


int iter;

void test_led_state_low(){
    TEST_ASSERT_EQUAL(LOW, digitalRead(4));
}
void test_led_state_high(){
    TEST_ASSERT_EQUAL(HIGH, digitalRead(4));
}

void setup(){
    delay(2000);
    foo = new hackPSU::led("foo", 4);
    iter = 3;

    UNITY_BEGIN();
    RUN_TEST(test_led_state_low);
}


void loop(){
    if(iter){
        foo->write(800);
        RUN_TEST(test_led_state_high);
        delay(500);
        foo->write(0);
        RUN_TEST(test_led_state_low);
        delay(500);
        --iter;
    } else {
        UNITY_END();
    }
}
