// Delft Mercurians
// Thomas Hettasch
// 2023-06-05
// thomas.hettasch@gmail.com

// Maximum ID length: 11 bits
// Maximum message length: 8 bytes

// ID Structure (11 bits):
// Top 3 bits: DEVICE_ID
// Bottom 8 bits (2 options):
//  8 bits for general commands (0x00 -> 0x3F)
// OR
//  2 bits for ACCESS type
//  6 bits for VARIABLE id

#pragma once
#include <map>
#include "../utils.h"

namespace CAN {

// Make message IDs from variable and access type
#define CAN_GENERATE_MESSAGE_ID_(variable, access) ((uint32_t) variable | (uint32_t) access)
#define CAN_GENERATE_MESSAGE_ID(variable, access) (CAN::MESSAGE_ID) (CAN_GENERATE_MESSAGE_ID_(variable, access))

// Make full ID from destination device and message type
#define CAN_MAKE_ID(Device, Message)     (((uint16_t) Device<<8) | (uint16_t) Message)

// Extract message and device ids from full ID
#define CAN_MAKE_MESSAGE_ID(CanID)            (CAN::MESSAGE_ID) (CanID & 0xff)
#define CAN_GET_DEVICE_ID(CanID)             (CAN::DEVICE_ID) (CanID>>8)

#define UPPER_LIMIT (1e5)
#define LOWER_LIMIT (-UPPER_LIMIT)

// Various device IDs (0x00 -> 0x07, 3 bits)
#define CAN_NUM_DEVICE_IDS (1 << 3)
enum class DEVICE_ID : uint8_t {
    PRIMARY = 0x6,     // Main device on bus, coordinates all other devices (high priority messages)

    ALL = 0x7,         // Send message to all devices (highest priority)
    ANY = 0x0,         // Send message to all devices (lowest priority messages)

    DRIVER_0 = 0x1,    // Motor driver 0 (Wheel motor)
    DRIVER_1 = 0x2,    // Motor driver 1 (Wheel motor)
    DRIVER_2 = 0x3,    // Motor driver 2 (wheel motor)
    DRIVER_3 = 0x4,    // Motor driver 3 (wheel motor)

    DRIVER_A = 0x5,    // Motor driver A (auxiliary motor)
};

// Various accessible variables (0x00 -> 0x3F, 6 bits)
enum class VARIABLE {
    ENABLE = 0x00,
    STATUS = 0x01,

    COMMAND = 0x0A,
    ENCODER = 0x0B,
    
    POSITION = 0x10,
    SPEED = 0x11,
    TORQUE = 0x12,
    TORQUE_FF = 0x13,
    
    POSITION_MES = 0x14,
    SPEED_MES = 0x15,
    
    MOTION_TYPE = 0x16, // Motion control type (voltage, velocity, angle, ol_velocity, ol_angle)
    TORQUE_TYPE = 0x17, // Torque control type (voltage, dc_current, foc_current)
    
    LIM_C = 0x18,       // Current limit
    LIM_U = 0x19,       // Voltage limit
    LIM_V = 0x1A,       // Speed limit
    
    // Current D
    PID_CD_P = 0x20,    // Proportional
    PID_CD_I = 0x21,    // Integral
    PID_CD_D = 0x22,    // Derivative
    PID_CD_R = 0x23,    // Ramp
    PID_CD_L = 0x24,    // Limit
    PID_CD_F = 0x25,    // LPF time constant
    
    // Current Q
    PID_CQ_P = 0x28,
    PID_CQ_I = 0x29,
    PID_CQ_D = 0x2A,
    PID_CQ_R = 0x2B,
    PID_CQ_L = 0x2C,
    PID_CQ_F = 0x2D,
    
    // Velocity
    PID_V_P = 0x30,
    PID_V_I = 0x31,
    PID_V_D = 0x32,
    PID_V_R = 0x33,
    PID_V_L = 0x34,
    PID_V_F = 0x35,
    
    // Angle
    PID_A_P = 0x38,
    PID_A_I = 0x39,
    PID_A_D = 0x3A,
    PID_A_R = 0x3B,
    PID_A_L = 0x3C,
    PID_A_F = 0x3D,
    
    MASK = 0x3F,
};

#define CAN_VARIABLE_TYPE float
#define CAN_VARIABLE_TYPE_INT uint32_t


// Wheel speed command message
#define CAN_SCALE_SPEED (400.0/INT16_MAX)
struct COMMAND {
    int16_t speeds[4];
};
// *******

// Motor Status message
#define CAN_SCALE_TEMP (200.0/INT16_MAX)
#define CAN_SCALE_BATV (50.0/INT16_MAX)
struct MotorStatus {
    HG::Status status;
    int16_t temp;
    int16_t battV;
};
struct MotorStatusMessage {
    DEVICE_ID txId;    // transmitting device
    MotorStatus ms;    // value
};
// *******

// Motor encoder message
struct EncoderFeedback {
    DEVICE_ID txId;     // transmitting device
    float speed;        // encoder measured speed
};
// *******


// Type of variable access
enum class ACCESS {
    READ = 0x40,
    WRITE = 0x80,
    MASK = 0xC0,
};


// Combines all message types in one. Some options are not explicitly named here (0x00 -> 0xFF, 8 bits)
// 0x00 -> 0x3F are general commands
// 0x40 -> 0x7F are variable read requests
// 0x80 -> 0xBF are variable write requests
// 0xC0 -> 0xFF are variable return answers
enum class MESSAGE_ID {
    ANNOUNCE = 0x00,        // Use for announcing presence on bus
    REQ_ANNOUNCE = 0x01,    // Use for requesting announcing presence on bus

    ACK = 0x05,             // Acknowledge messages

    LED_SET_DUTY = 0x11,    // Set led blink duty cycle ** defunct, might remove

    SYNC = 0x36,            // Synchronise clocks

    ESTOP = 0x3F,           // Emergency stop all drivers (highest priority) ** Priorities have changed
    STOP = 0x30,            // Gracefull stop all drivers (high priority) ** Priorities have changed

    SET_POSITION = CAN_GENERATE_MESSAGE_ID_(VARIABLE::POSITION, ACCESS::WRITE),    // Set a motor driver's position setpoint
    SET_SPEED = CAN_GENERATE_MESSAGE_ID_(VARIABLE::SPEED, ACCESS::WRITE),       // Set a motor driver's speed setpoint
    SET_TORQUE = CAN_GENERATE_MESSAGE_ID_(VARIABLE::TORQUE, ACCESS::WRITE),      // Set a motor driver's torque setpoint
    
    GET_POSITION = CAN_GENERATE_MESSAGE_ID_(VARIABLE::POSITION, ACCESS::READ),    // Get the encoder position
    GET_SPEED = CAN_GENERATE_MESSAGE_ID_(VARIABLE::SPEED, ACCESS::READ),       // Get encoder speed
    GET_TORQUE = CAN_GENERATE_MESSAGE_ID_(VARIABLE::TORQUE, ACCESS::READ),      // Get current torque output

    GET_POSITION_MES = CAN_GENERATE_MESSAGE_ID_(VARIABLE::POSITION_MES, ACCESS::READ),
    GET_SPEED_MES = CAN_GENERATE_MESSAGE_ID_(VARIABLE::SPEED_MES, ACCESS::READ),

    RETURN_POSITION_MES = CAN_GENERATE_MESSAGE_ID_(VARIABLE::POSITION_MES, ACCESS::MASK),
    RETURN_SPEED_MES = CAN_GENERATE_MESSAGE_ID_(VARIABLE::SPEED_MES, ACCESS::MASK),

    STATUS = CAN_GENERATE_MESSAGE_ID_(VARIABLE::STATUS, ACCESS::MASK),
    ENCODER = CAN_GENERATE_MESSAGE_ID_(VARIABLE::ENCODER, ACCESS::MASK),
    
};

// Maps actual hardware UIDs to device ID
// Mapping may be done in other ways later
const std::map<uint32_t, DEVICE_ID> DEVICE_ID_MAP {
    {0x200034, DEVICE_ID::DRIVER_0},
    {0x230034, DEVICE_ID::DRIVER_1},
    {0x370033, DEVICE_ID::DRIVER_2},
    {0x220027, DEVICE_ID::DRIVER_3},

    {0x5C004D, DEVICE_ID::DRIVER_0},
    {0x640046, DEVICE_ID::DRIVER_1},
    {0x560044, DEVICE_ID::DRIVER_2},
    {0x320026, DEVICE_ID::DRIVER_3},
};

// Used to send variables back to answer a request
struct Value_Return {
    DEVICE_ID txId;             // transmitting device
    CAN_VARIABLE_TYPE value;    // value
};


}
