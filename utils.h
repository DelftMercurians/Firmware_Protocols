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

    COOLDOWN = 0x0C,       // Kicker has kicked too many times in a row

    ARMED_REFLEX = 0x0D,     // Kicker is armed in reflex mode, will kick when ball is detected
    COOLDOWN_REFLEX = 0x0E,  // Kicker is in cooldown for reflex kicks but can still kick normally
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
    RADIO_CHANNEL_ALT = 0x26,
    RADIO_CHANNEL_DEBUG = 0x27,

    LOG_LEVEL_SERIAL = 0x28,
    LOG_LEVEL_RADIO = 0x29,
    LOG_LEVEL_SD = 0x2A,

    THRESHOLD_BREAKBEAM = 0x2B,

    THRESHOLD_BATTERY_LOW = 0x2C,
    THRESHOLD_BATTERY_CRITICAL = 0x2D,

    TRIGGER_SAVE = 0x31,

    SAS_Kp_yaw = 0x40,
    SAS_Kd_yaw = 0x41,
    SAS_max_yaw_speed = 0x42,
    SAS_max_yaw_accel = 0x43,
    SAS_max_lin_speed = 0x44,
    SAS_max_lin_accel = 0x45,
    SAS_gain_scheduling_threshold = 0x46,
    SAS_gain_scheduling_multiplier = 0x47,
    SAS_RESERVED_1 = 0x48,
    SAS_RESERVED_2 = 0x49,
    SAS_RESERVED_3 = 0x4A,
    SAS_RESERVED_4 = 0x4B,
    SAS_RESERVED_5 = 0x4C,
    SAS_RESERVED_6 = 0x4D,
    SAS_RESERVED_7 = 0x4E,
    SAS_RESERVED_8 = 0x4F,

    SAS_Fallback_Kp_yaw = 0x50,
    SAS_Fallback_Kd_yaw = 0x51,
    SAS_Fallback_max_yaw_speed = 0x52,
    SAS_Fallback_max_yaw_accel = 0x53,
    SAS_Fallback_max_lin_speed = 0x54,
    SAS_Fallback_max_lin_accel = 0x55,
    SAS_Fallback_RESERVED_1 = 0x56,
    SAS_Fallback_RESERVED_2 = 0x57,
    SAS_Fallback_RESERVED_3 = 0x58,
    SAS_Fallback_RESERVED_4 = 0x59,
    SAS_Fallback_RESERVED_5 = 0x5A,
    SAS_Fallback_RESERVED_6 = 0x5B,
    SAS_Fallback_RESERVED_7 = 0x5C,
    SAS_Fallback_RESERVED_8 = 0x5D,
    SAS_Fallback_RESERVED_9 = 0x5E,
    SAS_Fallback_RESERVED_A = 0x5F,

    MD_TRACTION_LIM_C = 0x60,
    MD_TRACTION_LIM_U = 0x61,
    MD_TRACTION_LIM_V = 0x62,

    MD_TRACTION_PID_CD_P = 0x63,
    MD_TRACTION_PID_CD_I = 0x64,
    MD_TRACTION_PID_CD_D = 0x65,
    MD_TRACTION_PID_CD_R = 0x66,
    MD_TRACTION_PID_CD_L = 0x67,
    MD_TRACTION_PID_CD_F = 0x68,

    MD_TRACTION_PID_CQ_P = 0x69,
    MD_TRACTION_PID_CQ_I = 0x6A,
    MD_TRACTION_PID_CQ_D = 0x6B,
    MD_TRACTION_PID_CQ_R = 0x6C,
    MD_TRACTION_PID_CQ_L = 0x6D,
    MD_TRACTION_PID_CQ_F = 0x6E,

    MD_TRACTION_PID_V_P = 0x6F,
    MD_TRACTION_PID_V_I = 0x70,
    MD_TRACTION_PID_V_D = 0x71,
    MD_TRACTION_PID_V_R = 0x72,
    MD_TRACTION_PID_V_L = 0x73,
    MD_TRACTION_PID_V_F = 0x74,

    MD_TRACTION_PID_A_P = 0x75,
    MD_TRACTION_PID_A_I = 0x76,
    MD_TRACTION_PID_A_D = 0x77,
    MD_TRACTION_PID_A_R = 0x78,
    MD_TRACTION_PID_A_L = 0x79,
    MD_TRACTION_PID_A_F = 0x7A,

    MD_TRACTION_RESERVED_1 = 0x7B,
    MD_TRACTION_RESERVED_2 = 0x7C,
    MD_TRACTION_RESERVED_3 = 0x7D,
    MD_TRACTION_RESERVED_4 = 0x7E,
    MD_TRACTION_RESERVED_5 = 0x7F,

    MD_DRIBBLER_LIM_C = 0x80,
    MD_DRIBBLER_LIM_U = 0x81,
    MD_DRIBBLER_LIM_V = 0x82,

    MD_DRIBBLER_PID_CD_P = 0x83,
    MD_DRIBBLER_PID_CD_I = 0x84,
    MD_DRIBBLER_PID_CD_D = 0x85,
    MD_DRIBBLER_PID_CD_R = 0x86,
    MD_DRIBBLER_PID_CD_L = 0x87,
    MD_DRIBBLER_PID_CD_F = 0x88,

    MD_DRIBBLER_PID_CQ_P = 0x89,
    MD_DRIBBLER_PID_CQ_I = 0x8A,
    MD_DRIBBLER_PID_CQ_D = 0x8B,
    MD_DRIBBLER_PID_CQ_R = 0x8C,
    MD_DRIBBLER_PID_CQ_L = 0x8D,
    MD_DRIBBLER_PID_CQ_F = 0x8E,

    MD_DRIBBLER_PID_V_P = 0x8F,
    MD_DRIBBLER_PID_V_I = 0x90,
    MD_DRIBBLER_PID_V_D = 0x91,
    MD_DRIBBLER_PID_V_R = 0x92,
    MD_DRIBBLER_PID_V_L = 0x93,
    MD_DRIBBLER_PID_V_F = 0x94,

    MD_DRIBBLER_PID_A_P = 0x95,
    MD_DRIBBLER_PID_A_I = 0x96,
    MD_DRIBBLER_PID_A_D = 0x97,
    MD_DRIBBLER_PID_A_R = 0x98,
    MD_DRIBBLER_PID_A_L = 0x99,
    MD_DRIBBLER_PID_A_F = 0x9A,

    MD_DRIBBLER_RESERVED_1 = 0x9B,
    MD_DRIBBLER_RESERVED_2 = 0x9C,
    MD_DRIBBLER_RESERVED_3 = 0x9D,
    MD_DRIBBLER_RESERVED_4 = 0x9E,
    MD_DRIBBLER_RESERVED_5 = 0x9F,

    BB_EXTREMA_COUNT = 0xA0,
    BB_EXPECTED_GAP = 0xA1,
    BB_HYSTERESIS = 0xA2,
    BB_CALIBRATION_WINDOWS = 0xA3,
    BB_CALIBRATION_TIMEOUT_MS = 0xA4,
    BB_STATE_CONFIRM_WINDOWS = 0xA5,
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
