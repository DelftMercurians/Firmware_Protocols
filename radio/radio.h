#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>

class CustomRF24 : public RF24 {
    public:
        CustomRF24() : RF24(PB0, PA4) {}

        template<typename T>
        void registerCallback(void (*fun)(T, uint8_t));

        template<typename T>
        void sendMessage(T msg);

        void receiveMessage(Radio::Message& msg);

        bool run();

    protected:
        Radio::Device identity;

        void preInit(Radio::Device device, rf24_pa_dbm_e pa_level);
        void postInit();

        void (*callback_confmsg)(Radio::ConfigMessage, uint8_t) = nullptr;
        void (*callback_command)(Radio::Command, uint8_t) = nullptr;
        void (*callback_reply)(Radio::Reply, uint8_t) = nullptr;
        // void (*callback_status)(Radio::Status, uint8_t) = nullptr;
        void (*callback_status_hf)(Radio::PrimaryStatusHF, uint8_t) = nullptr;
        void (*callback_status_lf)(Radio::PrimaryStatusLF, uint8_t) = nullptr;
        void (*callback_imu_readings)(Radio::ImuReadings, uint8_t) = nullptr;
};

class CustomRF24_Robot : public CustomRF24 {
    public:
        void init(Radio::Device device, rf24_pa_dbm_e pa_level = RF24_PA_MIN);
};

class CustomRF24_Base : public CustomRF24 {
    public:
        void init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);
        void setRxRobot(Radio::Device rx_robot);

        template<typename T>
        void sendMessageTo(T msg, Radio::Device rx_robot) {
            this->setRxRobot(rx_robot);
            this->sendMessage(msg);
        }

    // private:
        Radio::Device rx_robot = ROBOT_0;
};
