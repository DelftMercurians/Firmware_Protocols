// Delft Mercurians
// Thomas Hettasch
// 2023-05-18
// thomas.hettasch@gmail.com

#pragma once
#include "../utils.h"
#include "../can/protocols_can.h"

namespace Radio {

typedef uint8_t SSL_ID;

const SSL_ID Broadcast_ID = 0xEE;
const SSL_ID BaseStation_ID = 0xB0;

const uint64_t BaseAddress_BtR = 0x334867LL;    // Address base to robot
const uint64_t BaseAddress_RtB = 0x4348A7LL;    // Address robot to bases (LSB must be different enough for uniqueness to kick in)
const uint64_t BroadcastAddress = 0x3348F7LL;   // Address base to all robots

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
    READ,       // Allow only reading the variable from the robot
    WRITE,      // Allow only writing the variable to the robot
    READWRITE,  // Allow both writing/reading to/from the robot
};

/* COMMAND MESSAGES */

enum class RobotCommand : uint8_t;

struct RobotCommand_ {
    // (3 bit) Max 7
    enum class KickerCommand : uint8_t {
        NONE = 0x0,         // Do not send commands to the kicker
        ARM = 0x1,          // Arm the kicker
        DISARM = 0x2,       // Disarm, but do not discharge the kicker
        DISCHARGE = 0x3,    // Discharge the kicker capacitor
        LEGACY_KICK = 0x4,  // Perform a kick/chip the old school way
        ARM_COUNTER = 0x5,  // Arm the kicker and perform a kick if the counter increments
        ARM_TIMED = 0x6,    // Arm the kicker and perform a kick when the timer expsires
        ARM_REFLEX = 0x7,   // Arm the kicker and perform a kick when the ball is detected
    };

    // (1 bit) Max 1
    enum class KickerSelect : uint8_t {
        UNSPECIFIED = 0,    // Do not specify whether to use the kicker or chipper (for none, etc)
        KICKER = 0,         // Use the kicker
        CHIPPER = 1,        // Use the chipper
    };

    // (4 bit) Max 15 = 0xF
    enum class Auxilliary : uint8_t {
        NONE = 0x0,         // Do not perform an auxillary action
        COAST = 0x1,        // Coast the motors
        BEEP = 0x2,         // Make the robot beep
        REBOOT = 0x3,       // Reboot the main board microcontroller
        POWER_OFF = 0x4,    // Switch the robot off
        CALIBRATE_IMU = 0x5,  // Calibrate the IMU
        CALIBRATE_BB = 0x6,   // Calibrate the breakbeam sensor
    };

    KickerCommand kicker_command : 3;
    KickerSelect kicker_select  : 1;
    Auxilliary auxilliary     : 4;

    constexpr RobotCommand_(KickerCommand kc, KickerSelect ks = KickerSelect::UNSPECIFIED, Auxilliary aux = Auxilliary::NONE) :
        kicker_command((KickerCommand) (((uint8_t)kc) & 0b111)),
        kicker_select((KickerSelect) (((uint8_t)ks) & 0b1)),
        auxilliary((Auxilliary) (((uint8_t)aux) & 0b1111))
    { }
    
    static constexpr uint8_t to_byte_static(
        KickerCommand kc, KickerSelect ks = KickerSelect::UNSPECIFIED, Auxilliary aux = Auxilliary::NONE
    ) {
        return ((uint8_t)kc  & 0b111)        // bits [2:0]
            | (((uint8_t)ks & 0b001) << 3)  // bit  [3]
            | (((uint8_t)aux & 0b1111) << 4); // bits [7:4]
    }

    static constexpr uint8_t to_byte_static(
        Auxilliary aux
    ) {
        return to_byte_static(KickerCommand::NONE, KickerSelect::UNSPECIFIED, aux);
    }

    static constexpr RobotCommand_ from_byte(uint8_t byte) {
        return RobotCommand_(
            (KickerCommand) (byte        & 0b0111),
            (KickerSelect)  ((byte >> 3) & 0b0001),
            (Auxilliary)    ((byte >> 4) & 0b1111)
        );
    }

    static constexpr RobotCommand_ from_byte(RobotCommand byte) {
        return from_byte((uint8_t) byte);
    }
};
static_assert(sizeof(RobotCommand_) == 1);

// Robot commands
enum class RobotCommand : uint8_t {
    NONE = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::NONE),

    ARM = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM), // Arm the high voltage circuitry

    DISARM = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::DISARM), // Disarm the high voltage circuitry

    DISCHARGE = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::DISCHARGE),  // Discharge the capacitor

    KICK = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::LEGACY_KICK, RobotCommand_::KickerSelect::KICKER),       // Kick the ball
    CHIP = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::LEGACY_KICK, RobotCommand_::KickerSelect::CHIPPER),       // Chip the ball

    POWER_BOARD_OFF = RobotCommand_::to_byte_static(RobotCommand_::Auxilliary::POWER_OFF),    // Switch the power board off, shouldn't happen here, but what can I say
    REBOOT = RobotCommand_::to_byte_static(RobotCommand_::Auxilliary::REBOOT),     // Reboot mainboard

    BEEP = RobotCommand_::to_byte_static(RobotCommand_::Auxilliary::BEEP),   // Make a beep noise

    COAST = RobotCommand_::to_byte_static(RobotCommand_::Auxilliary::COAST),  // Coast all the motors

    HEADING_CONTROL [[deprecated]] = 0x0A,    // DEPRECATED Z command is heading angle (rad)
    YAW_RATE_CONTROL [[deprecated]] = 0x0B,   // DEPRECATED Z command is yaw rate (rad/s)


    ARM_COUNTER_KICK = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM_COUNTER, RobotCommand_::KickerSelect::KICKER),        // Arm the high voltage circuitry, wait for increment of smart kick counter (do kick)
    ARM_COUNTER_CHIP = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM_COUNTER, RobotCommand_::KickerSelect::CHIPPER),        // Arm the high voltage circuitry, wait for increment of smart kick counter (do chip)

    ARM_TIMED_KICK = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM_TIMED, RobotCommand_::KickerSelect::KICKER),     // Arm the high voltage circuitry, set countdown time until kick
    ARM_TIMED_CHIP = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM_TIMED, RobotCommand_::KickerSelect::CHIPPER),     // Arm the high voltage circuitry, set countdown time until chip

    ARM_REFLEX_KICK = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM_REFLEX, RobotCommand_::KickerSelect::KICKER),   // Arm the high voltage circuitry, kick when ball detected
    ARM_REFLEX_CHIP = RobotCommand_::to_byte_static(RobotCommand_::KickerCommand::ARM_REFLEX, RobotCommand_::KickerSelect::CHIPPER),   // Arm the high voltage circuitry, chip when ball detected

    CALIBRATE_IMU = RobotCommand_::to_byte_static(RobotCommand_::Auxilliary::CALIBRATE_IMU),  // Calibrate the IMU
    CALIBRATE_BB = RobotCommand_::to_byte_static(RobotCommand_::Auxilliary::CALIBRATE_BB),    // Calibrate the breakbeam sensor
};

struct GenericCommand {
    int16_t dribbler_speed_i;       // Desired dribbler speed (2 bytes) [rad/s]

    uint16_t kick_time_i;           // Kick time (2 bytes) [ms]
    uint16_t time_to_kick;          // For curved kicking (2 bytes)
    uint8_t smart_kick_couter;      // For kicking more reliably (1 byte)

    RobotCommand robot_command;     // Command for the robot (1 byte)
};

// Command from mothership to robot (28 bytes)
struct Command {
    HG::Pose speed;                 // Desired robot speed (12 bytes)

    GenericCommand gen_command;

    uint8_t _pad[8];
};
static_assert(sizeof(Command) == 28);

struct GlobalCommand {
    float global_speed_x;       // Global X speed [m/s] (facing towards heading = 0)
    float global_speed_y;       // Global Y speed [m/s] (facing towards heading = pi/2)

    float heading_last_measurement; // Last vision estimate of robot heading, can be nan [rad]
    float heading_setpoint;         // Where we want the robot to face [rad]

    GenericCommand gen_command;

    uint16_t max_yaw_rate;  // Maximum yaw rate for yaw controller, [1/10 rad/s]
    int8_t preferred_rotation_direction;    // Direction to turn in (+1, 0, -1)

    uint8_t _pad;    // Explicit padding for bindgen (1 bytes)
};
static_assert(sizeof(GlobalCommand) == 28);


/* REPLY MESSAGES */
// High frequency primary mcu status (28 bytes)
struct PrimaryStatusHF {
    int16_t motor_speeds_i[5];          // (10 bytes) Scaled from float with Scale::WHEEL_SPEED
    int16_t motor_currents_i[5];        // (10 bytes) Scaled from float with Scale::CURRENT

    uint8_t smart_kick_counter_return;  // (1 byte) number of the kick that was ok or not
    struct {
        bool breakbeam_ball_detected : 1;   // (1 bit) Breakbeam sensor is detecting ball
        bool breakbeam_sensor_ok : 1;       // (1 bit) Breakbeam sensor is working

        bool tof_ball_detected : 1;         // (1 bit) Time of flight sensor is detecting ball
        bool tof_sensor_ok : 1;             // (1 bit) Time of flight sensor is working
    };  // (1 byte) Ball detection bitfield
    
    uint8_t tof_ball_x;  // (1 byte) Time of flight ball sensor x position (distance)
    int8_t tof_ball_y;  // (1 byte) Time of flight ball sensor y position (left to right)

    uint16_t breakbeam_raw; // (2 bytes) Raw data from the breakbeam

    struct {
        uint8_t last_kick_ok : 4; // (4 bit), 0 if kick not ok, 1 or more if kick ok
        uint8_t reflex_kick_counter: 4; // (4 bit) number of reflex kicks since last arm
    };  // (1 byte) Kick status bitfield
    
    uint8_t _pad1[1];    // Explicit padding (1 bytes)
};
static_assert(sizeof(PrimaryStatusHF) == 28);

// Low frequency primary mcu status (18 bytes)
struct PrimaryStatusLF {
    int16_t main_board_current; // Scaled from float with Scale::CURRENT

    uint8_t pack_voltages[2];   // Scaled from float with Scale::MD_BATV
    uint8_t motor_driver_temps[5];  // Scaled from float with Scale::MD_TEMP
    
    uint8_t cap_voltage;    // Scaled from float with Scale::KICKER_VCAP

    uint8_t _pad0;

    HG::Status primary_status;
    HG::Status kicker_status;
    HG::Status imu_status;
    HG::Status tof_status;
    HG::Status motor_status[5];
    
    uint16_t avg_loop_time;     // 10 microseconds per LSB
    uint16_t max_loop_time;     // 10 microseconds per LSB

    uint8_t _pad[4];
};
static_assert(sizeof(PrimaryStatusLF) == 28);

// (24 bytes)
struct ImuReadings {
    float ang_x;
    float ang_y;
    float ang_z;
    float ang_wx;
    float ang_wy;
    float ang_wz;
};
static_assert(sizeof(ImuReadings) == 24);

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
static_assert(sizeof(OdometryReading) == 28);


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
static_assert(sizeof(OverrideOdometry) == 28);

// (28 bytes)
struct SerialMessage {
    uint32_t start_offset;
    char text[24];
};
static_assert(sizeof(SerialMessage) == 28);

// A list of all possible message types transmitted over radio
// Note: never repeat IDs, to avoid back-compatibility bugs
enum class MessageType : uint8_t {
    None = 0x00,               // No message received
    Command = 0x01,            // A command message
    // Reply [[deprecated]] = 0x02,              // A reply from the robot
    ConfigMessage = 0x03,      // A configuration message
    // Status [[deprecated]] = 0x04,             // A hearbeat status message
    PrimaryStatusHF = 0x10,    // A hearbeat status message (high freq.)
    PrimaryStatusLF = 0x11,    // A hearbeat status message (low freq.)
    ImuReadings = 0x12,        // IMU readings message
    OdometryReading = 0x13,     // Odometry reading
    OverrideOdometry = 0x14,    // Overwrite the odometry reading
    GlobalCommand = 0x15,       // Global coordinate control
    SerialMessage = 0x16,       // Serial text message

    MultiConfigMessage = 0x20,  // Multiple Configuration Accesses

    NoOp = 0xFF,                // No Operation
};

// A structure that can hold messages of any type (32 bytes)
struct Message {
    MessageType mt;             // The message type
    uint8_t _pad[3];    // Explicit padding (3 bytes)
    union {
        Command c;  // 28 bytes
        GlobalCommand gc;  // 28 bytes
        MultiConfigMessage mcm;
        PrimaryStatusHF ps_hf; // 28 bytes
        OdometryReading odo; // 28 bytes
        OverrideOdometry over_odo; // 28 bytes
        SerialMessage serial; // 28 bytes
        PrimaryStatusLF ps_lf; // 28 bytes
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

    Message(SerialMessage serial) :
        mt{MessageType::SerialMessage},
        _pad{0, 0, 0}
    {
        this->msg.serial = serial;
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
