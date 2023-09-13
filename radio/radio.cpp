#include "radio.h"

// Initialise Radio
void CustomRF24::init(Radio::Device device, rf24_pa_dbm_e pa_level)  {
	this->begin();
    this->setPayloadSize(sizeof(Radio::Message));
    if(device == Radio::Device::BaseStation) {
        this->openWritingPipe(Radio::DefaultAddress);
        // Open all five reading pipes (one for each robot)
        for(uint8_t pipe = 1; pipe < 6; pipe++){
            this->openReadingPipe(pipe, Radio::DefaultAddress + pipe);
        }
    } else {
        this->openReadingPipe(1, Radio::DefaultAddress);
        this->openWritingPipe(Radio::DefaultAddress + (uint64_t) device);
    }
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
    this->startListening();           // Always idle in receiving mode
}

// Send a generic message
void CustomRF24::sendMessage(Radio::Message msg) {
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
void CustomRF24::sendMessage(Radio::ConfigMessage configMsg) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::ConfigMessage;
    msg.msg.cm = configMsg;
    this->sendMessage(msg);
}
void CustomRF24::sendMessage(Radio::Command command) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::Command;
    msg.msg.c = command; 
    this->sendMessage(msg);
}
void CustomRF24::sendMessage(Radio::Reply reply) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::Reply;
    msg.msg.r = reply; 
    this->sendMessage(msg);
}
void CustomRF24::sendMessage(Radio::Status status) {
    Radio::Message msg;
    msg.mt = Radio::MessageType::Status;
    msg.msg.s = status; 
    this->sendMessage(msg);
}


// Receive a generic message
void CustomRF24::receiveMessage(Radio::Message& msg) {
    this->read(&msg, sizeof(msg));
}

void CustomRF24::registerCallback(void (*fun)(Radio::ConfigMessage, uint8_t)) {
    callback_confmsg = fun;
}

void CustomRF24::registerCallback(void (*fun)(Radio::Command, uint8_t)) {
    callback_command = fun;
}

void CustomRF24::registerCallback(void (*fun)(Radio::Reply, uint8_t)) {
    callback_reply = fun;
}

void CustomRF24::registerCallback(void (*fun)(Radio::Status, uint8_t)) {
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