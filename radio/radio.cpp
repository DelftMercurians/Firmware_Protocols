#include "radio.h"

// Initialise Radio
void CustomRF24::preInit(rf24_pa_dbm_e pa_level) {
	this->begin(this->spi);
    Serial.printf("Initialised Radio, IDENTITY = 0x%X\n", this->identity);
	Serial.printf("Failure detected: %s\n", this->failureDetected ? "true" : "false");
	Serial.printf("isChipConnected: %s\n", this->isChipConnected() ? "true" : "false");
	Serial.printf("isPVariant: %s\n", this->isPVariant() ? "true" : "false");
    this->setPayloadSize(sizeof(Radio::Message));
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
}
void CustomRF24::postInit() {
    this->startListening();           // Always idle in receiving mode
}


// Send a generic message
template<typename T = Radio::Message>
void CustomRF24::sendMessage(T msg) {
    // Serial.print("MSG = ");
    // for(uint8_t i = 0; i < sizeof(msg); i++) {
    //     Serial.printf("%02X ", ((uint8_t*) &msg)[i]);
    // }
    // Serial.println();
    this->stopListening();
    this->write(&msg, sizeof(msg));
    this->startListening();
}

// Have a sendMessage() command for every message type
template<>
void CustomRF24::sendMessage<Radio::ConfigMessage>(Radio::ConfigMessage msgi) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::ConfigMessage;
    msg.msg.cm = msgi;
    this->sendMessage(msg);
}
template<>
void CustomRF24::sendMessage<Radio::Command>(Radio::Command msgi) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::Command;
    msg.msg.c = msgi; 
    this->sendMessage(msg);
}
template<>
void CustomRF24::sendMessage<Radio::Reply>(Radio::Reply msgi) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::Reply;
    msg.msg.r = msgi; 
    this->sendMessage(msg);
}
template<>
void CustomRF24::sendMessage<Radio::Status>(Radio::Status msgi) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::Status;
    msg.msg.s = msgi; 
    this->sendMessage(msg);
}


// Receive a generic message
void CustomRF24::receiveMessage(Radio::Message& msg) {
    this->read(&msg, sizeof(msg));
}

template<>
void CustomRF24::registerCallback<Radio::ConfigMessage>(void (*fun)(Radio::ConfigMessage, uint8_t)) {
    callback_confmsg = fun;
}

template<>
void CustomRF24::registerCallback<Radio::Command>(void (*fun)(Radio::Command, uint8_t)) {
    callback_command = fun;
}

template<>
void CustomRF24::registerCallback<Radio::Reply>(void (*fun)(Radio::Reply, uint8_t)) {
    callback_reply = fun;
}

template<>
void CustomRF24::registerCallback<Radio::Status>(void (*fun)(Radio::Status, uint8_t)) {
    callback_status = fun;
}

bool CustomRF24::run() {
    uint8_t pipe = 0;
    if(!this->available(&pipe)){
        // No message received
        return false;
    }
    Radio::Message msg;
    msg.mt = Radio::MessageType::None;
    receiveMessage(msg);
    switch(msg.mt) {
        case Radio::MessageType::ConfigMessage:
            if(callback_confmsg != nullptr){
                callback_confmsg(msg.msg.cm, pipe);
            }
            return true;
        case Radio::MessageType::Command:
            if(callback_command != nullptr){
                callback_command(msg.msg.c, pipe);
            }
            return true;
        case Radio::MessageType::Reply:
            if(callback_reply != nullptr){
                callback_reply(msg.msg.r, pipe);
            }
            return true;
        case Radio::MessageType::Status:
            if(callback_status != nullptr){
                callback_status(msg.msg.s, pipe);
            }
            return true;
        case Radio::MessageType::None:
            // No message received
            break;
        default:
            //Unknown message type
            // Serial.println(" Unknown message type received!");
            return false;
    }
    return false;
}


// --------------ROBOT------------------ //

CustomRF24_Robot::CustomRF24_Robot()
    : CustomRF24(RadioPins::RobotPinMap.ce, RadioPins::RobotPinMap.cs)
{
    if (RadioPins::RobotPinMap.spi_bus == RadioPins::SpiBus::Spi_1) {
        this->spi = new SPIClass(PA7, PA6, PA5);
    } else if (RadioPins::RobotPinMap.spi_bus == RadioPins::SpiBus::Spi_2) {
        this->spi = new SPIClass(PB15, PB14, PB13);
    }
}


void CustomRF24_Robot::init(uint8_t robot, rf24_pa_dbm_e pa_level) {
    this->identity = robot;
    this->preInit(pa_level);
    this->openReadingPipe(1, Radio::BaseAddress_BtR + (uint64_t) identity);   // Listen on base to robot address
    this->openWritingPipe(Radio::BaseAddress_RtB + (uint64_t) identity);      // Transmit on robot to base address
    this->postInit();
}


// ---------------BASE------------------ //
CustomRF24_Base::CustomRF24_Base(uint8_t group)
    : CustomRF24(RadioPins::GroupPinMap.at(group).ce,  RadioPins::GroupPinMap.at(group).cs)
 {
    this->identity = group;
    RadioPins::RadioPins pins = RadioPins::GroupPinMap.at(group);
    if (pins.spi_bus == RadioPins::SpiBus::Spi_1) {
        this->spi = new SPIClass(PA7, PA6, PA5);
    } else if (pins.spi_bus == RadioPins::SpiBus::Spi_2) {
        this->spi = new SPIClass(PB15, PB14, PB13);
    }
}

void CustomRF24_Base::init(rf24_pa_dbm_e pa_level) {
    this->preInit(pa_level);
    this->openWritingPipe(Radio::BaseAddress_BtR + (uint64_t) this->rx_robot);
    // Open all five reading pipes (one for each robot)
    for (uint8_t pipe = 1; pipe < 5; pipe++) {
        this->openReadingPipe(pipe, Radio::BaseAddress_RtB + 4*identity + pipe);
    }
    this->postInit();
}

void CustomRF24_Base::setRxRobot(uint8_t rx_robot) {
    if(rx_robot != this->rx_robot) {
        this->rx_robot = rx_robot;
        this->openWritingPipe(Radio::BaseAddress_BtR + (uint64_t) this->rx_robot);
    }
}

/*
        if(rx_robot >= Radio::Device::Sniff_Robot_0) {
            this->openReadingPipe(0, (Radio::BaseAddress_BtR - 10) + (uint64_t) this->rx_robot);
        } else {
            
        }
*/