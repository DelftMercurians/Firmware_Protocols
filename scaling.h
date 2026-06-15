#pragma once

#include <stdint.h>

namespace Scale {

constexpr float WHEEL_SPEED = (400.0/INT16_MAX);
constexpr float MD_TEMP = (200.0/INT8_MAX);
constexpr float MD_BATV = 0.1;
constexpr float KICKER_VCAP = (260.0/UINT8_MAX);

constexpr float CURRENT = (50.0/INT16_MAX);

}
