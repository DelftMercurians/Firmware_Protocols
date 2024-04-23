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
const std::map<uint8_t, RadioPins> GroupPinMap = {
    { 0, { PB0, PA3, SpiBus::Spi_1 }},
    { 1, { PB11, PA1, SpiBus::Spi_2 }},
    { 2, { PB1, PA4, SpiBus::Spi_1 }},
    { 3, { PB10, PA0, SpiBus::Spi_2 }},
};

}