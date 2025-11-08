#ifndef _ILOGGER_HPP_
#define _ILOGGER_HPP_

#include <sstream>
#include <string>
#include <utility>
// Interface de logging do sistema.
//
// Níveis (interpretação padrão):
// - INFO: mensagens informativas de alto nível.
// - DEBUG: detalhes de fluxo, validações, decisões internas.
// - ERROR: falhas relevantes (infraestrutura, exceções não tratadas no negócio).
//
// Política sugerida (a ser aplicada pelas implementações / configuração):
// - Modo "info": exibe INFO + DEBUG + ERROR.
// - Modo "debug": exibe DEBUG + ERROR.
// - Modo "error": exibe apenas ERROR.
//
// Uso recomendado:
// - Erros de negócio esperados: DEBUG (para rastrear).
// - Erros de infra ou inesperados: ERROR.
// - Eventos relevantes de fluxo ou operações concluídas: INFO.

class ILogger {
    public:
        virtual ~ILogger() = default;

        virtual void logInfo(const std::string& message) = 0;
        virtual void logDebug(const std::string& message) = 0;
        virtual void logError(const std::string& message) = 0;
};

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define STR(...) str_concat(__VA_ARGS__)

#define LOG_INF(...) log.logInfo(std::string("[" __FILE__ ", " STRINGIZE(__LINE__) ", ") + __func__ + "] " + str_concat(__VA_ARGS__));
#define LOG_ERR(...) log.logError(std::string("[" __FILE__ ", " STRINGIZE(__LINE__) ", ") + __func__ + "] " +str_concat(__VA_ARGS__));

#ifdef NDEBUG
    #define LOG_DBG(...)  /* no-op */
#else
    #define LOG_DBG(...) log.logDebug(std::string("[" __FILE__ ", " STRINGIZE(__LINE__) ", ") + __func__ + "] " + str_concat(__VA_ARGS__));
#endif    

template <typename... Args>
std::string str_concat(Args&&... args) {
    std::ostringstream oss;
    (oss << ... << std::forward<Args>(args)); 
    return oss.str();
}


#endif