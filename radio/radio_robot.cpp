#include "radio.h"
#include "radio/protocols_radio.h"

// --------------ROBOT------------------ //

CustomRF24_Robot::CustomRF24_Robot()
    : CustomRF24(RadioPins::RobotPinMap.ce, RadioPins::RobotPinMap.cs),
        tx_buffer_len{0}
{
    if (RadioPins::RobotPinMap.spi_bus == RadioPins::SpiBus::Spi_1) {
        this->spi = new SPIClass(PA7, PA6, PA5);
    } else if (RadioPins::RobotPinMap.spi_bus == RadioPins::SpiBus::Spi_2) {
        this->spi = new SPIClass(PB15, PB14, PB13);
    }
}


bool CustomRF24_Robot::init(uint8_t robot, uint8_t channel, rf24_pa_dbm_e pa_level) {
    this->identity = robot;
    this->preInit(pa_level);
    this->openReadingPipe(1, Radio::BaseAddress_BtR + (uint64_t) identity);   // Listen on base to robot address
    this->openReadingPipe(2, Radio::BroadcastAddress);   // Listen on broadcast address
    this->setAutoAck(2, false); // Disable auto-ack, so it can receive multicast
    this->setChannel(channel);
    this->openWritingPipe(Radio::BaseAddress_RtB + (uint64_t) identity);      // Transmit on robot to base address
    this->startListening();           // Always idle in receiving mode
    return this->isChipConnected();
}

void CustomRF24_Robot::handleMultiConfigMessage(Radio::MultiConfigMessage mcm) {
    switch(mcm.operation) {
        case HG::ConfigOperation::READ:
            // Send back variable value
            {
                for(uint8_t i = 0; i < 5; i++) {
                    if(mcm.vars[i] == HG::Variable::NONE) continue;
                    if(this->config_variables[(uint8_t) mcm.vars[i]] == nullptr) {
                        mcm.vars[i] = HG::Variable::NONE; // Variable is not available
                        continue;
                    }
                    switch(this->config_access_width[(uint8_t) mcm.vars[i]].access) {
                        case Radio::Access::NONE:
                        case Radio::Access::WRITE:
                            mcm.vars[i] = HG::Variable::NONE; // Variable is not readable
                            continue;
                        case Radio::Access::READ:
                        case Radio::Access::READWRITE:
                            break;
                    }
                    mcm.values[i] = 0;
                    switch(this->config_access_width[(uint8_t) mcm.vars[i]].width) {
                        case WIDTH::B8:
                            *((uint8_t*) &mcm.values[i]) = *config_variables_ptr<uint8_t>(mcm.vars[i]);
                            break;
                        case WIDTH::B16:
                            *((uint16_t*) &mcm.values[i]) = *config_variables_ptr<uint16_t>(mcm.vars[i]);
                            break;
                        case WIDTH::B32:
                            *((uint32_t*) &mcm.values[i]) = *config_variables_ptr<uint32_t>(mcm.vars[i]);
                            break;
                    }
                }
                mcm.operation = HG::ConfigOperation::READ_RETURN;
                txQueue.push(Radio::Message{mcm});
            }
            break;
        case HG::ConfigOperation::WRITE:
            // Overwrite config value
            {
                for(uint8_t i = 0; i < 5; i++) {
                    if(mcm.vars[i] == HG::Variable::NONE) continue;
                    if(this->config_variables[(uint8_t) mcm.vars[i]] == nullptr){
                        mcm.vars[i] = HG::Variable::NONE; // Variable is not available
                        continue;
                    };
                    switch(this->config_access_width[(uint8_t) mcm.vars[i]].access) {
                        case Radio::Access::NONE:
                        case Radio::Access::READ:
                            mcm.vars[i] = HG::Variable::NONE; // Variable is not writeable
                            continue;
                        case Radio::Access::WRITE:
                        case Radio::Access::READWRITE:
                            break;
                    }
                    switch(this->config_access_width[(uint8_t) mcm.vars[i]].width) {
                        case WIDTH::B8:
                            *config_variables_ptr<uint8_t>(mcm.vars[i]) = *((uint8_t*) &mcm.values[i]);
                            mcm.values[i] = 0;
                            *((uint8_t*) &mcm.values[i]) = *config_variables_ptr<uint8_t>(mcm.vars[i]);
                            break;
                        case WIDTH::B16:
                            *config_variables_ptr<uint16_t>(mcm.vars[i]) = *((uint16_t*) &mcm.values[i]);
                            mcm.values[i] = 0;
                            *((uint16_t*) &mcm.values[i]) = *config_variables_ptr<uint16_t>(mcm.vars[i]);
                            break;
                        case WIDTH::B32:
                            *config_variables_ptr<uint32_t>(mcm.vars[i]) = *((uint32_t*) &mcm.values[i]);
                            mcm.values[i] = 0;
                            *((uint32_t*) &mcm.values[i]) = *config_variables_ptr<uint32_t>(mcm.vars[i]);
                            break;
                    }
                }
                mcm.operation = HG::ConfigOperation::WRITE_RETURN;
                txQueue.push(Radio::Message{mcm});
            }
            break;
        case HG::ConfigOperation::SET_DEFAULT:
            // Set a variable to its default value
            {
                for(uint8_t i = 0; i < 5; i++) {
                    if(mcm.vars[i] == HG::Variable::NONE) continue;
                    if(this->config_variables[(uint8_t) mcm.vars[i]] == nullptr){
                        mcm.vars[i] = HG::Variable::NONE; // Variable is not available
                        continue;
                    };
                    switch(this->config_access_width[(uint8_t) mcm.vars[i]].access) {
                        case Radio::Access::NONE:
                        case Radio::Access::READ:
                            mcm.vars[i] = HG::Variable::NONE; // Variable is not writeable
                            continue;
                        case Radio::Access::WRITE:
                        case Radio::Access::READWRITE:
                            break;
                    }
                    mcm.values[i] = 0;
                    switch(this->config_access_width[(uint8_t) mcm.vars[i]].width) {
                        case WIDTH::B8:
                            *config_variables_ptr<uint8_t>(mcm.vars[i]) = *((uint8_t*) &config_variables_defaults[(uint8_t) mcm.vars[i]]);
                            *((uint8_t*) &mcm.values[i]) = *config_variables_ptr<uint8_t>(mcm.vars[i]);
                            break;
                        case WIDTH::B16:
                            *config_variables_ptr<uint16_t>(mcm.vars[i]) = *((uint16_t*) &config_variables_defaults[(uint8_t) mcm.vars[i]]);
                            *((uint16_t*) &mcm.values[i]) = *config_variables_ptr<uint16_t>(mcm.vars[i]);
                            break;
                        case WIDTH::B32:
                            *config_variables_ptr<uint32_t>(mcm.vars[i]) = *((uint32_t*) &config_variables_defaults[(uint8_t) mcm.vars[i]]);
                            *((uint32_t*) &mcm.values[i]) = *config_variables_ptr<uint32_t>(mcm.vars[i]);
                            break;
                    }
                }
                mcm.operation = HG::ConfigOperation::SET_DEFAULT_RETURN;
                txQueue.push(Radio::Message{mcm});
            }
            break;
        default:
            return;
    }
}

void CustomRF24_Robot::writeTxBuffer(uint8_t index, Radio::Message msg) {
    if(index >= MAX_TX_BUFFER) return;
    if(index >= tx_buffer_len) tx_buffer_len = index + 1;
    this->txBuffer[index] = msg;
    writeTx();
}

bool CustomRF24_Robot::run() {
    // Receive
    uint8_t pipe = 0;
    if(!this->available(&pipe)){
        // No message received
        return false;
    }
    auto ret = receiveAndCallback();

    // Transmit
    writeTx();

    return ret;
}


void CustomRF24_Robot::writeTx() {
    // Fill TX buffer
    // this->flush_tx();
    // if(tx_buffer_len >= 1) {
    if(this->txQueue.empty()) {
        this->writeAckPayload(1, &txBuffer[tx_rotate%tx_buffer_len], sizeof(txBuffer[0]));
        tx_rotate++;
    } else {
        this->writeAckPayload(1, &this->txQueue.front(), sizeof(txBuffer[0]));
        this->txQueue.pop();
    }
	//     this->writeAckPayload(1, &txBuffer[(tx_rotate + 1)%tx_buffer_len], sizeof(txBuffer[0]));
	//     this->writeAckPayload(1, &txBuffer[(tx_rotate + 2)%tx_buffer_len], sizeof(txBuffer[0]));
    // }
}

bool CustomRF24_Robot::receiveAndCallback() {
    Radio::Message msg;
    auto size = getDynamicPayloadSize();
    receiveMessage(msg);

    if(callback_msg != nullptr){
        callback_msg(msg);
    }
    // TODO: can be replaced by a template
    switch(msg.mt) {
        case Radio::MessageType::MultiConfigMessage:
            // handle incoming multi config message
            handleMultiConfigMessage(msg.msg.mcm);
            return true;
        case Radio::MessageType::Command:
            if(callback_command != nullptr){
                callback_command(msg.msg.c);
            }
            return true;
        case Radio::MessageType::PrimaryStatusHF:
            if(callback_status_hf != nullptr){
                callback_status_hf(msg.msg.ps_hf);
            }
            return true;
        case Radio::MessageType::PrimaryStatusLF:
            if(callback_status_lf != nullptr){
                callback_status_lf(msg.msg.ps_lf);
            }
            return true;
        case Radio::MessageType::ImuReadings:
            if(callback_imu_readings != nullptr){
                callback_imu_readings(msg.msg.ir);
            }
            return true;
        case Radio::MessageType::OdometryReading:
            if(callback_odo_reading != nullptr){
                callback_odo_reading(msg.msg.odo);
            }
            return true;
        case Radio::MessageType::OverrideOdometry:
            if(callback_override_odo != nullptr){
                callback_override_odo(msg.msg.over_odo);
            }
            return true;
        case Radio::MessageType::None:
            // No message received
            // Serial.printf(" <%u> NONE\n", id);
            break;
        default:
            //Unknown message type
            // Serial.println(" Unknown message type received!");
            // uint8_t *data = (uint8_t*) &msg;
            // for(uint8_t i = 0; i < 32; i++) {
            //     Serial.printf(" 0x%02X", data[i]);
            // }
            // Serial.println();
            return false;
    }
    return false;
}



template<>
void CustomRF24_Robot::registerCallback<Radio::Command>(void (*fun)(Radio::Command)) {
    callback_command = fun;
}

template<>
void CustomRF24_Robot::registerCallback<Radio::Message>(void (*fun)(Radio::Message)) {
    callback_msg = fun;
}

template<>
void CustomRF24_Robot::registerCallback<Radio::PrimaryStatusHF>(void (*fun)(Radio::PrimaryStatusHF)) {
    callback_status_hf = fun;
}

template<>
void CustomRF24_Robot::registerCallback<Radio::PrimaryStatusLF>(void (*fun)(Radio::PrimaryStatusLF)) {
    callback_status_lf = fun;
}

template<>
void CustomRF24_Robot::registerCallback<Radio::ImuReadings>(void (*fun)(Radio::ImuReadings)) {
    callback_imu_readings = fun;
}

template<>
void CustomRF24_Robot::registerCallback<Radio::OdometryReading>(void (*fun)(Radio::OdometryReading)) {
    callback_odo_reading = fun;
}

template<>
void CustomRF24_Robot::registerCallback<Radio::OverrideOdometry>(void (*fun)(Radio::OverrideOdometry)) {
    callback_override_odo = fun;
}


