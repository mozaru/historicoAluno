#include "ConsoleLogger.hpp"
#include <iostream>

namespace {
    void write_log(std::ostream& os, const char* level, const std::string& message) {
        os << "[" << level << "] " << message << std::endl;
    }
}

ConsoleLogger::ConsoleLogger(const Configuracao& conf) : verbose(conf.isVerbose())
{}

void ConsoleLogger::logInfo(const std::string& message) {
    if (verbose)
        write_log(std::cout, "INFO", message);
}

void ConsoleLogger::logDebug(const std::string& message) {
    if (verbose)
        write_log(std::cout, "DEBUG", message);
}

void ConsoleLogger::logError(const std::string& message) {
    write_log(std::cerr, "ERROR", message);
}
