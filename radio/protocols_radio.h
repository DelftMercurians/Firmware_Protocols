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


// Configuration message (bidirectional)
struct MultiConfigMessage {
    HG::Variable vars[5];   // Variable/Parameter that is being accessed

    HG::ConfigOperation operation;         // Configuration operation
    HG::VariableType type;  // not used...

    uint8_t _pad;

    uint32_t values[5];     // Value to be written/that is being acknowledged
};

enum class Access : uint8_t {
    NONE,
    READ,
    WRITE,
    READWRITE,
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

// Command from mothership to robot (28 bytes)
struct Command {
    HG::Pose speed;                 // Desired robot speed (12 bytes)
    // uint8_t _pad0;

    float dribbler_speed;           // Desired dribbler speed (4 bytes)

    KickerCommand kicker_command;   // Command for the kicker (1 byte)

    uint8_t _pad[3];    // Explicit padding for bindgen (3 bytes)

    float kick_time;                // How long to kick for (if kick is requested) (4 bytes)

    float fan_speed;                // Downforce fan speed (percentage) (4 bytes)
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

// High frequency primary mcu status (28 bytes)
struct PrimaryStatusHF {
    uint16_t pressure;              // (2 bytes)
    uint8_t _pad0[2];    // Explicit padding (2 bytes)
    float motor_speeds[5];          // (20 bytes)
    bool breakbeam_ball_detected;   // (1 byte)
    bool breakbeam_sensor_ok;       // (1 byte)
    uint8_t _pad1[2];    // Explicit padding (2 bytes)
};

// Low frequency primary mcu status (18 bytes)
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

// (24 bytes)
struct ImuReadings {
    float ang_x;
    float ang_y;
    float ang_z;
    float ang_wx;
    float ang_wy;
    float ang_wz;
};

// (28 bytes)
struct OdometryReading {
    float pos_x;    // [m]
    float pos_y;    // [m]
    float ang_z;    // [rad]

    float vel_x;    // [m/s]
    float vel_y;    // [m/s]
    float ang_wz;   // [rad/s]

    float err_est;  // An estimation as to the error (no idea how I'll do this, basically just padding at this point)
};


// (28 bytes)
struct OverrideOdometry {
    float pos_x;    // [m]
    float pos_y;    // [m]
    float ang_z;    // [rad]

    bool set_pos_x;  // whether to override the x position value
    bool set_pos_y;  // whether to override the y position value
    bool set_ang_z;  // whether to override the z angle value

    uint8_t _pad0;

    uint8_t _pad[12]; // Ugh, padding manually
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
    OdometryReading = 0x13,     // Odometry reading
    OverrideOdometry = 0x14,    // Overwrite the odometry reading

    MultiConfigMessage = 0x20,  // Multiple Configuration Accesses

    NoOp = 0xFF,                // No Operation
};

// A structure that can hold messages of any type (32 bytes)
struct Message {
    MessageType mt;             // The message type
    uint8_t _pad[3];    // Explicit padding (3 bytes)
    union {
        Command c;  // 28 bytes
        // Reply r;
        // ConfigMessage cm;
        // Status s;
        MultiConfigMessage mcm;
        PrimaryStatusHF ps_hf; // 28 bytes
        OdometryReading odo; // 28 bytes
        OverrideOdometry over_odo; // 28 bytes
        struct {
            PrimaryStatusLF ps_lf; // 18 bytes + padding
            uint8_t _pad0[10];
        };
        struct {
            ImuReadings ir;
            uint8_t _pad1[4];
        };
    } msg;                      // The message contents

    Message() :
        mt{MessageType::None},
        _pad{0, 0, 0}
    {

    }

    static Message NOOP() {
        Message msg;
        msg.mt = MessageType::NoOp;
        return msg;
    }

    Message(Command c) :
        mt{MessageType::Command},
        _pad{0, 0, 0}
    {
        this->msg.c = c;
    }

    Message(OverrideOdometry over_odo) :
        mt{MessageType::OverrideOdometry},
        _pad{0, 0, 0}
    {
        this->msg.over_odo = over_odo;
    }

    Message(OdometryReading odo) :
        mt{MessageType::OdometryReading},
        _pad{0, 0, 0}
    {
        this->msg.odo = odo;
    }

    Message(PrimaryStatusLF ps_lf) :
        mt{MessageType::PrimaryStatusLF},
        _pad{0, 0, 0}
    {
        this->msg.ps_lf = ps_lf;
    }

    Message(PrimaryStatusHF ps_hf) :
        mt{MessageType::PrimaryStatusHF},
        _pad{0, 0, 0}
    {
        this->msg.ps_hf = ps_hf;
    }

    Message(ImuReadings ir) :
        mt{MessageType::ImuReadings},
        _pad{0, 0, 0}
    {
        this->msg.ir = ir;
    }

    Message(MultiConfigMessage mcm) :
        mt{MessageType::MultiConfigMessage},
        _pad{0, 0, 0}
    {
        this->msg.mcm = mcm;
    }
};
// constexpr size_t sizeOfT = sizeof(OverrideOdometry);

static_assert(sizeof(Message) <= 32, "Message exceeds maximum size");

struct MessageWrapper {
    Radio::SSL_ID id;
    uint8_t _pad[3];
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
