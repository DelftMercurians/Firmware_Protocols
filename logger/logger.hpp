#pragma once
#include <cstdio>
#include <string>
#include <utility>

class Logger {
    public:
        enum class LogLevel {
            NONE = 0x0,
            ERROR = 0x1,
            WARN = 0x2,
            INFO = 0x3,
            DEBUG = 0x4
        };

        template<typename... Args>
        void error(const char *source, const char *fmt, Args&&... args) {
            log(LogLevel::ERROR, source, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void warn(const char *source, const char *fmt, Args&&... args) {
            log(LogLevel::WARN, source, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void info(const char *source, const char *fmt, Args&&... args) {
            log(LogLevel::INFO, source, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void debug(const char *source, const char *fmt, Args&&... args) {
            log(LogLevel::DEBUG, source, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void log(LogLevel level, const char *source, const char *fmt, Args&&... args) {
            int size = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
            if (size < 0) {
                log_text(level, "[log format error]");
                return;
            }
            std::string text;
            text.resize(static_cast<size_t>(size));
            std::snprintf(text.data(), static_cast<size_t>(size) + 1, fmt, std::forward<Args>(args)...);

            const char *level_tag = "[unknown] ";
            switch (level) {
                case LogLevel::ERROR: level_tag = "[error] "; break;
                case LogLevel::WARN:  level_tag = "[warn] "; break;
                case LogLevel::INFO:  level_tag = "[info] "; break;
                case LogLevel::DEBUG: level_tag = "[debug] "; break;
                default: break;
            }

            // Construct the final formatted string: [level] (source) message
            std::string source_tag = "(";
            source_tag += source ? source : "(unknown)";
            source_tag += ")";

            text.insert(0, level_tag);
            text += " ";
            text += source_tag;
            text += " ";

            log_text(level, text);
        }

        virtual ~Logger() = default;

    protected:
        virtual void log_text(LogLevel level, const std::string &text) = 0;
};