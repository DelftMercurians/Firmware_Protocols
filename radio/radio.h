#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <radio/protocols_radio.h>
#include <radio/pins_radio.h>
#include <queue>

class CustomRF24 : public RF24 {
    public:
        CustomRF24() : RF24() {};
        CustomRF24(rf24_gpio_pin_t _cepin, rf24_gpio_pin_t _cspin) : RF24(_cepin, _cspin) {};

        template<typename T>
        void registerCallback(void (*fun)(T, Radio::SSL_ID));

        void registerVariable(uint32_t *ptr, HG::Variable var);

        void receiveMessage(Radio::Message& msg);


    protected:
        

        uint8_t identity;
        SPIClass* spi;
        uint8_t num_radios_online = 1;

        bool sendMessage(Radio::Message msg);

        template<typename T>
        bool sendMessage(T msgi) {
            return this->sendMessage(Radio::Message{msgi});
        }

        void preInit(rf24_pa_dbm_e pa_level);
        void postInit();

        uint32_t* config_variables[256];

        void handleMultiConfigMessage(Radio::MultiConfigMessage);

        void (*callback_command)(Radio::Command, Radio::SSL_ID) = nullptr;
        // void (*callback_reply)(Radio::Reply, Radio::SSL_ID) = nullptr;
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

const uint8_t MAX_TX_BUFFER = 5;
class CustomRF24_Robot : public CustomRF24 {
    public:
        CustomRF24_Robot();
        bool run();
        
        bool init(uint8_t robot, rf24_pa_dbm_e pa_level = RF24_PA_MIN);

        void writeTxBuffer(uint8_t index, Radio::Message msg);
        using CustomRF24::sendMessage;

        void handleMultiConfigMessage(Radio::MultiConfigMessage);
        bool receiveAndCallback(uint8_t id);

    private:
        Radio::Message txBuffer[MAX_TX_BUFFER];
        uint8_t tx_buffer_len;
        uint8_t tx_rotate;

        std::queue<Radio::Message> txQueue;

        void writeTx();
};

class CustomRF24_Base : public CustomRF24 {
    public:
        CustomRF24_Base(uint8_t group);
        bool run();

        void setRadioID(uint8_t identity);

        bool init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);
        void openPipes(uint8_t num_radios_online);
        void setRxRobot(Radio::SSL_ID rx_robot);

        void handleMultiConfigMessage(Radio::MultiConfigMessage);

        template<typename T>
        bool sendMessageToRobot(T msg, uint8_t rx_robot) {
            this->setRxRobot(rx_robot);
            return this->sendMessage(msg);
        }
        bool receiveAndCallback(uint8_t id);

    private:
        Radio::SSL_ID rx_robot = 0;
        Radio::SSL_ID getID(uint8_t pipe);
        uint8_t getPipe(Radio::SSL_ID id);
};
