// Delft Mercurians
// Thomas Hettasch
// 2023-05-18
// thomas.hettasch@gmail.com

#pragma once
#include "../utils.h"
#include "../can/protocols_can.h"

namespace Radio {

enum class Device {
    BaseStation = 0,
    Robot_0 = 1,
    Robot_1 = 2,
    Robot_2 = 3,
    Robot_3 = 4,
    Robot_4 = 5,
};

const uint64_t DefaultAddress = 0xF0F0F0F0E0LL;

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
// Command from mothership to robot
struct Command {
    HG::Pose speed;     // Desired robot speed
};

/* REPLY MESSAGES */
// Reply from robot to mothership
struct Reply {
    HG::Status status;          // Robot MCU status
    HG::Status md_status[4];    // Motor driver MCU statuses
    HG::Pose speed;             // Measured speed
};


// A list of all possible message types transmitted over radio
enum class MessageType : uint8_t {
    None,
    Command,
    Reply,
    ConfigMessage,
};

// A structure that can hold messages of any type
struct Message {
    MessageType mt;
    union {
        Command c;
        Reply r;
        ConfigMessage cm;
    } msg;
};


}
