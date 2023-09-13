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

        template<typename T>
        void registerCallback(void (*fun)(T, uint8_t));

        template<typename T>
        void sendMessage(T msg);

        void receiveMessage(Radio::Message& msg);

        void run();

    private:
        void (*callback_confmsg)(Radio::ConfigMessage, uint8_t) = nullptr;
        void (*callback_command)(Radio::Command, uint8_t) = nullptr;
        void (*callback_reply)(Radio::Reply, uint8_t) = nullptr;
        void (*callback_status)(Radio::Status, uint8_t) = nullptr;
};