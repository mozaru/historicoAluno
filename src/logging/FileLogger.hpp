// FileLogger.hpp
#ifndef _FILELOGGER_HPP_
#define _FILELOGGER_HPP_

#include "ILogger.hpp"
#include <fstream>
#include <string>

class FileLogger : public ILogger {
    private:
        bool verbose;
        std::ofstream file_;

        void write(const char* level, const std::string& message);
    public:
        explicit FileLogger(bool aVerbose=false, const std::string& aFilename="");
        ~FileLogger() override;

        bool isOpen() const noexcept;

        void logInfo(const std::string& message) override;
        void logDebug(const std::string& message) override;
        void logError(const std::string& message) override;
};

#endif // _FILELOGGER_HPP_
