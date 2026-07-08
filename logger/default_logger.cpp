#include <logger/default_logger.hpp>

#include <Arduino.h>

void DefaultLogger::log_text(LogLevel level, const std::string &text) {
    (void) level;
    Serial.println(text.c_str());
}
