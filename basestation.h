// Delft Mercurians
// Thomas Hettasch
// 2024-05-24
// thomas.hettasch@gmail.com

#pragma once
#include <stdint.h>
#include "utils.h"

namespace Base {

struct Information {
    HG::Version version;
    uint8_t num_radios;
    uint8_t max_robots;
    uint16_t radios_online;  // bitfield
    uint8_t channel;
};

}; // namespace Base