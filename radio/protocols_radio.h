// Delft Mercurians
// Thomas Hettasch
// 2023-05-18
// thomas.hettasch@gmail.com

#pragma once
#include "../utils.h"
#include "../can/protocols_can.h"

#define BASESTATION Radio::Device::BaseStation
#define ROBOT_0 Radio::Device::Robot_0
#define ROBOT_1 Radio::Device::Robot_1
#define ROBOT_2 Radio::Device::Robot_2
#define ROBOT_3 Radio::Device::Robot_3
#define ROBOT_4 Radio::Device::Robot_4

namespace Radio {

// Due to limitations of the radio, there can only be 5 robots for now
enum class Device {
    BaseStation = 0,
    Robot_0 = 1,
    Robot_1 = 2,
    Robot_2 = 3,
    Robot_3 = 4,
    Robot_4 = 5,
};

const uint64_t BaseAddress_BtR = 0x114867LL;    // Address base to robot
const uint64_t BaseAddress_RtB = 0x124867LL;    // Address robot to bases

/* CONFIG MESSAGES */
// Configuration Message Types
enum class ConfigMessageType {
    NONE,                   // Don't do anything with this
    SET_DEFAULT,            // Set a parameter to defaults
    SET_DEFAULT_RETURN,     // Reply stating value set to defaults
    READ = (int) CAN::ACCESS::READ,                   // Read a parameter from the robot
    READ_RETURN = (int) CAN::ACCESS::MASK,            // Robot return message with value
    WRITE = (int) CAN::ACCESS::WRITE,                  // Write a parameter to the robot
    WRITE_RETURN,           // Robot return message stating new value
};

// Configuration message (bidirectional)
struct ConfigMessage {
    ConfigMessageType mt;       // Message type
    CAN::VARIABLE var;          // Variable/Parameter that is being accessed
    CAN_VARIABLE_TYPE value;    // Value to be written/that is being acknowledged
};


/* COMMAND MESSAGES */
// Kicker subcommands
enum class KickerCommand : uint8_t {
    NONE,

    ARM,        // Arm the high voltage circuitry
    DISARM,     // Disarm the high voltage circuitry

    DISCHARGE,  // Discharge the capacitor

    KICK,       // Kick the ball
    CHIP,       // Chip the ball
};

// Command from mothership to robot
struct Command {
    HG::Pose speed;                 // Desired robot speed

    float dribbler_speed;           // Desired dribbler speed

    KickerCommand kicker_command;   // Command for the kicker
    float kick_time;                // How long to kick for (if kick is requested)

    float fan_speed;                // Downforce fan speed (percentage)
};


/* REPLY MESSAGES */
// Reply from robot to mothership
struct Reply {
    HG::Status status;          // Robot MCU status
    HG::Status md_status[5];    // Motor driver MCU statuses
    HG::Status kick_status;     // Kicker status
    HG::Status fan_status;      // Fan status
    HG::Pose speed;             // Measured speed
};

/* STATUS MESSAGES */
// Repeated status heartbeat from robot
struct Status {
    HG::Status primary_status;
    CAN::MotorStatus motor_status[5];
    HG::KickerStatus kicker_status;
    HG::Status fan_status;
};


// A list of all possible message types transmitted over radio
enum class MessageType : uint8_t {
    None,               // No message received
    Command,            // A command message
    Reply,              // A reply from the robot
    ConfigMessage,      // A configuration message
    Status,             // A hearbeat status message
};

// A structure that can hold messages of any type
struct Message {
    MessageType mt;             // The message type
    union {
        Command c;
        Reply r;
        ConfigMessage cm;
        Status s;
    } msg;                      // The message contents
};

static_assert(sizeof(Message) <= 32, "Message exceeds maximum size");

}
