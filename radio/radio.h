#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>
#include <radio/pins_radio.h>

class CustomRF24 : public RF24 {
    public:                
        template<typename T>
        void registerCallback(void (*fun)(T, uint8_t));

        void receiveMessage(Radio::Message& msg);

        bool run();

    protected:
        uint8_t identity;
        SPIClass* spi;

        template<typename T>
        void sendMessage(T msg);

        void preInit(rf24_pa_dbm_e pa_level);
        void postInit();

        void (*callback_confmsg)(Radio::ConfigMessage, uint8_t) = nullptr;
        void (*callback_command)(Radio::Command, uint8_t) = nullptr;
        void (*callback_reply)(Radio::Reply, uint8_t) = nullptr;
        void (*callback_status)(Radio::Status, uint8_t) = nullptr;
};

class CustomRF24_Robot : public CustomRF24 {
    public:
        CustomRF24_Robot(uint8_t robot);
        
        void init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);

        using CustomRF24::sendMessage;
};

class CustomRF24_Base : public CustomRF24 {
    public:
        CustomRF24_Base(uint8_t group);
        CustomRF24_Base() {}; // TODO: this is a hack since we use an array of these in the radio station, should find a better way

        void init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);
        void setRxRobot(uint8_t rx_robot);

        template<typename T>
        void sendMessageToRobot(T msg, uint8_t rx_robot);

    private:
        uint8_t rx_robot = 0;
};
