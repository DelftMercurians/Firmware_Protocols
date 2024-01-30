// Delft Mercurians
// Thomas Hettasch
// 2023-09-09
// thomas.hettasch@gmail.com

// Defines many useful structures and messages that can be reused accross the codebase

#pragma once
#include <Arduino.h>

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
};

// Kicker status message
#define KICKER_SCALE_TEMP (100.0/INT8_MAX)
#define KICKER_SCALE_VCAP (260.0/UINT8_MAX)
struct KickerStatus {
    HG::Status status;
    int8_t temp;
    uint8_t capv;
};

}