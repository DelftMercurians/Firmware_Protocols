#include "radio.h"
#include "radio/protocols_radio.h"

// Initialise Radio
void CustomRF24::preInit(rf24_pa_dbm_e pa_level) {
	this->begin(this->spi);
    Serial.printf("Initialised Radio, IDENTITY = 0x%X\n", this->identity);
	Serial.printf("Failure detected: %s\n", this->failureDetected ? "true" : "false");
	Serial.printf("isChipConnected: %s\n", this->isChipConnected() ? "true" : "false");
	Serial.printf("isPVariant: %s\n", this->isPVariant() ? "true" : "false");
    // this->setPayloadSize(sizeof(Radio::Message));
	this->setPALevel(pa_level);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
    this->enableDynamicPayloads();
    this->enableAckPayload();
}

void CustomRF24::postInit() {
    // this->startListening();           // Always idle in receiving mode
    this->stopListening();
}


// Send a generic message
bool CustomRF24::sendMessage(Radio::Message msg) {
    // Serial.print("MSG = ");
    // for(uint8_t i = 0; i < sizeof(msg); i++) {
    //     Serial.printf("%02X ", ((uint8_t*) &msg)[i]);
    // }
    // Serial.println();
    // this->stopListening();
    return this->write(&msg, sizeof(msg));
    // if(!res) {
    //     Serial.print("F\n");
    // }
    // this->startListening();
}

// Receive a generic message
void CustomRF24::receiveMessage(Radio::Message& msg) {
    this->read(&msg, sizeof(msg));
}

void CustomRF24::registerVariable(uint32_t *ptr, HG::Variable var) {
    config_variables[(uint8_t) var] = ptr;
}

template<>
void CustomRF24::registerCallback<Radio::Command>(void (*fun)(Radio::Command, Radio::SSL_ID)) {
    callback_command = fun;
}

template<>
void CustomRF24::registerCallback<Radio::Message>(void (*fun)(Radio::Message, Radio::SSL_ID)) {
    callback_msg = fun;
}

template<>
void CustomRF24::registerCallback<Radio::PrimaryStatusHF>(void (*fun)(Radio::PrimaryStatusHF, uint8_t)) {
    callback_status_hf = fun;
}

template<>
void CustomRF24::registerCallback<Radio::PrimaryStatusLF>(void (*fun)(Radio::PrimaryStatusLF, uint8_t)) {
    callback_status_lf = fun;
}

template<>
void CustomRF24::registerCallback<Radio::ImuReadings>(void (*fun)(Radio::ImuReadings, uint8_t)) {
    callback_imu_readings = fun;
}

template<>
void CustomRF24::registerCallback<Radio::OdometryReading>(void (*fun)(Radio::OdometryReading, uint8_t)) {
    callback_odo_reading = fun;
}

template<>
void CustomRF24::registerCallback<Radio::OverrideOdometry>(void (*fun)(Radio::OverrideOdometry, uint8_t)) {
    callback_override_odo = fun;
}

void CustomRF24_Robot::handleMultiConfigMessage(Radio::MultiConfigMessage mcm) {
    switch(mcm.operation) {
        case HG::ConfigOperation::READ:
            // Send back variable value
            {
                Radio::MultiConfigMessage ret_message;
                ret_message.operation = HG::ConfigOperation::READ_RETURN;
                ret_message.type = mcm.type;
                for(uint8_t i = 0; i < 5; i++) {
                    ret_message.vars[i] = mcm.vars[i];
                    if(mcm.vars[i] == HG::Variable::NONE) continue;
                    if(this->config_variables[(uint8_t) mcm.vars[i]] == nullptr) {
                        ret_message.vars[i] = HG::Variable::NONE; // Variable is not available
                        continue;
                    }
                    ret_message.values[i] = *this->config_variables[(uint8_t) mcm.vars[i]];
                }
                txQueue.push(Radio::Message{ret_message});
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
                    *this->config_variables[(uint8_t) mcm.vars[i]] = mcm.values[i];
                }
                mcm.operation = HG::ConfigOperation::WRITE_RETURN;
                txQueue.push(Radio::Message{mcm});
            }
            break;
        case HG::ConfigOperation::SET_DEFAULT:
            // Set a variable to its default value
            // TODO
            break;
        default:
            return;
    }
}

void CustomRF24_Base::handleMultiConfigMessage(Radio::MultiConfigMessage mcm) {
    return;
}

bool CustomRF24_Base::receiveAndCallback(uint8_t id) {
    Radio::Message msg;
    auto size = getDynamicPayloadSize();
    receiveMessage(msg);

    if(callback_msg != nullptr){
        callback_msg(msg, id);
    }
    // TODO: can be replaced by a template
    switch(msg.mt) {
        case Radio::MessageType::MultiConfigMessage:
            // handle incoming multi config message
            return true;
        case Radio::MessageType::Command:
            if(callback_command != nullptr){
                callback_command(msg.msg.c, id);
            }
            return true;
        case Radio::MessageType::PrimaryStatusHF:
            if(callback_status_hf != nullptr){
                callback_status_hf(msg.msg.ps_hf, id);
            }
            return true;
        case Radio::MessageType::PrimaryStatusLF:
            if(callback_status_lf != nullptr){
                callback_status_lf(msg.msg.ps_lf, id);
            }
            return true;
        case Radio::MessageType::ImuReadings:
            if(callback_imu_readings != nullptr){
                callback_imu_readings(msg.msg.ir, id);
            }
            return true;
        case Radio::MessageType::OdometryReading:
            if(callback_odo_reading != nullptr){
                callback_odo_reading(msg.msg.odo, id);
            }
            return true;
        case Radio::MessageType::OverrideOdometry:
            if(callback_override_odo != nullptr){
                callback_override_odo(msg.msg.over_odo, id);
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


bool CustomRF24_Robot::init(uint8_t robot, rf24_pa_dbm_e pa_level) {
    this->identity = robot;
    this->preInit(pa_level);
    this->openReadingPipe(1, Radio::BaseAddress_BtR + (uint64_t) identity);   // Listen on base to robot address
    this->openWritingPipe(Radio::BaseAddress_RtB + (uint64_t) identity);      // Transmit on robot to base address
    this->postInit();
    this->startListening();           // Always idle in receiving mode
    return this->isChipConnected();
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
    auto ret = receiveAndCallback(pipe);

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

bool CustomRF24_Robot::receiveAndCallback(uint8_t id) {
    Radio::Message msg;
    auto size = getDynamicPayloadSize();
    receiveMessage(msg);

    if(callback_msg != nullptr){
        callback_msg(msg, id);
    }
    // TODO: can be replaced by a template
    switch(msg.mt) {
        case Radio::MessageType::MultiConfigMessage:
            // handle incoming multi config message
            handleMultiConfigMessage(msg.msg.mcm);
            return true;
        case Radio::MessageType::Command:
            if(callback_command != nullptr){
                callback_command(msg.msg.c, id);
            }
            return true;
        case Radio::MessageType::PrimaryStatusHF:
            if(callback_status_hf != nullptr){
                callback_status_hf(msg.msg.ps_hf, id);
            }
            return true;
        case Radio::MessageType::PrimaryStatusLF:
            if(callback_status_lf != nullptr){
                callback_status_lf(msg.msg.ps_lf, id);
            }
            return true;
        case Radio::MessageType::ImuReadings:
            if(callback_imu_readings != nullptr){
                callback_imu_readings(msg.msg.ir, id);
            }
            return true;
        case Radio::MessageType::OdometryReading:
            if(callback_odo_reading != nullptr){
                callback_odo_reading(msg.msg.odo, id);
            }
            return true;
        case Radio::MessageType::OverrideOdometry:
            if(callback_override_odo != nullptr){
                callback_override_odo(msg.msg.over_odo, id);
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


// ---------------BASE------------------ //
CustomRF24_Base::CustomRF24_Base(uint8_t group)
    : CustomRF24(RadioPins::GroupPinMap[group].ce,  RadioPins::GroupPinMap[group].cs)
 {
    this->identity = group;
    RadioPins::RadioPins pins = RadioPins::GroupPinMap[this->identity];
    if (pins.spi_bus == RadioPins::SpiBus::Spi_1) {
        this->spi = new SPIClass(PA7, PA6, PA5);
    } else if (pins.spi_bus == RadioPins::SpiBus::Spi_2) {
        this->spi = new SPIClass(PB15, PB14, PB13);
    }
}

void CustomRF24_Base::setRadioID(uint8_t identity) {
    this->identity = identity;
}

bool CustomRF24_Base::init(rf24_pa_dbm_e pa_level) {
    this->preInit(pa_level);
    return this->isChipConnected();
}

bool CustomRF24_Base::run() {
    uint8_t pipe = 0;
    if(!this->available(&pipe)){
        // No message received
        return false;
    }
    if(pipe == 0) {
        return receiveAndCallback(this->rx_robot);  // Received on basestation backlistening pipe
    } else {
        return receiveAndCallback(Radio::getRobotID(pipe, identity, num_radios_online));
    }
}

void CustomRF24_Base::openPipes(uint8_t num_radios_online) {
    this->num_radios_online = num_radios_online;
    this->openWritingPipe(Radio::BaseAddress_BtR + (uint64_t) this->rx_robot);
    // Open all five reading pipes (one for each robot)
    for (uint8_t pipe = 1; pipe <= 5; pipe++) {
        this->openReadingPipe(pipe, Radio::BaseAddress_RtB + getID(pipe));
    }
    this->postInit();
}


void CustomRF24_Base::setRxRobot(Radio::SSL_ID rx_robot) {
    if(rx_robot != this->rx_robot) {
        this->rx_robot = rx_robot;
        // this->openReadingPipe(0, Radio::BaseAddress_BtR + (uint64_t) this->rx_robot); // For listening to other base stations
        this->openWritingPipe(Radio::BaseAddress_BtR + (uint64_t) this->rx_robot);
    }
}

// Get robot ssl id based on pipe of current radio
Radio::SSL_ID CustomRF24_Base::getID(uint8_t pipe) {
    if(pipe < 1 || pipe > 5) return 255;            // Invalid pipe number
    if(identity >= num_radios_online) return 255;   // Invalid identity, given number of online radios
    return Radio::getRobotID(pipe, identity, num_radios_online);
}
// Get pipe for specific robot id
uint8_t CustomRF24_Base::getPipe(Radio::SSL_ID id) {
    if(Radio::getRadioID(id, num_radios_online) != identity) return 0;    // ID is not assigned to this radio
    return Radio::getPipe(id, num_radios_online);
}
