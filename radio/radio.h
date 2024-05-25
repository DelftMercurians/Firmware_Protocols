#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>
#include <radio/pins_radio.h>

class CustomRF24 : public RF24 {
    public:
        CustomRF24() : RF24() {};
        CustomRF24(rf24_gpio_pin_t _cepin, rf24_gpio_pin_t _cspin) : RF24(_cepin, _cspin) {};

        template<typename T>
        void registerCallback(void (*fun)(T, Radio::SSL_ID));

        void receiveMessage(Radio::Message& msg);


    protected:
        bool receiveAndCallback(uint8_t id);

        uint8_t identity;
        SPIClass* spi;
        uint8_t num_radios_online = 1;

        template<typename T>
        void sendMessage(T msg);

        void preInit(rf24_pa_dbm_e pa_level);
        void postInit();

        void (*callback_confmsg)(Radio::ConfigMessage, Radio::SSL_ID) = nullptr;
        void (*callback_command)(Radio::Command, Radio::SSL_ID) = nullptr;
        void (*callback_reply)(Radio::Reply, Radio::SSL_ID) = nullptr;
        // void (*callback_status)(Radio::Status, uint8_t) = nullptr;
        void (*callback_status_hf)(Radio::PrimaryStatusHF, Radio::SSL_ID) = nullptr;
        void (*callback_status_lf)(Radio::PrimaryStatusLF, Radio::SSL_ID) = nullptr;
        void (*callback_imu_readings)(Radio::ImuReadings, Radio::SSL_ID) = nullptr;
        void (*callback_odo_reading)(Radio::OdometryReading, Radio::SSL_ID) = nullptr;
        void (*callback_override_odo)(Radio::OverrideOdometry, Radio::SSL_ID) = nullptr;
        void (*callback_msg)(Radio::Message, Radio::SSL_ID) = nullptr;
};

// Note on callbacks:
// For Robot
//  second param is the incoming pipe number (0 to 5)
// For Basestation
//  second param is the robot id the transmission is from (for)

class CustomRF24_Robot : public CustomRF24 {
    public:
        CustomRF24_Robot();
        bool run();
        
        bool init(uint8_t robot, rf24_pa_dbm_e pa_level = RF24_PA_MIN);

        using CustomRF24::sendMessage;
};

class CustomRF24_Base : public CustomRF24 {
    public:
        CustomRF24_Base(uint8_t group);
        bool run();

        void setRadioID(uint8_t identity);

        bool init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);
        void openPipes(uint8_t num_radios_online);
        void setRxRobot(Radio::SSL_ID rx_robot);

        template<typename T>
        void sendMessageToRobot(T msg, uint8_t rx_robot) {
            this->setRxRobot(rx_robot);
            this->sendMessage(msg);
        }

    private:
        Radio::SSL_ID rx_robot = 0;
        Radio::SSL_ID getID(uint8_t pipe);
        uint8_t getPipe(Radio::SSL_ID id);
};
