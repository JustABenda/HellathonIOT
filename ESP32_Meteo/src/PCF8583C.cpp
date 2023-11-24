#include <PCF8583C.hpp>

PCF8583C::PCF8583C(int sda, int scl, int address){
    rtc = new PCF8583(address, sda, scl);
    rtc->setMode(MODE_EVENT_COUNTER);
    rtc->setCount(0);
}
void PCF8583C::reset(){
    rtc->setCount(0);
}
int PCF8583C::getCount(){
    return rtc->getCount();
}
