#pragma once
#include <map>
#include "../utils.h"

namespace CAN {
// Maps actual hardware UIDs to device ID
// Mapping may be done in other ways later
const std::map<uint32_t, DEVICE_ID> DEVICE_ID_MAP {
    {0x230034, DEVICE_ID::DRIVER_0},    // Moved due to blown board (2024-02-08 Thomas)
    {0x370033, DEVICE_ID::DRIVER_2},
    {0x220027, DEVICE_ID::DRIVER_3},

    {0x5C004D, DEVICE_ID::DRIVER_0}, 
    {0x56005C, DEVICE_ID::DRIVER_0},    // replacement board (2023-12-05 Thomas)
    {0x640046, DEVICE_ID::DRIVER_1},    // blown up CAN tranceiver
    {0x560062, DEVICE_ID::DRIVER_1},    // replacement board (2023-??-?? Thomas)
    {0x670039, DEVICE_ID::DRIVER_1},    // replacement board nr 2 (2023-11-?? Nianlei)
    {0x560044, DEVICE_ID::DRIVER_2},
    {0x32003C, DEVICE_ID::DRIVER_2},    // replacement board (2023-12-04 Tim)
    {0x320026, DEVICE_ID::DRIVER_3},

    {0x600040, DEVICE_ID::DRIVER_0},    // 600040 534B5002 20343932 New board (2024-02-03 Thomas)
    {0x460058, DEVICE_ID::DRIVER_1},    // 460058 534B5002 20343932 New board (2024-02-03 Thomas)
    {0x630062, DEVICE_ID::DRIVER_2},    // 630062 534B5002 20343932 Newish board (2024-02-05 Alex)
    // {0x630052, DEVICE_ID::DRIVER_3},    // 630052 534B5002 20343932 New board (2024-02-05 Alex) // RIP
    {0x200034, DEVICE_ID::DRIVER_3},    // 200034 5056500D 20313856 Moved from old old (2024-02-11 Thomas)
    
    {0x34004A, DEVICE_ID::DRIVER_A},    // 34004A 534B5002 20343932 Dribbler (2024-02-02 Thomas)
    {0x290039, DEVICE_ID::DRIVER_A},    // 290039 534B5002 20343932 Dribbler (2024-02-03 Alex)
    {0x670042, DEVICE_ID::DRIVER_A},    // 670042 534B5002 20343932 Dribbler (2024-02-10 Alex)
};
}