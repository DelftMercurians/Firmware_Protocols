#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>
using namespace Radio;

class CustomRF24 : public RF24 {
    public:
        CustomRF24() : RF24(PB0, PA4) {}

        void init(Address address_r, Address address_w, uint8_t pipe = 1, rf24_pa_dbm_e pa_level = RF24_PA_MIN) ;

        void sendMessage(Message msg);
        void sendMessage(ConfigMessage configMsg);
        void sendMessage(Command command);
        void sendMessage(Reply reply);

        void receiveMessage(Message &msg);
};