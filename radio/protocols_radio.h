// Delft Mercurians
// Thomas Hettasch
// 2023-05-18
// thomas.hettasch@gmail.com

#pragma once
#include "../utils.h"
#include "../can/protocols_can.h"

namespace Radio {

typedef uint8_t SSL_ID;

const uint64_t BaseAddress_BtR = 0x324867LL;    // Address base to robot
const uint64_t BaseAddress_RtB = 0x4248A7LL;    // Address robot to bases (LSB must be different enough for uniqueness to kick in)

/* CONFIG MESSAGES */
// Configuration operations
enum class ConfigOperation {
    NONE,                   // Don't do anything with this
    SET_DEFAULT,            // Set a parameter to defaults
    SET_DEFAULT_RETURN,     // Reply stating value set to defaults
    READ = (int) CAN::ACCESS::READ,                   // Read a parameter from the robot
    READ_RETURN = (int) CAN::ACCESS::MASK,            // Robot return message with value
    WRITE = (int) CAN::ACCESS::WRITE,                  // Write a parameter to the robot
    WRITE_RETURN,           // Robot return message stating new value
};

// Configuration message (bidirectional)
struct ConfigMessage {
    ConfigOperation op;         // Configuration operation
    CAN::VARIABLE var;          // Variable/Parameter that is being accessed
    CAN_VARIABLE_TYPE value;    // Value to be written/that is being acknowledged
};


/* COMMAND MESSAGES */
// Kicker subcommands
enum class KickerCommand : uint8_t {
    NONE,

    ARM,        // Arm the high voltage circuitry
    DISARM,     // Disarm the high voltage circuitry

    DISCHARGE,  // Discharge the capacitor

    KICK,       // Kick the ball
    CHIP,       // Chip the ball

    POWER_BOARD_OFF,    // Switch the power board off, shouldn't happen here, but what can I say
};

// Command from mothership to robot
struct Command {
    HG::Pose speed;                 // Desired robot speed

    float dribbler_speed;           // Desired dribbler speed

    KickerCommand kicker_command;   // Command for the kicker
    float kick_time;                // How long to kick for (if kick is requested)

    float fan_speed;                // Downforce fan speed (percentage)
};


/* REPLY MESSAGES */
// TODO: scrap this
// Reply from robot to mothership
struct Reply {
    HG::Status status;          // Robot MCU status
    HG::Status md_status[5];    // Motor driver MCU statuses
    HG::Status kick_status;     // Kicker status
    HG::Status fan_status;      // Fan status
    HG::Pose speed;             // Measured speed
};

/* STATUS MESSAGES */
// Repeated status heartbeat from robot
struct Status {
    // HG::Status primary_status;
    // CAN::MotorStatus motor_status[5];
    // HG::KickerStatus kicker_status;
    // HG::Status fan_status;
};

struct PrimaryStatusHF {
    uint16_t pressure;
    float motor_speeds[5];  // TODO: motor speeds readings (for later)
    bool breakbeam_ball_detected;
    bool breakbeam_sensor_ok;
};

struct PrimaryStatusLF {
    uint8_t pack_voltages[2];
    uint8_t motor_driver_temps[5];
    uint8_t cap_voltage;
    uint8_t kicker_temp;

    HG::Status primary_status;
    HG::Status kicker_status;
    HG::Status fan_status;
    HG::Status imu_status;  // TODO: IMU chip status (for later)
    HG::Status motor_status[5];
};

struct ImuReadings {
    float ang_x;
    float ang_y;
    float ang_z;
    float ang_wx;
    float ang_wy;
    float ang_wz;
};

// A list of all possible message types transmitted over radio
// Note: never repeat IDs, to avoid back-compatibility bugs
enum class MessageType : uint8_t {
    None = 0x00,               // No message received
    Command = 0x01,            // A command message
    Reply = 0x02,              // A reply from the robot
    ConfigMessage = 0x03,      // A configuration message
    // Status = 0x04,             // A hearbeat status message
    PrimaryStatusHF = 0x10,    // A hearbeat status message (high freq.)
    PrimaryStatusLF = 0x11,    // A hearbeat status message (low freq.)
    ImuReadings = 0x12,        // IMU readings message
};

// A structure that can hold messages of any type
struct Message {
    MessageType mt;             // The message type
    union {
        Command c;
        Reply r;
        ConfigMessage cm;
        // Status s;
        PrimaryStatusHF ps_hf;
        PrimaryStatusLF ps_lf;
        ImuReadings ir;
    } msg;                      // The message contents
};

static_assert(sizeof(Message) <= 32, "Message exceeds maximum size");

struct MessageWrapper {
    Radio::SSL_ID id;
    Message msg;
};

// These functions define robot <=> radio (pipe) assignments, only use these functions

inline constexpr Radio::SSL_ID getRobotID(uint8_t pipe, uint8_t radio_id, uint8_t num_radios_online) {
    return (pipe - 1) * num_radios_online + radio_id;
}
inline constexpr uint8_t getPipe(Radio::SSL_ID robot_id, uint8_t num_radios_online) {
    return (robot_id/num_radios_online) + 1;
}
inline constexpr uint8_t getRadioID(Radio::SSL_ID robot_id, uint8_t num_radios_online) {
    return (robot_id % num_radios_online);
}

// Some hamfisted tests
static_assert(getRadioID(getRobotID(1, 3, 4), 4) == 3);
static_assert(getPipe(getRobotID(1, 3, 4), 4) == 1);
static_assert(getRobotID(getPipe(0xC, 4), getRadioID(0xC, 4), 4) == 0xC);
static_assert(getRadioID(getRobotID(1, 2, 3), 3) == 2);
static_assert(getPipe(getRobotID(4, 2, 3), 3) == 4);
static_assert(getRobotID(getPipe(0x3, 3), getRadioID(0x3, 3), 3) == 0x3);
}
