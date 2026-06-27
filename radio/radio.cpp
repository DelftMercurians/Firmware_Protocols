#include "radio.h"
#include "radio/protocols_radio.h"
#include <logger/default_logger.hpp>

Logger* CustomRF24::getDefaultLogger() {
    static DefaultLogger default_logger;
    return &default_logger;
}

void CustomRF24::setLogger(Logger* logger_instance) {
    this->logger = logger_instance ? logger_instance : getDefaultLogger();
}

// Initialise Radio
void CustomRF24::preInit(rf24_pa_dbm_e pa_level) {
	this->begin(this->spi);
    this->logger->info("radio", "Initialised radio, identity=0x%X", this->identity);
    if (this->failureDetected) {
        this->logger->warn("radio", "Failure detected during RF24 bring-up");
    }
    if (!this->isChipConnected()) {
        this->logger->error("radio", "RF24 chip connection failed");
    }
    if (!this->isPVariant()) {
        this->logger->warn("radio", "RF24 is not P-variant");
    }
    // this->setPayloadSize(sizeof(Radio::Message));
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
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
    this->startFastWrite(&msg, sizeof(msg), multicast);
    return true;
    // return this->write(&msg, sizeof(msg), multicast);
    // if(!res) {
    //     Serial.print("F\n");
    // }
    // this->startListening();
}

// Receive a generic message
void CustomRF24::receiveMessage(Radio::Message& msg) {
    this->read(&msg, sizeof(msg));
}
