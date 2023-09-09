#include "radio.h"

// Initialise Radio
void CustomRF24::init(uint64_t address, rf24_pa_dbm_e pa_level, bool idleRx) {
    this->idleRx = idleRx;
	this->begin();
	this->openReadingPipe(1, address);   //Setting the address at which we will receive the data
	this->openWritingPipe(address+1);
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
	if(idleRx){
        this->startListening();              //This sets the module as receiver
    }
}

// Send a generic message
void CustomRF24::sendMessage(Message msg) {
    if(idleRx) {
        this->stopListening();
        this->write(&msg, sizeof(msg));
        this->startListening();
    } else {
        this->write(&msg, sizeof(msg));
    }
}

// Have a sendMessage() command for every message type
void CustomRF24::sendMessage(ConfigMessage configMsg) {
    Message msg;
    msg.mt = MessageType::ConfigMessage;
    msg.msg.cm = configMsg;
    this->sendMessage(msg);
}
void CustomRF24::sendMessage(Command command) {
    Message msg;
    msg.mt = MessageType::Command;
    msg.msg.c = command; 
    this->sendMessage(msg);
}
void CustomRF24::sendMessage(Reply reply) {
    Message msg;
    msg.mt = MessageType::Reply;
    msg.msg.r = reply; 
    this->sendMessage(msg);
}

// Receive a generic message
void CustomRF24::receiveMessage(Message &msg) {
    if(!idleRx) {
        this->startListening();
        this->read(&msg, sizeof(msg));
        this->stopListening();
    } else {
        this->read(&msg, sizeof(msg));
    }
}