#ifndef _UI_WEB_HPP_
#define _UI_WEB_HPP_

#include <string>
#include "IUserInterface.hpp"
#include "Configuracao.hpp"
#include "IHistoricoService.hpp"
#include "ILogger.hpp"

class UIWeb : public IUserInterface {
public:
    UIWeb(IHistoricoService& service, ILogger& logger, const Configuracao& conf);
    void run() override;

private:
    IHistoricoService& svc_;
    ILogger& log;
    int port_;
    std::string docroot_;

    void serveLoop();
    void handleClient(int sock);
    void handleRequest(const std::string& req, int sock);
    bool isApi(const std::string& path) const;
    void handleApi(const std::string& method, const std::string& path,
                   const std::string& body, int sock);
    void handleStatic(const std::string& path, int sock);

    static std::string urlDecode(const std::string& s);
    static std::string guessMime(const std::string& path);
    static std::string httpResponse(int status, const std::string& statusText,
                                    const std::string& contentType,
                                    const std::string& body);
    static std::string httpJson(int status, const std::string& statusText,
                                const std::string& jsonBody);
    static std::string readFile(const std::string& path);
};

#endif