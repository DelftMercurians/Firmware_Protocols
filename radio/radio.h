#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>

class CustomRF24 : public RF24 {
    public:
        CustomRF24() : RF24(PB0, PA4) {}

        void init(  Radio::Device device,
                    rf24_pa_dbm_e pa_level = RF24_PA_MIN) ;

        void registerCallback(void (*)(Radio::ConfigMessage, uint8_t));
        void registerCallback(void (*)(Radio::Command, uint8_t));
        void registerCallback(void (*)(Radio::Reply, uint8_t));
        void registerCallback(void (*)(Radio::Status, uint8_t));

        void sendMessage(Radio::Message msg);
        void sendMessage(Radio::ConfigMessage configMsg);
        void sendMessage(Radio::Command command);
        void sendMessage(Radio::Reply reply);
        void sendMessage(Radio::Status status);

        void receiveMessage(Radio::Message& msg);

        bool run();

    private:
        void (*callback_confmsg)(Radio::ConfigMessage, uint8_t) = nullptr;
        void (*callback_command)(Radio::Command, uint8_t) = nullptr;
        void (*callback_reply)(Radio::Reply, uint8_t) = nullptr;
        void (*callback_status)(Radio::Status, uint8_t) = nullptr;
};