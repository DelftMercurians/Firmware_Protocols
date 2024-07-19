#include "radio.h"
#include "radio/protocols_radio.h"

// ---------------BASE------------------ //
CustomRF24_Base::CustomRF24_Base(uint8_t group)
    : CustomRF24(RadioPins::GroupPinMap[group].ce,  RadioPins::GroupPinMap[group].cs)
 {
    this->identity = group;
    RadioPins::RadioPins pins = RadioPins::GroupPinMap[this->identity];
    if (pins.spi_bus == RadioPins::SpiBus::Spi_1) {
        this->spi = new SPIClass(PA7, PA6, PA5);
    } else if (pins.spi_bus == RadioPins::SpiBus::Spi_2) {
        this->spi = new SPIClass(PB15, PB14, PB13);
    }
}

void CustomRF24_Base::setRadioID(uint8_t identity) {
    this->identity = identity;
}

bool CustomRF24_Base::init(rf24_pa_dbm_e pa_level) {
    this->preInit(pa_level);
    this->startListening();           // Always idle in listening mode
    this->setAutoAck(false);
    return this->isChipConnected();
}

bool CustomRF24_Base::run() {
    uint8_t pipe = 0;
    if(!this->available(&pipe)){
        // No message received
        return false;
    }
    // if(pipe == 0) {
        return receiveAndCallback(pipe + 1);  // Received on basestation backlistening pipe
    // } else {
    //     return receiveAndCallback(Radio::getRobotID(pipe, identity, num_radios_online));
    // }
}

void CustomRF24_Base::registerCallback(void (*fun)(Radio::Message, Radio::SSL_ID)){
    callback_msg = fun;
}

void CustomRF24_Base::openPipes(uint8_t num_radios_online) {
    this->num_radios_online = num_radios_online;
    // this->openWritingPipe(Radio::BaseAddress_BtR + (uint64_t) this->rx_robot);
    // Open all five reading pipes (one for each robot)
    for (uint8_t pipe = 0; pipe <= 5; pipe++) {
        this->openReadingPipe(pipe, Radio::BaseAddress_BtR + pipe + 1);
    }
}

bool CustomRF24_Base::receiveAndCallback(Radio::SSL_ID id) {
    Radio::Message msg;
    auto size = getDynamicPayloadSize();
    receiveMessage(msg);

    if(callback_msg != nullptr){
        callback_msg(msg, id);
    }
    return true;
}


void CustomRF24_Base::setRxRobot(Radio::SSL_ID rx_robot) {
    if(rx_robot != this->rx_robot) {
        this->rx_robot = rx_robot;
        this->openWritingPipe(Radio::BaseAddress_BtR + (uint64_t) this->rx_robot);
    }
}

void CustomRF24_Base::setRxBroadcast() {
    if(0xEE != this->rx_robot) {
        this->rx_robot = 0xEE;
        this->openWritingPipe(Radio::BroadcastAddress);
    }
}

// Get robot ssl id based on pipe of current radio
Radio::SSL_ID CustomRF24_Base::getID(uint8_t pipe) {
    if(pipe < 1 || pipe > 5) return 255;            // Invalid pipe number
    if(identity >= num_radios_online) return 255;   // Invalid identity, given number of online radios
    return Radio::getRobotID(pipe, identity, num_radios_online);
}
// Get pipe for specific robot id
uint8_t CustomRF24_Base::getPipe(Radio::SSL_ID id) {
    if(Radio::getRadioID(id, num_radios_online) != identity) return 0;    // ID is not assigned to this radio
    return Radio::getPipe(id, num_radios_online);
}
