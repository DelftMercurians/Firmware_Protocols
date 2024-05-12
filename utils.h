// Delft Mercurians
// Thomas Hettasch
// 2023-09-09
// thomas.hettasch@gmail.com

// Defines many useful structures and messages that can be reused accross the codebase

#pragma once
#include <stdint.h>

namespace HG {

// A 2D robot pose. XY are linear, Z is angular
struct Pose {
    float x;
    float y;
    float z;
};

// Statuses that the robot or motor controller MCU's could have
enum class Status : uint8_t {
    EMERGENCY,      // Robot is in estop because something is very wrong
    OK,             // Everything is running as it should
    STOP,           // The robot is stopped, but can be started again easily
    STARTING,       // The MCU is performing a startup procedure
    OVERTEMP,       // For when something has overheated

    NO_REPLY,       // No reply was received. This is not explicitly sent, but defaulted to.

    ARMED,          // Kicker is armed, capacitor will be automatically recharged
    DISARMED,       // Kicker is disarmed, but capacitor may still be on
    SAFE,           // Kicker is discharged and safe to handle

    NOT_INSTALLED,  // This submodule is not installed/implemented
};

// Kicker status message
#define KICKER_SCALE_TEMP (100.0/INT8_MAX)
#define KICKER_SCALE_VCAP (260.0/UINT8_MAX)
struct KickerStatus {
    HG::Status status;
    uint8_t temp;
    uint8_t capv;
};

enum class SUBSYSTEM : uint8_t {

    MAIN_BOARD = 0x1,   // Main device on bus, coordinates all other devices (high priority messages)

    KICKER = 0x2,    // Solenoid Driver board
    POWER_BOARD = 0x3,    // Power board

    TRACTION_MOTOR_0 = 0x4,    // Motor driver 0 (Wheel motor)
    TRACTION_MOTOR_1 = 0x5,    // Motor driver 1 (Wheel motor)
    TRACTION_MOTOR_2 = 0x6,    // Motor driver 2 (wheel motor)
    TRACTION_MOTOR_3 = 0x7,    // Motor driver 3 (wheel motor)

    DRIBBLER = 0x8,    // Motor driver (dribbler)

};

}

namespace CAN {
// Various device IDs (0x00 -> 0x07, 3 bits)
const size_t NUM_DEVICE_IDS = 1 << 3;
enum class DEVICE_ID : uint8_t {
    PRIMARY = 0x7,     // Main device on bus, coordinates all other devices (high priority messages)

    ALL = 0x7,         // Send message to all devices (highest priority)
    ANY = 0x0,         // Send message to all devices (lowest priority messages)

    DRIVER_0 = 0x1,    // Motor driver 0 (Wheel motor)
    DRIVER_1 = 0x2,    // Motor driver 1 (Wheel motor)
    DRIVER_2 = 0x3,    // Motor driver 2 (wheel motor)
    DRIVER_3 = 0x4,    // Motor driver 3 (wheel motor)

    DRIVER_A = 0x5,    // Motor driver A (auxiliary motor)
};
}