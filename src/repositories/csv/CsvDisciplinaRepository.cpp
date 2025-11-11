#include "CsvDisciplinaRepository.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <iomanip>

#include "Errors.hpp"
#include "Uteis.hpp"

using namespace std;

// --------------------------------------------------------
// Construtor / destrutor
// --------------------------------------------------------

CsvDisciplinaRepository::CsvDisciplinaRepository(ILogger& aLog,
                                                 const Configuracao& conf)
    : log(aLog)
{
    // Usa diretamente a connection string.
    // Se quiser forçar extensão .csv, pode fazer:
    // filename = changeExtension(conf.getConnectionString(), "csv");
    filename = conf.getFileName("csv");

    LOG_INF("CsvDisciplinaRepository arquivo=", filename);
}

CsvDisciplinaRepository::~CsvDisciplinaRepository() = default;

// --------------------------------------------------------
// Helpers CSV básicos (sem libs externas)
// --------------------------------------------------------

std::vector<std::string> CsvDisciplinaRepository::splitCsv(const std::string& line)
{
    std::vector<std::string> cols;
    std::string current;

    for (char c : line)
    {
        if (c == ',')
        {
            cols.push_back(trim(current));
            current.clear();
        }
        else
        {
            current.push_back(c);
        }
    }

    cols.push_back(trim(current));
    return cols;
}

std::string CsvDisciplinaRepository::disciplinaToCsv(const Disciplina& d)
{
    std::ostringstream oss;

    // Não precisa escapar vírgula; assumido pelo domínio da aplicação.
    oss << d.getMatricula() << ','
        << d.getNome()      << ','
        << d.getSemestre()  << ','
        << d.getAno()       << ','
        << d.getCreditos()  << ',';

    // notas com 2 casas decimais
    oss << fixed << setprecision(2) << d.getNota1() << ','
        << fixed << setprecision(2) << d.getNota2();

    return oss.str();
}

Disciplina CsvDisciplinaRepository::csvToDisciplina(const std::string& line, int id)
{
    auto cols = splitCsv(line);

    if (cols.size() != 7)
        throw ConversionError("Linha CSV invalida: quantidade de colunas diferente de 7.");

    Disciplina d;
    d.setId(id);
    d.setMatricula(cols[0]);
    d.setNome(cols[1]);

    try
    {
        d.setSemestre(std::stoi(cols[2]));
        d.setAno(std::stoi(cols[3]));
        d.setCreditos(std::stoi(cols[4]));
        d.setNota1(std::stod(cols[5]));
        d.setNota2(std::stod(cols[6]));
    }
    catch (...)
    {
        throw ConversionError("Falha ao converter campos numericos do CSV de disciplina.");
    }

    return d;
}

// --------------------------------------------------------
// Contagem e leitura de linha por ID (id = numero da linha, 1-based)
// --------------------------------------------------------

int CsvDisciplinaRepository::getRecordCount() const
{
    std::ifstream in(filename);
    if (!in.good())
        return 0;

    int count = 0;
    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty())
            ++count;
    }

    return count;
}

bool CsvDisciplinaRepository::readLineById(int id, std::string& outLine) const
{
    if (id <= 0)
        return false;

    std::ifstream in(filename);
    if (!in)
        return false;

    std::string line;
    int current = 0;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        ++current;
        if (current == id)
        {
            outLine = line;
            return true;
        }
    }

    return false;
}

// --------------------------------------------------------
// Métodos do repositório
// --------------------------------------------------------

Disciplina CsvDisciplinaRepository::get(int id) const
{
    LOG_DBG("csv.get id=", id);

    if (id <= 0)
        throw InfraError("Id invalido para leitura de disciplina (id=" + std::to_string(id) + ")");

    std::string line;
    if (!readLineById(id, line))
        throw InfraError("Disciplina nao encontrada (id=" + std::to_string(id) + ")");

    Disciplina d = csvToDisciplina(line, id);

    LOG_DBG("csv.get ok id=", id, " nome=", d.getNome());
    return d;
}

int CsvDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("csv.insert nome=", disciplina.getNome());

    // Garantir que o arquivo exista (modo append já cria, mas se quiser tratar erro):
    std::ofstream out(filename, std::ios::app);
    if (!out)
        throw InfraError("Falha ao abrir arquivo CSV para escrita.");

    std::string line = disciplinaToCsv(disciplina);

    // Se arquivo não está vazio e não termina com newline, std::ofstream em texto já cuida,
    // mas vamos sempre adicionar newline explícito.
    out << line << '\n';
    if (!out)
        throw InfraError("Falha ao gravar disciplina no arquivo CSV.");

    int total = getRecordCount();
    LOG_DBG("csv.insert ok id=", total);
    return total; // novo id é ultima linha
}

void CsvDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("csv.update id=", id, " novo_nome=", disciplina.getNome());

    if (id <= 0)
        throw InfraError("Id invalido para atualizacao (id=" + std::to_string(id) + ")");

    std::ifstream in(filename);
    if (!in)
        throw InfraError("Falha ao abrir arquivo CSV para leitura em update.");

    std::vector<std::string> lines;
    lines.reserve(128);

    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    if (id > static_cast<int>(lines.size()))
        throw InfraError("Disciplina nao encontrada para atualizacao (id=" + std::to_string(id) + ")");

    // Substitui linha correspondente
    lines[static_cast<size_t>(id - 1)] = disciplinaToCsv(disciplina);

    // Regrava arquivo completo
    std::ofstream out(filename, std::ios::trunc);
    if (!out)
        throw InfraError("Falha ao abrir arquivo CSV para escrita em update.");

    for (const auto& l : lines)
        out << l << '\n';

    if (!out)
        throw InfraError("Falha ao gravar arquivo CSV em update.");

    LOG_DBG("csv.update ok id=", id);
}

void CsvDisciplinaRepository::remove(int id)
{
    LOG_DBG("csv.remove id=", id);

    if (id <= 0)
        throw InfraError("Id invalido para remocao (id=" + std::to_string(id) + ")");

    std::ifstream in(filename);
    if (!in)
        throw InfraError("Falha ao abrir arquivo CSV para leitura em remove.");

    std::vector<std::string> lines;
    lines.reserve(128);

    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }

    int total = static_cast<int>(lines.size());
    if (id > total || total == 0)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");

    if (total > 1 && id != total)
    {
        // Swap com ultima para manter a regra id = numero da linha compacta
        lines[static_cast<size_t>(id - 1)] = lines.back();
    }

    lines.pop_back();

    std::ofstream out(filename, std::ios::trunc);
    if (!out)
        throw InfraError("Falha ao abrir arquivo CSV para escrita em remove.");

    for (const auto& l : lines)
        out << l << '\n';

    if (!out)
        throw InfraError("Falha ao gravar arquivo CSV em remove.");

    LOG_DBG("csv.remove ok id=", id, " total_antigo=", total, " total_novo=", total - 1);
}

std::vector<Disciplina> CsvDisciplinaRepository::list() const
{
    LOG_DBG("csv.list");

    std::vector<Disciplina> out;
    std::ifstream in(filename);
    if (!in)
    {
        LOG_DBG("csv.list arquivo inexistente, retornando vazio");
        return out;
    }

    std::string line;
    int id = 0;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        ++id;
        out.push_back(csvToDisciplina(line, id));
    }

    LOG_DBG("csv.list retornou=", out.size());
    return out;
}

bool CsvDisciplinaRepository::exist(const std::string& matricula,
                                    int ano,
                                    int semestre) const
{
    LOG_DBG("csv.exist matricula=", matricula,
            " ano=", ano, " semestre=", semestre);

    std::ifstream in(filename);
    if (!in)
        return false;

    std::string line;
    int id = 0;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        ++id;
        auto d = csvToDisciplina(line, id);
        if (d.getMatricula() == matricula &&
            d.getAno()       == ano &&
            d.getSemestre()  == semestre)
        {
            LOG_DBG("csv.exist true id=", id);
            return true;
        }
    }

    LOG_DBG("csv.exist false");
    return false;
}

bool CsvDisciplinaRepository::exist(int id) const
{
    LOG_DBG("csv.exist(id) id=", id);
    int total = getRecordCount();
    bool ok = (id > 0 && id <= total);
    LOG_DBG(ok ? "true" : "false");
    return ok;
}
