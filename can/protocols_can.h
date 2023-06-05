// Delft Mercurians
// Thomas Hettasch
// 2023-06-05
// thomas.hettasch@gmail.com

#pragma once
#include <map>

#define GENERATE_MESSAGE_ID_(variable, access) ((uint32_t) variable | (uint32_t) access)
#define GENERATE_MESSAGE_ID(variable, access) (CAN_MESSAGE_ID) (GENERATE_MESSAGE_ID_(variable, access))

#define UPPER_LIMIT (1e5)
#define LOWER_LIMIT (-UPPER_LIMIT)

enum class CAN_DEVICE_ID {
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

enum class CAN_VARIABLE {
    POSITION = 0x00,
    SPEED = 0x01,
    TORQUE = 0x02,
    MASK = 0x0F,
};

#define VARIABLE_TYPE float

enum class CAN_ACCESS {
    READ = 0xB0,
    WRITE = 0xA0,
    MASK = 0xF0,
};

enum class CAN_MESSAGE_ID {
    ANNOUNCE = 0x00,        // Use for announcing presence on bus
    REQ_ANNOUNCE = 0x01,    // Use for requesting announcing presence on bus

    ACK = 0x05,             // Acknowledge messages

    LED_SET_DUTY = 0x11,    // Set led blink duty cycle

    SET_POSITION = GENERATE_MESSAGE_ID_(CAN_VARIABLE::POSITION, CAN_ACCESS::WRITE),    // Set a motor driver's position setpoint
    SET_SPEED = GENERATE_MESSAGE_ID_(CAN_VARIABLE::SPEED, CAN_ACCESS::WRITE),       // Set a motor driver's speed setpoint
    SET_TORQUE = GENERATE_MESSAGE_ID_(CAN_VARIABLE::TORQUE, CAN_ACCESS::WRITE),      // Set a motor driver's torque setpoint

    SYNC = 0xF6,            // Synchronise clocks

    ESTOP = 0xFF,           // Emergency stop all drivers (highest priority)
    STOP = 0xF0,            // Gracefull stop all drivers (high priority)
};

const std::map<uint32_t, CAN_DEVICE_ID> DEVICE_ID_MAP {
    {0x200034, CAN_DEVICE_ID::DRIVER_A1},
    {0x230034, CAN_DEVICE_ID::DRIVER_A2},
    {0x370033, CAN_DEVICE_ID::DRIVER_A3},
    {0x220027, CAN_DEVICE_ID::DRIVER_A4},
};



#define PP_MAKE_CAN_ID(Device, Message)     (((uint16_t) Device<<8) | (uint16_t) Message) 
#define PP_GET_MESSAGE_ID(CanID)            (CAN_MESSAGE_ID) (CanID & 0xff)
#define PP_GET_DEVICE_ID(CanID)             (CAN_DEVICE_ID) (CanID>>8)