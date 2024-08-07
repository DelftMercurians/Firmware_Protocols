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
#include "../utils.h"

namespace CAN {

// Make message IDs from variable and access type
#define CAN_GENERATE_MESSAGE_ID_(variable, access) ((uint32_t) variable | (uint32_t) access)
#define CAN_GENERATE_MESSAGE_ID(variable, access) (::CAN::MESSAGE_ID) (CAN_GENERATE_MESSAGE_ID_(variable, access))

// Make full ID from destination device and message type
#define CAN_MAKE_ID(Device, Message)     (((uint16_t) Device<<8) | (uint16_t) Message)

// Extract message and device ids from full ID
#define CAN_MAKE_MESSAGE_ID(CanID)            (::CAN::MESSAGE_ID) (CanID & 0xff)
#define CAN_GET_DEVICE_ID(CanID)             (::CAN::DEVICE_ID) (CanID>>8)

#define UPPER_LIMIT (1e5)
#define LOWER_LIMIT (-UPPER_LIMIT)


// Various accessible variables (0x00 -> 0x3F, 6 bits)
enum class VARIABLE {
    ENABLE = 0x00,
    MODE = ENABLE, // Alias
    
    STATUS = 0x01,

    UNIQUE_ID_0 = 0x02,
    UNIQUE_ID_1 = 0x03,
    UNIQUE_ID_2 = 0x04,

    PACK_L_VOLTAGE = 0x05,  // Left battery pack voltage <float>
    PACK_R_VOLTAGE = 0x06,  // Right battery pack voltage <float>
    PACK_EN = 0x07,         // Battery pack enable <HG::PackEnable>
    FAN_SPEED = 0x08,       // Fan speed percentage (0 = off, <50 = low, >50 = high)
    STATUS_POWER_BOARD = 0x09,  // <HG::Status>

    COMMAND = 0x0A,
    ENCODER = 0x0B,

    STATUS_KICKER = 0x0C,   // <HG::Status>
    KICKER_CAP_VOLTAGE = 0x0D,  // Check the capacitor voltage <float>
    KICK_TRIGGER_SPEED = 0x0E,  // Trigger a kick of a specific speed <float> (m/s)
    CHIP_TRIGGER_SPEED = 0x0F,  // Trigger a chip of a specific speed <float> (m/s)
    
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
const float CAN_SCALE_SPEED = (400.0/INT16_MAX);
struct COMMAND {
    int16_t speeds[4];
};
static_assert(sizeof(COMMAND) <= 8, "COMMAND exceeds maximum size");
// *******

// Motor Status message
const float CAN_SCALE_TEMP = (200.0/INT8_MAX);
const float CAN_SCALE_BATV = (50.0/INT8_MAX);
struct MotorStatus {
    HG::Status status;
    int8_t temp;
    int8_t battV;
};
struct MotorStatusMessage {
    DEVICE_ID txId;    // transmitting device
    MotorStatus ms;    // value
};
static_assert(sizeof(MotorStatusMessage) <= 8, "MotorStatusMessage exceeds maximum size");
// *******

// Motor encoder message
struct EncoderFeedback {
    DEVICE_ID txId;     // transmitting device
    float speed;        // encoder measured speed
};
static_assert(sizeof(EncoderFeedback) <= 8, "EncoderFeedback exceeds maximum size");
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


// Used to send variables back to answer a request
struct Value_Return {
    DEVICE_ID txId;             // transmitting device
    CAN_VARIABLE_TYPE value;    // value
};

static_assert(sizeof(Value_Return) <= 8, "Value_Return exceeds maximum size");

}


// =========== Definitions for STM32 CAN chip (MCP2562) ============
// Modified from: https://github.com/nopnop2002/Arduino-STM32-CAN/blob/master/stm32f103/stm32f103.ino
namespace STM32::CAN {

// Symbolic names for bit rate of CAN message
enum Bitrate {
    CAN_50kbps = 0,
    CAN_100kbps,
    CAN_125kbps,
    CAN_250kbps,
    CAN_500kbps,
    CAN_1000kbps
};

// Real speed for bit rate of CAN message
constexpr uint32_t SPEED[6] =
  {50 * 1000, 100 * 1000, 125 * 1000, 250 * 1000, 500 * 1000, 1000 * 1000};

enum Error {
    Unsupported_bit_rate = 1000,
    MSR_INAK_not_set = 1001,
    MSR_INAK_not_cleared = 1002,
    Unsupported_frame_format = 1003
};

// Symbolic names for remapping CAN_RX and CAN_TX pins respectively
enum class Ports {
    PA11_PA12 = 0,
    PB8_PB9 = 2,
    PD0_PD1 = 3
};

// Symbolic names for formats of CAN message
enum Format {
    Standard = 0,
    Extended
};

// Symbolic names for type of CAN message
enum Frame {
    Data = 0,
    Remote
};

typedef struct {
    uint32_t id;      // 29 bit identifier
    uint8_t data[8];  // Data field
    uint8_t len;      // Length of data field in bytes
    uint8_t format;   // 0 - Standard, 1- Extended Identifier
    uint8_t type;     // 0 - Data Frame, 1 - Remote Frame
} msg_t;

typedef struct {
    uint16_t baud_rate_prescaler;          // [1 to 1024]
    uint8_t time_segment_1;                // [1 to 16]
    uint8_t time_segment_2;                // [1 to 8]
    uint8_t resynchronization_jump_width;  // [1 to 4] (recommended value is 1)
} bit_timing_config_t;

constexpr uint32_t TIR_TXRQ = (1U << 0U);  // Bit 0: Transmit Mailbox Request
constexpr uint32_t TIR_RTR = (1U << 1U);   // Bit 1: Remote Transmission Request
constexpr uint32_t TIR_IDE = (1U << 2U);   // Bit 2: Identifier Extension
constexpr uint32_t RIR_RTR = (1U << 1U);   // Bit 1: Remote Transmission Request
constexpr uint32_t RIR_IDE = (1U << 2U);   // Bit 2: Identifier Extension

constexpr uint32_t EXT_ID_MASK = 0x1FFFFFFFU;
constexpr uint32_t STD_ID_MASK = 0x000007FFU;

}  // namespace STM32::CAN
