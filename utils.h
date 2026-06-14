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
    EMERGENCY = 0x00,      // Robot is in estop because something is very wrong
    OK = 0x01,             // Everything is running as it should
    STOP = 0x02,           // The robot is stopped, but can be started again easily
    STARTING = 0x03,       // The MCU is performing a startup procedure
    OVERTEMP = 0x04,       // For when something has overheated

    NO_REPLY = 0x05,       // No reply was received. This is not explicitly sent, but defaulted to.

    ARMED = 0x06,          // Kicker is armed, capacitor will be automatically recharged
    DISARMED = 0x07,       // Kicker is disarmed, but capacitor may still be on
    SAFE = 0x08,           // Kicker is discharged and safe to handle

    NOT_INSTALLED = 0x09,  // This submodule is not installed/implemented

    STANDBY = 0x0A,        // Submodule is inactive, but ready to go

    READY = 0x0B,          // Kicker is charged and ready to fire
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
enum class [[deprecated]] PackEnable : uint8_t {
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
    NONE = 0x00,

    ROBOT_ID = 0x01,

    MODE = 0x02,

    UID_0 = 0x03,
    UID_1 = 0x04,
    UID_2 = 0x05,

    DIPSWITCHES = 0x06,

    FW_VERSION_MAJOR = 0x07,
    FW_VERSION_MINOR = 0x08,
    FW_VERSION_PATCH = 0x09,

    FW_PROTOCOL_VERSION_MAJOR = 0x0A,
    FW_PROTOCOL_VERSION_MINOR = 0x0B,

    STATUS_PRIMARY = 0x0C,
    STATUS_KICKER = 0x0D,
    STATUS_IMU = 0x0E,
    STATUS_FAN [[deprecated]] = 0x0F,
    STATUS_POWER_BOARD [[deprecated]] = 0x10,

    PACK_L_ENABLE [[deprecated]] = 0x11,
    PACK_R_ENABLE [[deprecated]] = 0x12,

    PACK_L_VOLTAGE = 0x13,
    PACK_R_VOLTAGE = 0x14,

    DUCT_PRESSURE [[deprecated]] = 0x15,

    BREAKBEAM_SENSOR_OK = 0x16,
    BREAKBEAM_BALL_DETECTED = 0x17,

    KICKER_VOLTAGE = 0x18,
    KICKER_TEMPERATURE = 0x19,
    KICKER_MODE = 0x1A,

    MOTION_POS_X = 0x1B,
    MOTION_POS_Y = 0x1C,
    MOTION_ANG_Z = 0x1D,

    MOTION_VEL_X = 0x1E,
    MOTION_VEL_Y = 0x1F,
    MOTION_ANG_VEL_Z = 0x20,
    MOTION_VEL_NULL = 0x21,

    MOTION_ACC_X = 0x22,
    MOTION_ACC_Y = 0x23,
    MOTION_ANG_ACC_Z = 0x24,

    RADIO_CHANNEL = 0x25,

};

} // namespace HG



namespace CAN {
// Various device IDs (0x00 -> 0x07, 3 bits)
#define CAN_NUM_DEVICE_IDS (1 << 3)
enum class DEVICE_ID : uint8_t {
    BROADCAST = 0x0,   // Send message to all devices (highest priority)

    PRIMARY = 0x6,     // Main device on bus, coordinates all other devices

    AUXILIARY = 0x7, // Power board and solenoid driver boards (shared ID)

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
