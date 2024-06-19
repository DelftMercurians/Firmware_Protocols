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

    STANDBY,        // Submodule is inactive, but ready to go
};

// Kicker status message
const float KICKER_SCALE_TEMP = (100.0/INT8_MAX);
const float KICKER_SCALE_VCAP = (260.0/UINT8_MAX);
struct KickerStatus {
    HG::Status status;
    uint8_t temp;
    uint8_t capv;
};

// Battery pack enable/disable status
enum class PackEnable : uint8_t {
    BOTH = 0b11,       // Both battery packs enabled
    LEFT_ONLY = 0b10,  // Only left battery pack enabled
    RIGHT_ONLY = 0b01, // Only right battery pack enabled
    OFF = 0b00         // Neither battery pack enabled, robot will shut down
};

struct Version {
    uint8_t major;
    uint8_t minor;
    uint16_t patch;

    uint8_t protocols_major;
    uint8_t protocols_minor;
};

// Configuration operations (4 bits)
enum class ConfigOperation : uint8_t {
    NONE,                   // Don't do anything with this
    SET_DEFAULT,            // Set a parameter to defaults
    SET_DEFAULT_RETURN,     // Reply stating value set to defaults
    READ,                   // Read a parameter from the robot
    READ_RETURN,            // Robot return message with value
    WRITE,                  // Write a parameter to the robot
    WRITE_RETURN,           // Robot return message stating new value
};

// Configuration variable types (4 bits)
enum class VariableType : uint8_t {
    VOID,

    U32,
    U16,
    U8,

    F32,

    I32,
    I16,
    I8,
};

// Robot Variables (8 bits)
enum class Variable : uint8_t {
    NONE,

    ROBOT_ID,

    MODE,

    UID_0,
    UID_1,
    UID_2,

    DIPSWITCHES,

    FW_VERSION_MAJOR,
    FW_VERSION_MINOR,
    FW_VERSION_PATCH,

    FW_PROTOCOL_VERSION_MAJOR,
    FW_PROTOCOL_VERSION_MINOR,

    STATUS_PRIMARY,
    STATUS_KICKER,
    STATUS_IMU,
    STATUS_FAN,
    STATUS_POWER_BOARD,

    PACK_L_ENABLE,
    PACK_R_ENABLE,

    PACK_L_VOLTAGE,
    PACK_R_VOLTAGE,

    DUCT_PRESSURE,

    BREAKBEAM_SENSOR_OK,
    BREAKBEAM_BALL_DETECTED,

    KICKER_VOLTAGE,
    KICKER_TEMPERATURE,
    KICKER_MODE,

    MOTION_POS_X,
    MOTION_POS_Y,
    MOTION_ANG_Z,

    MOTION_VEL_X,
    MOTION_VEL_Y,
    MOTION_ANG_VEL_Z,
    MOTION_VEL_NULL,

    MOTION_ACC_X,
    MOTION_ACC_Y,
    MOTION_ANG_ACC_Z,


};

} // namespace HG



namespace CAN {
// Various device IDs (0x00 -> 0x07, 3 bits)
#define CAN_NUM_DEVICE_IDS (1 << 3)
enum class DEVICE_ID : uint8_t {
    BROADCAST = 0x0,   // Send message to all devices (highest priority)

    PRIMARY = 0x6,     // Main device on bus, coordinates all other devices

    POWER_BOARD = 0x7, // Power board, responsible for system on/off and fan

    DRIVER_0 = 0x1,    // Motor driver 0 (Wheel motor)
    DRIVER_1 = 0x2,    // Motor driver 1 (Wheel motor)
    DRIVER_2 = 0x3,    // Motor driver 2 (wheel motor)
    DRIVER_3 = 0x4,    // Motor driver 3 (wheel motor)

    DRIVER_A = 0x5,    // Motor driver A (auxiliary motor)
};

} // namespace CAN



namespace Motor {
// Motor Mode
enum class Mode : uint32_t{
    DISABLE = 0,    // Switch the motor off
    ENABLE = 1,     // Switch the motor on
    COAST = 2,      // Switch the motor off, but renable automaticaly when a new value is sent
    // TODO maybe add different modes for high power/low power etc.
};

} // namespace Motor
