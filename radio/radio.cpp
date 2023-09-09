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

// Receive a generic message
void CustomRF24::receiveMessage(Radio::Message msg) {
    this->read(&msg, sizeof(msg));
}