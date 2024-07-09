#include "radio.h"
#include "radio/protocols_radio.h"

// Initialise Radio
void CustomRF24::preInit(rf24_pa_dbm_e pa_level) {
	this->begin(this->spi);
    Serial.printf("Initialised Radio, IDENTITY = 0x%X\n", this->identity);
	Serial.printf("Failure detected: %s\n", this->failureDetected ? "true" : "false");
	Serial.printf("isChipConnected: %s\n", this->isChipConnected() ? "true" : "false");
	Serial.printf("isPVariant: %s\n", this->isPVariant() ? "true" : "false");
    // this->setPayloadSize(sizeof(Radio::Message));
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
    this->setChannel(76);
    this->enableDynamicPayloads();
    this->enableAckPayload();
}


// Send a generic message
bool CustomRF24::sendMessage(Radio::Message msg, bool multicast) {
    // Serial.print("MSG = ");
    // for(uint8_t i = 0; i < sizeof(msg); i++) {
    //     Serial.printf("%02X ", ((uint8_t*) &msg)[i]);
    // }
    // Serial.println();
    // this->stopListening();
    return this->write(&msg, sizeof(msg), multicast);
    // if(!res) {
    //     Serial.print("F\n");
    // }
    // this->startListening();
}

// Receive a generic message
void CustomRF24::receiveMessage(Radio::Message& msg) {
    this->read(&msg, sizeof(msg));
}
