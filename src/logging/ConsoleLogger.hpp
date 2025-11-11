#ifndef _CONSOLELOGGER_HPP_
#define _CONSOLELOGGER_HPP_

#include "ILogger.hpp"
#include <string>
#include "Configuracao.hpp"

class ConsoleLogger : public ILogger {
    private:
        bool verbose;
    public:
        explicit ConsoleLogger(const Configuracao& conf);
        ~ConsoleLogger() override = default;

        void logInfo(const std::string& message) override;
        void logDebug(const std::string& message) override;
        void logError(const std::string& message) override;
};

#endif // _CONSOLELOGGER_HPP_
