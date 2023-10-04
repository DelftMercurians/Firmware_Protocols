// Delft Mercurians
// Thomas Hettasch
// 2023-06-05
// thomas.hettasch@gmail.com

// Maximum ID length: 11 bits
// Maximum message length: 8 bytes

// ID Structure (11 bits):
// Top 1 bit - configuration variable or not
// 1 bit - direction motor to/from primary
// 3 bits - device id
// Bottom 6 bits - config variable or message

#pragma once
#include <map>
#include "../utils.h"

namespace CAN {

const float UPPER_LIMIT = (1e5);
const float LOWER_LIMIT = (-UPPER_LIMIT);

// Various device IDs (0x00 -> 0x07, 3 bits)
const size_t NUM_DEVICE_IDS = 1 << 3;
enum class DEVICE : uint8_t {
    PRIMARY = 0x6,     // Main device on bus, coordinates all other devices (high priority messages)

    ALL = 0x7,         // Send message to all devices (highest priority)
    ANY = 0x0,         // Send message to all devices (lowest priority messages)

    DRIVER_0 = 0x1,    // Motor driver 0 (Wheel motor)
    DRIVER_1 = 0x2,    // Motor driver 1 (Wheel motor)
    DRIVER_2 = 0x3,    // Motor driver 2 (wheel motor)
    DRIVER_3 = 0x4,    // Motor driver 3 (wheel motor)

    DRIVER_A = 0x5,    // Motor driver A (auxiliary motor)
};

// Various accessible configuration variables (0x00 -> 0x3F, 6 bits)
enum class CONFVAR : uint8_t {
   
    MOTION_TYPE, // Motion control type (voltage, velocity, angle, ol_velocity, ol_angle)
    TORQUE_TYPE, // Torque control type (voltage, dc_current, foc_current)
    
    LIM_C,       // Current limit
    LIM_U,       // Voltage limit
    LIM_V,       // Speed limit
    
    // Current D
    PID_CD_P,    // Proportional
    PID_CD_I,    // Integral
    PID_CD_D,    // Derivative
    PID_CD_R,    // Ramp
    PID_CD_L,    // Limit
    PID_CD_F,    // LPF time constant
    
    // Current Q
    PID_CQ_P,
    PID_CQ_I,
    PID_CQ_D,
    PID_CQ_R,
    PID_CQ_L,
    PID_CQ_F,
    
    // Velocity
    PID_V_P,
    PID_V_I,
    PID_V_D,
    PID_V_R,
    PID_V_L,
    PID_V_F,
    
    // Angle
    PID_A_P,
    PID_A_I,
    PID_A_D,
    PID_A_R,
    PID_A_L,
    PID_A_F,

    // Monitoring
    LOOP_TIME_AVG,
    
    MASK = 0x3F,
};


typedef union {
    uint32_t val_u;
    int32_t val_i;
    float val_f;
} CONFVAR_TYPE;

inline CONFVAR_TYPE make_confvar(CONFVAR_TYPE ci) { return ci; }
inline CONFVAR_TYPE make_confvar(float f) { CONFVAR_TYPE ci; ci.val_f=f; return ci; }
inline CONFVAR_TYPE make_confvar(uint32_t u) { CONFVAR_TYPE ci; ci.val_u=u; return ci; }
inline CONFVAR_TYPE make_confvar(int32_t i) { CONFVAR_TYPE ci; ci.val_i=i; return ci; }


// Command / high speed communication identifiers
// 6 bits(max 0x3F)
enum class MESSAGE : uint8_t {
    ESTOP = 0x00,           // Emergency stop all drivers (highest priority)
    STOP = 0x01,            // Gracefull stop all drivers (high priority)

    SYNC = 0x02,            // Synchronise clocks

    ENABLE = 0x03,          // Enable / disable motor
    STATUS = 0x04,          // Motor status

    MULTI_SPEED_SET = 0x10, // Special message type to set all 4 motor speeds in a single message
    
    POSITION_SET = 0x11,    // Position setpoint (not really used on wheel motors)
    SPEED_SET = 0x12,       // Speed setpoint
    TORQUE_SET = 0x13,      // Torque setpoint
    TORQUE_FF = 0x14,       // Torque feedforward

    POSITION_MES = 0x21,    // Measured encoder position
    SPEED_MES = 0x22,       // Measured encoder speed
    TORQUE_MES = 0x23,      // Measured motor torque

    REQ_CONFVAR = 0x2A,     // Request a configuration variable's value
    
    ANNOUNCE = 0x3C,        // Use for announcing presence on bus
    REQ_ANNOUNCE = 0x3D,    // Use for requesting announcing presence on bus

    ACK = 0x3E,             // Acknowledge messages (could be done with native CAN)

    MASK = 0x3F,
};


enum class TYPE : uint8_t {
    ERROR,
    BOOL,
    FLOAT,
    INT16,
    DEVICE,
    MULTI_SPEED,
    MOTOR_STATUS,
};

const std::map<MESSAGE, TYPE> COMMAND_TYPE_MAP {
    {MESSAGE::ESTOP,            TYPE::INT16},
    {MESSAGE::STOP,             TYPE::INT16},
    {MESSAGE::SYNC,             TYPE::INT16},
    {MESSAGE::ENABLE,           TYPE::BOOL},
    {MESSAGE::STATUS,           TYPE::MOTOR_STATUS},
    {MESSAGE::MULTI_SPEED_SET,  TYPE::MULTI_SPEED},

    {MESSAGE::POSITION_SET,     TYPE::FLOAT},
    {MESSAGE::SPEED_SET,        TYPE::FLOAT},
    {MESSAGE::TORQUE_SET,       TYPE::FLOAT},
    {MESSAGE::TORQUE_FF,        TYPE::FLOAT},

    {MESSAGE::POSITION_MES,     TYPE::FLOAT},
    {MESSAGE::SPEED_MES,        TYPE::FLOAT},
    {MESSAGE::TORQUE_MES,       TYPE::FLOAT},

    {MESSAGE::ANNOUNCE,         TYPE::DEVICE},
    {MESSAGE::REQ_ANNOUNCE,     TYPE::DEVICE},
    {MESSAGE::ACK,              TYPE::INT16},

    {MESSAGE::MASK,             TYPE::ERROR},       // Default type
};

const std::map<CONFVAR, TYPE> CONFVAR_TYPE_MAP {
    {CONFVAR::MOTION_TYPE,            TYPE::INT16},
    {CONFVAR::TORQUE_TYPE,            TYPE::INT16},

    {CONFVAR::MASK,                   TYPE::FLOAT},    // Default type
};

// CAN ID direction
enum class DIR : bool {
    MOTOR_TO_PRIMARY = false,
    PRIMARY_TO_MOTOR = true,

    MtP = MOTOR_TO_PRIMARY,     // Motor to Primary alias
    PtM = PRIMARY_TO_MOTOR,     // Primary to Motor alias
};

// CAN ID definition
typedef union CAN_ID_UNION {
    uint16_t raw : 11;
    struct {
        union {
            CONFVAR confvar : 6;
            MESSAGE msg : 6;
        };
        DEVICE device : 3;
        DIR direction : 1;
        bool config : 1;
    };

    CAN_ID_UNION(DEVICE did, CONFVAR value, DIR dir) :
        device{did}, 
        direction{dir},
        confvar{value},
        config{true} {}

    CAN_ID_UNION(DEVICE did, MESSAGE value, DIR dir) :
        device{did}, 
        direction{dir},
        msg{value},
        config{false} {}

    CAN_ID_UNION(uint32_t raw) :
        raw{raw} {}

    CAN_ID_UNION() :
        raw{0x3FF} {}

    void print() {
        Serial.printf("Raw = 0x%04x\n", raw);
        Serial.printf("Device    = 0x%02x\n", device);
        Serial.printf("Direction = %s\n", String((bool) direction ? "P -> M" : "M -> P" ).c_str());
        Serial.printf("Type      = %s\n", String(config ? "config" : "command" ).c_str());
        if(config) {
        Serial.printf("Variable  = 0x%02x\n", confvar);
        } else {
        Serial.printf("Command   = 0x%02x\n", msg);
        }
    }

} ID;



//// Special message types ////

// Wheel speed command message
#define CAN_SCALE_SPEED (400.0/INT16_MAX)
struct MultiSpeed {
    int16_t speeds[4];
};
static_assert(sizeof(MultiSpeed) <= 8, "MultiSpeed exceeds maximum size");
// *******

// Motor Status message
#define CAN_SCALE_TEMP (200.0/INT16_MAX)
#define CAN_SCALE_BATV (50.0/INT16_MAX)
struct MotorStatus {
    HG::Status status;
    int16_t temp;
    int16_t battV;
};
static_assert(sizeof(MotorStatus) <= 8, "MotorStatus exceeds maximum size");
// *******


// Maps actual hardware UIDs to device ID
// Mapping may be done in other ways later
const std::map<uint32_t, DEVICE> DEVICE_ID_MAP {
    {0x200034, DEVICE::DRIVER_0},
    {0x230034, DEVICE::DRIVER_1},
    {0x370033, DEVICE::DRIVER_2},
    {0x220027, DEVICE::DRIVER_3},

    {0x5C004D, DEVICE::DRIVER_0},
    {0x640046, DEVICE::DRIVER_1},    // blown up CAN tranceiver
    {0x560062, DEVICE::DRIVER_1},    // replacement board
    {0x560044, DEVICE::DRIVER_2},
    {0x320026, DEVICE::DRIVER_3},
};

}
