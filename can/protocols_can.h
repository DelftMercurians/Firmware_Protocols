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
#ifndef USING_BINDGEN
#include "can_id.h"
#endif

namespace CAN {

// CANopen Node ID (7 bits)
enum class NODE_ID : uint8_t {
    BROADCAST = 0x00,   // Leave unchanged
};

using DEVICE_ID = NODE_ID;

// CANopen FUNCTION_CODE (4 bits)
enum class FUNCTION_CODE : uint8_t {
    NMT = 0b0000,
    SYNC = 0b0001,
    EMCY = 0b0001,
    TIME = 0b0010,
    PDO_1_TX = 0b0011,
    PDO_1_RX = 0b0100,
    PDO_2_TX = 0b0101,
    PDO_2_RX = 0b0110,
    PDO_3_TX = 0b0111,
    PDO_3_RX = 0b1000,
    PDO_4_TX = 0b1001,
    PDO_4_RX = 0b1010,
    SDO_TX = 0b1011,
    SDO_RX = 0b1100,
    HEARTBEART = 0b1110,
};


// CAN ID definition
typedef union CAN_ID_UNION {
    uint16_t raw : 11;
    struct {
        FUNCTION_CODE function_code : 4;
        NODE_ID node_id : 7;
    };

    CAN_ID_UNION(FUNCTION_CODE function_code, NODE_ID node_id) :
        node_id{node_id}, 
        function_code{function_code} {}

    CAN_ID_UNION(uint32_t raw) :
        raw{raw} {}

    void print() {
        Serial.printf("Raw = 0x%04x\n", raw);
        Serial.printf("Node ID       = 0x%02x\n", node_id);
        Serial.printf("Function code = 0x%01x\n", function_code);
    }

} ID;

// IDs where the node_id is always zero
namespace CONST_ID {
    const ID NMT = CAN_ID_UNION(FUNCTION_CODE::NMT, NODE_ID::BROADCAST);
    const ID SYNC = CAN_ID_UNION(FUNCTION_CODE::SYNC, NODE_ID::BROADCAST);
    const ID TIME = CAN_ID_UNION(FUNCTION_CODE::TIME, NODE_ID::BROADCAST);
}



enum class VARIABLE : uint16_t {

};





// CANopen Network management command
enum class NMT_COMMAND : uint8_t {
    GO_OPERATIONAL = 0x01,
    GO_STOPPED = 0x02,
    GO_PRE_OPERATIONAL = 0x80,
    GO_RESET_NODE = 0x81,
    GO_RESET_COMMUNICATION = 0x82,
};

// CANopen Network management state
enum class NMT_STATE : uint8_t {
    BOOT_UP = 0x00,
    STOPPED = 0x04,
    OPERATIONAL = 0x05,
    PRE_OPERATIONAL = 0x7F,
};

// CANopen client command specifier (3 bits)
enum class CCS : uint8_t {
    SEGMENT_DOWNLOAD = 0,
    INITIATE_DOWNLAOD = 1,
    INITIATE_UPLOAD = 2,
    SEGMENT_UPLOAD = 3,
    ABORT_TRANSFER = 4,
    BLOCK_UPLOAD = 5,
    BLOCK_DOWNLAOD = 6,
};

// CANopen client command specifier (3 bits)
enum class TRANSFER_TYPE : uint8_t {
    NORMAL = 0,
    EXPEDITED = 1,
};

// CANopen client command specifier (3 bits)
enum class SIZE_INDICATOR : uint8_t {
    NOT_INDICATED = 0,
    INDICATED = 1,
};

// CAN ID definition
typedef union SDO_UNION {
    uint8_t raw[8];
    struct {
        union {
            uint8_t header;
            struct{
                CCS ccs : 3;
                uint8_t padding : 1;
                union {
                    uint8_t nes : 4;
                    struct {
                        uint8_t n : 2;
                        TRANSFER_TYPE e : 1;
                        SIZE_INDICATOR s : 1;
                    };
                };
            };
        };

        uint16_t index;
        uint8_t subindex;

        union {
            uint8_t data[4];
            uint64_t raw_data;
        };
    };
    

    SDO_UNION(CCS ccs, uint8_t n, TRANSFER_TYPE e, SIZE_INDICATOR s, uint16_t index, uint8_t subindex, uint64_t raw_data) :
        ccs{ccs}, 
        padding{0},
        n{n},
        e{e},
        s{s},
        index{index},
        subindex{subindex},
        raw_data{raw_data} {}

    // template<typename T>
    // SDO_UNION(CCS ccs, uint16_t index, uint8_t subindex, T raw_data) :
    //     ccs{ccs}, 
    //     padding{0},
    //     n{4 - sizeof(T)},
    //     e{TRANSFER_TYPE::EXPEDITED},
    //     s{SIZE_INDICATOR::INDICATED},
    //     index{index},
    //     subindex{subindex},
    //     raw_data{raw_data}
    // {
    //     static_assert(sizeof(T) <= 4);
    // }

    void print() {
        Serial.printf("Raw = %02x %02x %02x %02x %02x %02x %02x %02x\n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7]);
        // TODO print individual bits
    }

} SDO;

static_assert(sizeof(SDO_UNION) == 8);

// Useful functions from making SDOs
namespace CONST_SDO {
    inline SDO REQUEST_VAR(uint16_t index, uint8_t subindex) { return SDO_UNION(CCS::INITIATE_UPLOAD, 0, (TRANSFER_TYPE) 0, (SIZE_INDICATOR) 0, index, subindex, 0); };

    template<typename T>
    inline SDO RETURN_VAR(uint16_t index, uint8_t subindex, T val) { return SDO_UNION(CCS::INITIATE_UPLOAD, 4 - sizeof(T), TRANSFER_TYPE::EXPEDITED, SIZE_INDICATOR::INDICATED, index, subindex, val); };

    template<typename T>
    inline SDO WRITE_VAR(uint16_t index, uint8_t subindex, T val) { return SDO_UNION(CCS::INITIATE_DOWNLAOD, 4 - sizeof(T), TRANSFER_TYPE::EXPEDITED, SIZE_INDICATOR::INDICATED, index, subindex, val); };
    
    inline SDO CONFIRM_WRITE(uint16_t index, uint8_t subindex) { return SDO_UNION(CCS::SEGMENT_DOWNLOAD, 0, (TRANSFER_TYPE) 0, (SIZE_INDICATOR) 0, index, subindex, 0); };
}

// Wheel speed command message
#define CAN_SCALE_SPEED (400.0/INT16_MAX)
struct COMMAND {
    int16_t speeds[4];
};
static_assert(sizeof(COMMAND) <= 8, "COMMAND exceeds maximum size");
// *******

// Motor Status message
#define CAN_SCALE_TEMP (200.0/INT8_MAX)
#define CAN_SCALE_BATV (50.0/INT8_MAX)
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
