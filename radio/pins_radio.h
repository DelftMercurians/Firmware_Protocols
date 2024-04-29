#pragma once
#include "../utils.h"
#include "../can/protocols_can.h"

namespace RadioPins {

enum class SpiBus {
    Spi_1 = 1,
    Spi_2 = 2,
};
struct RadioPins {
    int ce;
    int cs;
    SpiBus spi_bus;
};
const RadioPins RobotPinMap = { PB0, PA4, SpiBus::Spi_2 };

#ifdef BASE_STATION_MULTI_RADIO
#define __RADIO_PINS_NUMBER_OF_RADIOS 4
const RadioPins GroupPinMap[] = {
    { PB0, PA3, SpiBus::Spi_1 },
    { PB11, PA1, SpiBus::Spi_2 },
    { PB1, PA4, SpiBus::Spi_1 },
    { PB10, PA0, SpiBus::Spi_2 },
};
#else
#define __RADIO_PINS_NUMBER_OF_RADIOS 1
const RadioPins GroupPinMap[] = {
    { PB0, PA4, SpiBus::Spi_1 },
};
#endif

const uint8_t NumberOfRadios = __RADIO_PINS_NUMBER_OF_RADIOS;
static_assert(NumberOfRadios == (sizeof(GroupPinMap)/sizeof(GroupPinMap[0])));

}