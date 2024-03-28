#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>
#include <radio/radio.h>

class RadioStation {
    public:        
        RadioStation();

        void init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);
        void setRxRobot(Radio::Device rx_robot);

        template<typename T>
        void sendMessageTo(T msg, Radio::Device rx_robot);

        template<typename T>
        void registerCallback(void (*fun)(T, uint8_t));

        // template<typename T>
        // void sendMessage(T msg);

        // void receiveMessage(Radio::Message& msg);

        bool run();

    protected:
        CustomRF24_Base radios[Radio::NumberOfRadios];
       
    // private:
        Radio::Device rx_robot = ROBOT_0;

};
