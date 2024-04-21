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
const RadioPins RobotPinMap = { PB0, PA4, SpiBus::Spi_1 };
const std::map<uint8_t, RadioPins> GroupPinMap = {
    { 0, { PB0, PA3, SpiBus::Spi_1 }},
    { 1, { PB1, PA4, SpiBus::Spi_1 }},
    { 2, { PB10, PA11, SpiBus::Spi_2 }},
    { 3, { PB11, PA12, SpiBus::Spi_2 }},
};

}