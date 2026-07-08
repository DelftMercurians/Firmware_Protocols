#pragma once

#include <logger/logger.hpp>

class DefaultLogger : public Logger {
    protected:
        void log_text(LogLevel level, const std::string &text) override;
};
