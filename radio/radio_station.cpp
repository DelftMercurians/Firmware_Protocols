#include "radio_station.h"

void RadioStation::init(rf24_pa_dbm_e pa_level) {
    for (int i = 0; i < Radio::NumberOfRadios; i++) {
        radios[i] = CustomRF24_Base(static_cast<Radio::Group>(i));
        radios[i].init();
    }
}

void RadioStation::setRxRobot(Radio::Device rx_robot) {
    Radio::Group group = Radio::RobotGroupMap.at(rx_robot);
    radios[(int)group].setRxRobot(rx_robot);
}

template<typename T>
void RadioStation::sendMessageTo(T msg, Radio::Device rx_robot) {
    Radio::Group group = Radio::RobotGroupMap.at(rx_robot);
    radios[(int)group].sendMessageTo(msg, rx_robot);
}

template<typename T>
void RadioStation::registerCallback(void (*fun)(T, uint8_t)) {
    for (int i = 0; i < Radio::NumberOfRadios; i++) {
        radios[i].registerCallback(fun);
    }
}

bool RadioStation::run() {
    bool res = false ;
    for (int i = 0; i < Radio::NumberOfRadios; i++) {
        res = radios[i].run() || res;
    }
    return res;
}