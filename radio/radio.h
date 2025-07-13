#pragma once
#pragma GCC system_header // Silence unnamed warnings

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

        void receiveMessage(Radio::Message& msg);

        
    protected:
        uint8_t identity;
        SPIClass* spi;
        uint8_t num_radios_online = 1;

        bool sendMessage(Radio::Message msg, bool multicast = false);

        template<typename T>
        bool sendMessage(T msgi, bool multicast = false) {
            return this->sendMessage(Radio::Message{msgi}, multicast);
        }

        void preInit(rf24_pa_dbm_e pa_level);

        
};


const uint8_t MAX_TX_BUFFER = 5;
class CustomRF24_Robot : public CustomRF24 {
    public:
        CustomRF24_Robot();
        
        bool init(uint8_t robot, uint8_t channel, rf24_pa_dbm_e pa_level = RF24_PA_MIN);

        // Register a configuration variable pointer
        template<typename T>
        void registerVariable(T *ptr, HG::Variable var, Radio::Access access) {
            static_assert(sizeof(T) != 0);
            static_assert(sizeof(T) != 3);
            static_assert(sizeof(T) < 5);
            switch(sizeof(T)){
                case 1:
                    config_access_width[(uint8_t) var] = ACCESS_WIDTH{WIDTH::B8, access};
                    *((uint8_t*) &config_variables_defaults[(uint8_t) var]) = *((uint8_t*) ptr);
                    break;
                case 2:
                    config_access_width[(uint8_t) var] = ACCESS_WIDTH{WIDTH::B16, access};
                    *((uint16_t*) &config_variables_defaults[(uint8_t) var]) = *((uint16_t*) ptr);
                    break;
                case 4:
                    config_access_width[(uint8_t) var] = ACCESS_WIDTH{WIDTH::B32, access};
                    *((uint32_t*) &config_variables_defaults[(uint8_t) var]) = *((uint32_t*) ptr);
                    break;
            }
            config_variables[(uint8_t) var] = (uint32_t*) ptr;
            
        }

        // Add/overwrite something in tx buffer
        void writeTxBuffer(uint8_t index, Radio::Message msg);

        // Call in loop, handles all communications
        bool run();

        // Register message specific callbacks
        template<typename T>
        void registerCallback(void (*fun)(T));

    private:
        
        enum class WIDTH : uint8_t {
            B8,
            B16,
            B32,
        };
        struct ACCESS_WIDTH {
            WIDTH width : 4;
            Radio::Access access : 4;
        };


        // Outgoing buffer (r -> b)
        Radio::Message txBuffer[MAX_TX_BUFFER];
        uint8_t tx_buffer_len;
        uint8_t tx_rotate;

        // Outgoing queue (r -> b)
        std::queue<Radio::Message> txQueue;

        // Receive all messages and trigger callbacks
        bool receiveAndCallback();

        // Write outgoing messages to ack packets (r -> b)
        void writeTx();

        // Configuration variable handling (b -> r)
        void handleMultiConfigMessage(Radio::MultiConfigMessage);
        uint32_t* config_variables[256];    // Pointers to configuration variables
        uint32_t config_variables_defaults[256];
        ACCESS_WIDTH config_access_width[256];

        template<typename T>
        T* config_variables_ptr(HG::Variable var) {
            return (T*) this->config_variables[(uint8_t) var];
        }

        // Callbacks for different message types
        void (*callback_command)(Radio::Command) = nullptr;
        void (*callback_status_hf)(Radio::PrimaryStatusHF) = nullptr;
        void (*callback_status_lf)(Radio::PrimaryStatusLF) = nullptr;
        void (*callback_imu_readings)(Radio::ImuReadings) = nullptr;
        void (*callback_odo_reading)(Radio::OdometryReading) = nullptr;
        void (*callback_override_odo)(Radio::OverrideOdometry) = nullptr;
        void (*callback_gcommand)(Radio::GlobalCommand) = nullptr;

        void (*callback_msg)(Radio::Message) = nullptr;
};

class CustomRF24_Base : public CustomRF24 {
    public:
        CustomRF24_Base(uint8_t group);
        bool run();

        void setRadioID(uint8_t identity);

        bool init(rf24_pa_dbm_e pa_level = RF24_PA_MIN);
        void openPipes(uint8_t num_radios_online);
        void setRxRobot(Radio::SSL_ID rx_robot);
        void setRxBroadcast();

        void handleMultiConfigMessage(Radio::MultiConfigMessage);

        template<typename T>
        bool sendMessageToRobot(T msg, uint8_t rx_robot) {
            this->setRxRobot(rx_robot);
            return this->sendMessage(msg);
        }

        template<typename T>
        bool sendMessageBroadcast(T msg) {
            this->setRxBroadcast();
            return this->sendMessage(msg, true);
        }

        // Register message callback
        void registerCallback(void (*fun)(Radio::Message, Radio::SSL_ID));


    private:
        Radio::SSL_ID rx_robot = 0;
        Radio::SSL_ID getID(uint8_t pipe);
        uint8_t getPipe(Radio::SSL_ID id);

        bool receiveAndCallback(uint8_t id);

        void (*callback_msg)(Radio::Message, Radio::SSL_ID) = nullptr;

};
