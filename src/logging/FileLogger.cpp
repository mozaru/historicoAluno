#include "FileLogger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Uteis.hpp"

FileLogger::FileLogger(const Configuracao &conf)
    : verbose(conf.isVerbose())
{
    std::string path = conf.getLogPath();
    std::string prefix = conf.getLogPrefix();
    std::string filename = "";

    if (prefix.empty())
       prefix = "log_historico_";

    if (filename.empty()) {
        using namespace std::chrono;
        const auto now = system_clock::now();
        const std::time_t t = system_clock::to_time_t(now);
        std::tm tm{};

#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&tm, &t);
#endif

        std::ostringstream oss;
        oss << prefix
            << std::put_time(&tm, "%Y%m%d_%H%M%S")
            << ".log";

        filename = oss.str();
    }

    filename = joinPath(path,filename);

    file_.open(filename, std::ios::app);
}

FileLogger::~FileLogger() {
    if (file_.is_open()) {
        file_.flush();
        file_.close();
    }
}

bool FileLogger::isOpen() const noexcept {
    return file_.is_open();
}

void FileLogger::write(const char* level, const std::string& message) {
    if (!file_.is_open()) {
        return;
    }
    file_ << "[" << level << "] " << message << '\n';
    file_.flush();
}

void FileLogger::logInfo(const std::string& message) {
    if (verbose)
        write("INFO", message);
}

void FileLogger::logDebug(const std::string& message) {
    if (verbose)
        write("DEBUG", message);
}

void FileLogger::logError(const std::string& message) {
    write("ERROR", message);
}
