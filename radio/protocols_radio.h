// Delft Mercurians
// Thomas Hettasch
// 2023-05-18
// thomas.hettasch@gmail.com

#pragma once
namespace Radio {

// Statuses that the robot or motor controller MCU's could have
enum class Status {
    EMERGENCY,      // Robot is in estop because something is very wrong
    OK,             // Everything is running as it should
    STOP,           // The robot is stopped, but can be started again easily
    STARTING,       // The MCU is performing a startup procedure
    NO_REPLY,       // No reply was received. This is not explicitly sent, but defaulted to.
};

// A 2D robot pose. XY are linear, Z is angular
struct Pose {
    float x;
    float y;
    float z;
};

// Configuration Message Types
enum class ConfigMessageType {
    NONE,                   // Don't do anything with this
    SET_DEFAULT,            // Set a parameter to defaults
    SET_DEFAULT_RETURN,     // Reply stating value set to defaults
    READ,                   // Read a parameter from the robot
    READ_RETURN,            // Robot return message with value
    WRITE,                  // Write a parameter to the robot
    WRITE_RETURN,           // Robot return message stating new value
};
   
// Onboard robot configuration variables that can be changed
enum class ConfigVariabe {
    PID_CD_P,
    // ADD MORE HERE
};

// Configuration message (bidirectional)
struct ConfigMessage {
    ConfigMessageType mt;   // Message type
    ConfigVariabe var;      // Variable/Parameter that is being accessed
    float value;            // Value to be written/that is being acknowledged
};

// Command from mothership to robot
struct Command {
    Pose speed;     // Desired robot speed
};

// Reply from robot to mothership
struct Reply {
    Status status;          // Robot MCU status
    Status md_status[4];    // Motor driver MCU statuses
    Pose speed;             // Measured speed
    Pose position;          // Measured position
};

}
