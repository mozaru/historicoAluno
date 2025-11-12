#ifndef _CONFIGURACAO_HPP_
#define _CONFIGURACAO_HPP_

#include <string>

class Configuracao
{
public:
    // Cria configuração resolvendo:
    // 1. variáveis de ambiente
    // 2. arquivo (se existir)
    // 3. argumentos de linha de comando
    // nessa ordem, sem sobrescrever valores já definidos.
    Configuracao(int argc,
                 char* argv[],
                 const std::string& configFilePath = "app.config");

    bool isVerbose() const;
    const std::string& getConnectionString() const;
    const std::string& getFileName() const;
    std::string getFileName(const std::string& extencao) const;
    const std::string& getLogPath() const;
    const std::string& getLogPrefix() const;
    const std::string& getLogType() const;
    const std::string& getWebPathRoot() const;
    int getWebPort() const;

private:
    bool verbose;
    bool verboseDefinido;

    std::string connectionString;
    bool connectionStringDefinida;

    std::string fileName;
    bool fileNameDefinida;

    std::string logPath;
    bool logPathDefinida;

    std::string logPrefix;
    bool logPrefixDefinida;

    std::string logType;
    bool logTypeDefinida;

    std::string webPathRoot;
    bool webPathRootDefinida;

    int webPort;
    bool webPortDefinida;

    void carregarDeAmbiente();
    void carregarDeArquivo(const std::string& path);
    void carregarDeArgumentos(int argc, char* argv[]);

    void aplicarPar(const std::string& chave, const std::string& valor);
    static bool parseBool(const std::string& value, bool& ok);
};

#endif // CONFIGURACAO_HPP
