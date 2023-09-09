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
};


}