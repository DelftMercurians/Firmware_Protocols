// Delft Mercurians
// Thomas Hettasch
// 2023-06-05
// thomas.hettasch@gmail.com

#pragma once
#include <map>

namespace CAN {

#define CAN_GENERATE_MESSAGE_ID_(variable, access) ((uint32_t) variable | (uint32_t) access)
#define CAN_GENERATE_MESSAGE_ID(variable, access) (CAN::MESSAGE_ID) (CAN_GENERATE_MESSAGE_ID_(variable, access))

#define UPPER_LIMIT (1e5)
#define LOWER_LIMIT (-UPPER_LIMIT)

enum class DEVICE_ID {
    PRIMARY = 0xF0,     // Main device on bus, coordinates all other devices (high priority messages)

    ALL = 0xFF,         // Send message to all devices (highest priority)
    ANY = 0x00,         // Send message to all devices (lowest priority messages)

    DRIVER_A1 = 0x01,    // Motor driver 1
    DRIVER_A2 = 0x02,    // Motor driver 2
    DRIVER_A3 = 0x03,    // Motor driver 3
    DRIVER_A4 = 0x04,    // Motor driver 4
    DRIVER_A5 = 0x05,    // Motor driver 5

    DRIVER_B1 = 0x06,    // Motor driver 1
    DRIVER_B2 = 0x07,    // Motor driver 2
    DRIVER_B3 = 0x08,    // Motor driver 3
    DRIVER_B4 = 0x09,    // Motor driver 4
    DRIVER_B5 = 0x0A,    // Motor driver 5

    DRIVER_C1 = 0x0B,    // Motor driver 1
    DRIVER_C2 = 0x0C,    // Motor driver 2
    DRIVER_C3 = 0x0D,    // Motor driver 3
    DRIVER_C4 = 0x0E,    // Motor driver 4
    DRIVER_C5 = 0x0F,    // Motor driver 5
};

enum class VARIABLE {
    ENABLE = 0x00,
    
    POSITION = 0x01,
    SPEED = 0x02,
    TORQUE = 0x03,
    TORQUE_FF = 0x04,
    
    MOTION_TYPE = 0x10, // Motion control type (voltage, velocity, angle, ol_velocity, ol_angle)
    TORQUE_TYPE = 0x11, // Torque control type (voltage, dc_current, foc_current)
    
    LIM_C = 0x18,       // Current limit
    LIM_U = 0x19,       // Voltage limit
    LIM_V = 0x20,       // Speed limit
    
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

enum class ACCESS {
    READ = 0x40,
    WRITE = 0x80,
    MASK = 0xC0,
};

enum class MESSAGE_ID {
    ANNOUNCE = 0x00,        // Use for announcing presence on bus
    REQ_ANNOUNCE = 0x01,    // Use for requesting announcing presence on bus

    ACK = 0x05,             // Acknowledge messages

    LED_SET_DUTY = 0x11,    // Set led blink duty cycle

    SET_POSITION = CAN_GENERATE_MESSAGE_ID_(VARIABLE::POSITION, ACCESS::WRITE),    // Set a motor driver's position setpoint
    SET_SPEED = CAN_GENERATE_MESSAGE_ID_(VARIABLE::SPEED, ACCESS::WRITE),       // Set a motor driver's speed setpoint
    SET_TORQUE = CAN_GENERATE_MESSAGE_ID_(VARIABLE::TORQUE, ACCESS::WRITE),      // Set a motor driver's torque setpoint
    
    GET_POSITION = CAN_GENERATE_MESSAGE_ID_(VARIABLE::POSITION, ACCESS::READ),    // Get the encoder position
    GET_SPEED = CAN_GENERATE_MESSAGE_ID_(VARIABLE::SPEED, ACCESS::READ),       // Get encoder speed
    GET_TORQUE = CAN_GENERATE_MESSAGE_ID_(VARIABLE::TORQUE, ACCESS::READ),      // Get current torque output

    SYNC = 0xF6,            // Synchronise clocks

    ESTOP = 0xFF,           // Emergency stop all drivers (highest priority)
    STOP = 0xF0,            // Gracefull stop all drivers (high priority)
};

const std::map<uint32_t, DEVICE_ID> DEVICE_ID_MAP {
    {0x200034, DEVICE_ID::DRIVER_A1},
    {0x230034, DEVICE_ID::DRIVER_A2},
    {0x370033, DEVICE_ID::DRIVER_A3},
    {0x220027, DEVICE_ID::DRIVER_A4},
};



#define CAN_MAKE_ID(Device, Message)     (((uint16_t) Device<<8) | (uint16_t) Message) 
#define CAN_MAKE_MESSAGE_ID(CanID)            (CAN::MESSAGE_ID) (CanID & 0xff)
#define CAN_GET_DEVICE_ID(CanID)             (CAN::DEVICE_ID) (CanID>>8)

}
