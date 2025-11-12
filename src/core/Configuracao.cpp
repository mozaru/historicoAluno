#include "Configuracao.hpp"
#include "Uteis.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;



Configuracao::Configuracao(int argc,
                           char* argv[],
                           const std::string& configFilePath)
    : verbose(false)                    , verboseDefinido(false)
    , connectionString("dados.sqlite")  , connectionStringDefinida(false)
    , fileName("dados.bin")             , fileNameDefinida(false)
    , logPath("")                       , logPathDefinida(false)
    , logPrefix("log_historico_")       , logPrefixDefinida(false)
    , logType("file")                   , logTypeDefinida(false) 
    , webPathRoot("www")                , webPathRootDefinida(false)
    , webPort(9090)                     , webPortDefinida(false)
{
    // 1) Ambiente
    carregarDeAmbiente();

    // 2) Arquivo
    carregarDeArquivo(configFilePath);

    // 3) Argumentos
    carregarDeArgumentos(argc, argv);
}

bool Configuracao::isVerbose() const
{
    return verbose;
}

const std::string& Configuracao::getConnectionString() const
{
    return connectionString;
}

const std::string& Configuracao::getFileName() const
{
    return fileName;
}

std::string Configuracao::getFileName(const std::string& extencao) const
{
    return changeExtension(fileName,extencao);
}

const std::string&  Configuracao::getLogPath() const
{
    return logPath;
}
const std::string&  Configuracao::getLogPrefix() const
{
    return logPrefix;
}
const std::string&  Configuracao::getLogType() const
{
    return logType;
}
const std::string&  Configuracao::getWebPathRoot() const
{
    return webPathRoot;
}
int Configuracao::getWebPort() const
{
    return webPort;
}

// --------------------------------------------
// Fonte: Ambiente
// --------------------------------------------

void Configuracao::carregarDeAmbiente()
{
    // VERBOSE
    if (!verboseDefinido)
    {
        if (const char* v = std::getenv("VERBOSE"))
        {
            bool ok = false;
            bool b = parseBool(v, ok);
            if (ok)
            {
                verbose = b;
                verboseDefinido = true;
            }
        }
    }

    // CONNECTION_STRING / CONECTION_STRING (aceita ambas)
    if (!connectionStringDefinida)
    {
        const char* v = std::getenv("CONNECTION_STRING");
        if (!v)
            v = std::getenv("CONECTION_STRING"); // tolera typo

        if (v && *v)
        {
            connectionString = v;
            connectionStringDefinida = true;
        }
    }

    if (!fileNameDefinida)
    {
        const char* v = std::getenv("FILE_NAME");
        if (!v)
            v = std::getenv("FILE_NAME"); // tolera typo

        if (v && *v)
        {
            fileName = v;
            fileNameDefinida = true;
        }
    }
    if (!logTypeDefinida)
    {
        const char* v = std::getenv("LOG_TYPE");
        if (!v)
            v = std::getenv("LOG_TYPE"); // tolera typo

        if (v && *v)
        {
            logType = v;
            logTypeDefinida = true;
        }
    }
    if (!logPathDefinida)
    {
        const char* v = std::getenv("LOG_PATH");
        if (!v)
            v = std::getenv("LOG_PATH"); // tolera typo

        if (v && *v)
        {
            logPath = v;
            logPathDefinida = true;
        }
    }
    if (!logPrefixDefinida)
    {
        const char* v = std::getenv("LOG_PREFIX");
        if (!v)
            v = std::getenv("LOG_PREFIX"); // tolera typo

        if (v && *v)
        {
            logPrefix = v;
            logPrefixDefinida = true;
        }
    }

    if (!webPathRootDefinida)
    {
        const char* v = std::getenv("WEB_PATH_ROOT");
        if (!v)
            v = std::getenv("WEB_PATH_ROOT"); // tolera typo

        if (v && *v)
        {
            webPathRoot = v;
            webPathRootDefinida = true;
        }
    }
    if (!webPortDefinida)
    {
        const char* v = std::getenv("WEB_PORT");
        if (!v)
            v = std::getenv("WEB_PORT"); // tolera typo

        if (v && *v)
        {
            webPort = std::stoi( v );
            webPortDefinida = true;
        }
    }
}

// --------------------------------------------
// Fonte: Arquivo
// --------------------------------------------

void Configuracao::carregarDeArquivo(const std::string& configFilePath)
{
    if (configFilePath.empty())
        return;

    ifstream in(configFilePath);
    if (!in)
        return; // silencioso: arquivo é opcional

    string line;
    while (std::getline(in, line))
    {
        line = trim(line);
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;

        auto pos = line.find('=');
        if (pos == string::npos)
            continue;

        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));

        if (key.empty())
            continue;

        aplicarPar(key, value);
    }
}

// --------------------------------------------
// Fonte: Argumentos
// --------------------------------------------

void Configuracao::carregarDeArgumentos(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg.rfind("--", 0) == 0)
            arg = arg.substr(2);

        auto pos = arg.find('=');
        if (pos == string::npos)
            continue;

        string key = trim(arg.substr(0, pos));
        string value = trim(arg.substr(pos + 1));
        if (key.empty())
            continue;

        aplicarPar(key, value);
    }
}

// --------------------------------------------
// Utilitários
// --------------------------------------------

bool Configuracao::parseBool(const std::string& value, bool& ok)
{
    string v = toUpper(trim(value));
    if (v == "YES" || v == "TRUE" || v == "1")
    {
        ok = true;
        return true;
    }
    if (v == "NO" || v == "FALSE" || v == "0")
    {
        ok = true;
        return false;
    }

    ok = false;
    return false;
}

void Configuracao::aplicarPar(const std::string& chave, const std::string& valor)
{
    string keyUpper = toUpper(chave);

    if (keyUpper == "VERBOSE" && !verboseDefinido)
    {
        bool ok = false;
        bool b = parseBool(valor, ok);
        if (ok)
        {
            verbose = b;
            verboseDefinido = true;
        }
    }
    else if (keyUpper == "CONNECTION_STRING" && !connectionStringDefinida)
    {
        if (!valor.empty())
        {
            connectionString = valor;
            connectionStringDefinida = true;
        }
    }
    else if (keyUpper == "FILE_NAME" && !fileNameDefinida)
    {
        if (!valor.empty())
        {
            fileName = valor;
            fileNameDefinida = true;
        }
    }
    else if (keyUpper == "LOG_TYPE" && !logTypeDefinida)
    {
        if (!valor.empty())
        {
            logType = valor;
            logTypeDefinida = true;
        }
    }
    else if (keyUpper == "LOG_PREFIX" && !logPrefixDefinida)
    {
        if (!valor.empty())
        {
            logPrefix = valor;
            logPrefixDefinida = true;
        }
    }
    else if (keyUpper == "LOG_PATH" && !logPathDefinida)
    {
        if (!valor.empty())
        {
            logPath = valor;
            logPathDefinida = true;
        }
    }
    else if (keyUpper == "WEB_PATH_ROOT" && !webPathRootDefinida)
    {
        if (!valor.empty())
        {
            webPathRoot = valor;
            webPathRootDefinida = true;
        }
    }
    else if (keyUpper == "WEB_PORT" && !webPortDefinida)
    {
        if (!valor.empty())
        {
            webPort = std::stoi(valor);
            webPortDefinida = true;
        }
    }
}
