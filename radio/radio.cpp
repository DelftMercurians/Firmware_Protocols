#include "radio.h"

// Initialise Radio
void CustomRF24::init(Address address_r, Address address_w, uint8_t pipe, rf24_pa_dbm_e pa_level) {
	this->begin();
	this->openReadingPipe(pipe, (uint64_t) address_r);   //Setting the address at which we will receive the data
	this->openWritingPipe((uint64_t) address_w);
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
    this->startListening();           // Always idle in receiving mode
}

// Send a generic message
void CustomRF24::sendMessage(Message msg) {
    this->stopListening();
    this->write(&msg, sizeof(msg));
    this->startListening();
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
    this->read(&msg, sizeof(msg));
}