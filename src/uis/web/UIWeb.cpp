#include "UIWeb.hpp"
#include "ILogger.hpp"
#include "IHistoricoService.hpp"
#include "Disciplina.hpp"
#include "Errors.hpp"
#include "json.hpp"

#include <cstring>
#include <cctype>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>


#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #define CLOSESOCK closesocket
  #pragma comment(lib, "Ws2_32.lib")
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #define SOCKET int
  #define INVALID_SOCKET (-1)
  #define CLOSESOCK ::close
#endif

using Json = nlohmann::json;

// --- helpers JSON <-> Disciplina (ajuste campos conforme sua classe) ---
static Json disciplinaToJson(const Disciplina& d){
    Json j;
    j["id"]        = d.getId();
    j["matricula"] = d.getMatricula();
    j["nome"]      = d.getNome();
    j["creditos"]  = d.getCreditos();
    j["ano"]       = d.getAno();
    j["semestre"]  = d.getSemestre();
    j["nota1"]     = d.getNota1();
    j["nota2"]     = d.getNota2();
    j["media"]     = d.getMedia();
    return j;
}
static Disciplina disciplinaFromJson(const Json& j){
    Disciplina d;
    d.setMatricula(j.at("matricula").get<std::string>());
    d.setNome(j.at("nome").get<std::string>());
    d.setCreditos(j.at("creditos").get<int>());
    d.setAno(j.at("ano").get<int>());
    d.setSemestre(j.at("semestre").get<int>());
    d.setNota1(j.at("nota1").get<double>());
    d.setNota2(j.at("nota2").get<double>());
    return d;
}

// ----------------------------------------------------------------------

UIWeb::UIWeb(IHistoricoService& s, ILogger& lg, const Configuracao& conf)
    : svc_(s), log(lg), port_(conf.getWebPort()), docroot_(std::move(conf.getWebPathRoot())) {
    }

void UIWeb::run() { serveLoop(); }

void UIWeb::serveLoop(){
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    SOCKET srv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (srv == INVALID_SOCKET) {
        LOG_ERR("UIWeb: socket() falhou");
        return;
    }
    int yes = 1;
#ifdef _WIN32
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
#else
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((uint16_t)port_);
#ifdef _WIN32
    InetPtonA(AF_INET, "127.0.0.1", &addr.sin_addr);
#else
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
#endif
    if (::bind(srv, (sockaddr*)&addr, sizeof(addr)) != 0) {
        LOG_ERR("UIWeb: bind() falhou (porta em uso?)");
        CLOSESOCK(srv);
        return;
    }
    if (::listen(srv, 1) != 0) {
        LOG_ERR("UIWeb: listen() falhou");
        CLOSESOCK(srv);
        return;
    }
    LOG_INF("UIWeb ouvindo em http://127.0.0.1:", port_);

    // mono-usuário: 1 conexão por vez; 1 request por conexão; fecha
    for(;;){
        SOCKET cli = ::accept(srv, nullptr, nullptr);
        if (cli == INVALID_SOCKET) continue;
        handleClient((int)cli);
        CLOSESOCK(cli);
    }

    CLOSESOCK(srv);
#ifdef _WIN32
    WSACleanup();
#endif
}

void UIWeb::handleClient(int sock){
    // lê até os headers (CRLFCRLF) e, se houver Content-Length, lê o body
    std::string data; data.reserve(8192);
    char buf[2048]; int n;
    // leitura bloqueante básica
    while ((n = ::recv(sock, buf, sizeof(buf), 0)) > 0) {
        data.append(buf, buf + n);
        if (data.find("\r\n\r\n") != std::string::npos) break;
    }
    if (data.empty()) return;

    // se tiver Content-Length, completa o body
    auto hdrEnd = data.find("\r\n\r\n");
    std::string headers = data.substr(0, hdrEnd+4);
    std::string body    = (hdrEnd != std::string::npos) ? data.substr(hdrEnd+4) : "";

    size_t clpos = headers.find("Content-Length:");
    size_t need = 0;
    if (clpos != std::string::npos) {
        auto lineEnd = headers.find("\r\n", clpos);
        auto val = headers.substr(clpos + 15, lineEnd - (clpos + 15));
        need = (size_t)std::stoul(val);
    }
    while (body.size() < need) {
        n = ::recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) break;
        body.append(buf, buf + n);
    }

    handleRequest(data, sock);
}

static inline std::string trim(const std::string& s){
    size_t a=0,b=s.size();
    while(a<b && (unsigned char)s[a]<=32) ++a;
    while(b>a && (unsigned char)s[b-1]<=32) --b;
    return s.substr(a,b-a);
}

void UIWeb::handleRequest(const std::string& req, int sock){
    // request line
    auto eol = req.find("\r\n");
    if (eol == std::string::npos) return;
    std::string line = req.substr(0, eol);
    std::istringstream iss(line);
    std::string method, path, httpver; iss >> method >> path >> httpver;
    if (path.empty()) path = "/";

    // body (já extraído em handleClient)
    auto hdrEnd = req.find("\r\n\r\n");
    std::string body = (hdrEnd != std::string::npos) ? req.substr(hdrEnd+4) : "";
    // normaliza path
    path = urlDecode(path);
    if (path.find("..") != std::string::npos) { // evita traversal
        auto r = httpResponse(400,"Bad Request","text/plain","bad path");
        ::send(sock, r.data(), (int)r.size(), 0);
        return;
    }

    if (isApi(path)) {
        handleApi(method, path, body, sock);
    } else {
        handleStatic(path, sock);
    }
}

bool UIWeb::isApi(const std::string& path) const {
    return path.rfind("/api/", 0) == 0;
}

void UIWeb::handleApi(const std::string& method, const std::string& path,
                      const std::string& body, int sock)
{
    try {
        // rotas:
        // GET  /api/disciplinas
        // GET  /api/disciplinas/{id}
        // POST /api/disciplinas
        // PUT  /api/disciplinas/{id}
        // DELETE /api/disciplinas/{id}
        // GET  /api/cr

        if (path == "/api/disciplinas" && method == "GET") {
            Json arr = Json::array();
            for (auto& d : svc_.list()) arr.push_back(disciplinaToJson(d));
            auto r = httpJson(200,"OK", arr.dump());
            ::send(sock, r.data(), (int)r.size(), 0);
            return;
        }

        if (path.rfind("/api/disciplinas/", 0) == 0) {
            std::string tail = path.substr(std::string("/api/disciplinas/").size());
            if (method == "GET") {
                int id = std::stoi(tail);
                auto d = svc_.get(id);
                auto r = httpJson(200,"OK", disciplinaToJson(d).dump());
                ::send(sock, r.data(), (int)r.size(), 0);
                return;
            } else if (method == "PUT") {
                int id = std::stoi(tail);
                Json j = Json::parse(body);
                auto d = disciplinaFromJson(j);
                svc_.update(id, d);
                auto r = httpJson(200,"OK", R"({"ok":true})");
                ::send(sock, r.data(), (int)r.size(), 0);
                return;
            } else if (method == "DELETE") {
                int id = std::stoi(tail);
                svc_.remove(id);
                auto r = httpJson(200,"OK", R"({"ok":true})");
                ::send(sock, r.data(), (int)r.size(), 0);
                return;
            }
        }

        if (path == "/api/disciplinas" && method == "POST") {
            Json j = Json::parse(body);
            Disciplina d = disciplinaFromJson(j);
            int id = svc_.insert(d);
            Json resp; resp["id"] = id;
            auto r = httpJson(201,"Created", resp.dump());
            ::send(sock, r.data(), (int)r.size(), 0);
            return;
        }

        if (path == "/api/cr" && method == "GET") {
            double cr = svc_.calculateCR();
            Json j; j["cr"] = cr;
            auto r = httpJson(200,"OK", j.dump());
            ::send(sock, r.data(), (int)r.size(), 0);
            return;
        }

        auto r = httpJson(404,"Not Found", R"({"error":"rota nao encontrada"})");
        ::send(sock, r.data(), (int)r.size(), 0);
    }
    catch (const BusinessError& e){
        Json j; j["error"] = e.what();
        auto r = httpJson(400,"Bad Request", j.dump());
        ::send(sock, r.data(), (int)r.size(), 0);
    }
    catch (const InfraError& e){
        Json j; j["error"] = "infra";
        auto r = httpJson(500,"Internal Server Error", j.dump());
        ::send(sock, r.data(), (int)r.size(), 0);
    }
    catch (const std::exception& e){
        Json j; j["error"] = "unexpected";
        auto r = httpJson(500,"Internal Server Error", j.dump());
        ::send(sock, r.data(), (int)r.size(), 0);
    }
}

void UIWeb::handleStatic(const std::string& path, int sock){
    std::string p = path;
    if (p == "/") p = "/index.html";
    std::string full = docroot_ + p;
    std::string body = readFile(full);
    if (body.empty()) {
        auto r = httpResponse(404,"Not Found","text/plain","404 not found");
        ::send(sock, r.data(), (int)r.size(), 0);
        return;
    }
    auto ct = guessMime(p);
    auto r = httpResponse(200,"OK", ct, body);
    ::send(sock, r.data(), (int)r.size(), 0);
}

std::string UIWeb::urlDecode(const std::string& s){
    std::string out; out.reserve(s.size());
    for (size_t i=0;i<s.size();++i){
        if (s[i]=='%' && i+2<s.size()){
            int v = 0;
            std::istringstream is(s.substr(i+1,2));
            is >> std::hex >> v;
            out.push_back((char)v);
            i+=2;
        } else if (s[i]=='+') {
            out.push_back(' ');
        } else out.push_back(s[i]);
    }
    return out;
}

std::string UIWeb::guessMime(const std::string& path){
    auto dot = path.find_last_of('.');
    std::string ext = (dot==std::string::npos) ? "" : path.substr(dot+1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext=="html") return "text/html; charset=utf-8";
    if (ext=="js")   return "application/javascript; charset=utf-8";
    if (ext=="css")  return "text/css; charset=utf-8";
    if (ext=="png")  return "image/png";
    if (ext=="jpg" || ext=="jpeg") return "image/jpeg";
    if (ext=="ico")  return "image/x-icon";
    if (ext=="json") return "application/json; charset=utf-8";
    return "text/plain; charset=utf-8";
}

std::string UIWeb::readFile(const std::string& path){
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    std::ostringstream ss; ss << f.rdbuf();
    return ss.str();
}

std::string UIWeb::httpResponse(int status, const std::string& statusText,
                                const std::string& contentType,
                                const std::string& body)
{
    std::ostringstream ss;
    ss << "HTTP/1.1 " << status << ' ' << statusText << "\r\n";
    ss << "Content-Type: " << contentType << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << "Connection: close\r\n";
    ss << "Cache-Control: no-store\r\n";
    ss << "\r\n";
    ss << body;
    return ss.str();
}
std::string UIWeb::httpJson(int status, const std::string& statusText,
                            const std::string& jsonBody)
{
    return httpResponse(status, statusText, "application/json; charset=utf-8", jsonBody);
}
