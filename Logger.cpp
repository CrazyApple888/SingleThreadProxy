#include "Logger.h"

void Logger::info(const std::string& tag, const std::string& message) {
    ostream << "[INFO] " + tag + ": " + message << std::endl;
}

Logger::Logger(bool isDebug, std::ostream &ostream) : isDebug(isDebug), ostream(ostream) {}

void Logger::debug(const std::string &tag, const std::string &message) {
    if (isDebug) {
        ostream << "[DEBUG] " + tag + ": " + message << std::endl;
    }
}
