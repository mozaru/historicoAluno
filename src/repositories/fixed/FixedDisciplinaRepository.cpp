#include "FixedDisciplinaRepository.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <iomanip>

#include "Errors.hpp"

using namespace std;

// --------------------------------------------------------
// Construtor / destrutor
// --------------------------------------------------------

FixedDisciplinaRepository::FixedDisciplinaRepository(ILogger& aLog,
                                                     const Configuracao& conf)
    : log(aLog)
{
    // Usa a connection string como nome de arquivo.
    // Se você tiver getFileName("fix"), pode trocar aqui.
    filename = conf.getFileName("txt");

    LOG_INF("FixedDisciplinaRepository arquivo=", filename);
}

FixedDisciplinaRepository::~FixedDisciplinaRepository() = default;

// --------------------------------------------------------
// Utils de string
// --------------------------------------------------------

std::string FixedDisciplinaRepository::padOrTrim(const std::string& s, int width)
{
    if ((int)s.size() >= width)
        return s.substr(0, width);

    std::string out = s;
    out.append(static_cast<std::size_t>(width - s.size()), ' ');
    return out;
}

std::string FixedDisciplinaRepository::rtrim(const std::string& s)
{
    int end = static_cast<int>(s.size());
    while (end > 0 && std::isspace(static_cast<unsigned char>(s[static_cast<size_t>(end - 1)])))
        --end;
    return s.substr(0, static_cast<size_t>(end));
}

// --------------------------------------------------------
// Conversão Disciplina <-> linha fixa
// --------------------------------------------------------

std::string FixedDisciplinaRepository::toLine(const Disciplina& d)
{
    std::ostringstream oss;

    // matricula (20, left, espaço)
    oss << padOrTrim(d.getMatricula(), MATRICULA_LEN);

    // nome (60, left, espaço)
    oss << padOrTrim(d.getNome(), NOME_LEN);

    // semestre (1)
    {
        int sem = d.getSemestre();
        if (sem < 0 || sem > 9)
            sem = 0;
        oss << static_cast<char>('0' + sem);
    }

    // ano (4, zero padded)
    {
        int ano = d.getAno();
        if (ano < 0)
            ano = 0;
        oss << setw(ANO_LEN) << setfill('0') << ano;
        oss << setfill(' ');
    }

    // creditos (2, zero padded)
    {
        int cred = d.getCreditos();
        if (cred < 0)
            cred = 0;
        if (cred > 99)
            cred = 99;
        oss << setw(CREDITOS_LEN) << setfill('0') << cred;
        oss << setfill(' ');
    }

    // nota1 (5, fixo, 2 casas)
    {
        double n1 = d.getNota1();
        if (n1 < 0.0) n1 = 0.0;
        oss << setw(NOTA_LEN) << setfill(' ') << fixed << setprecision(2) << n1;
    }

    // nota2 (5, fixo, 2 casas)
    {
        double n2 = d.getNota2();
        if (n2 < 0.0) n2 = 0.0;
        oss << setw(NOTA_LEN) << setfill(' ') << fixed << setprecision(2) << n2;
    }

    std::string line = oss.str();

    if ((int)line.size() != RECORD_NO_EOL_LEN)
        throw InfraError("Bug interno: tamanho de linha invalido em FixedDisciplinaRepository::toLine.");

    return line;
}

Disciplina FixedDisciplinaRepository::fromLine(const std::string& line, int id)
{
    if ((int)line.size() != RECORD_NO_EOL_LEN)
        throw InfraError("Linha com tamanho invalido em arquivo fixed.");

    int pos = 0;

    std::string matricula = rtrim(line.substr(pos, MATRICULA_LEN));
    pos += MATRICULA_LEN;

    std::string nome = rtrim(line.substr(pos, NOME_LEN));
    pos += NOME_LEN;

    char semChar = line[static_cast<size_t>(pos)];
    pos += SEMESTRE_LEN;
    int semestre = (semChar >= '0' && semChar <= '9')
                   ? semChar - '0'
                   : 0;

    int ano = std::stoi(line.substr(pos, ANO_LEN));
    pos += ANO_LEN;

    int creditos = std::stoi(line.substr(pos, CREDITOS_LEN));
    pos += CREDITOS_LEN;

    double nota1 = std::stod(line.substr(pos, NOTA_LEN));
    pos += NOTA_LEN;

    double nota2 = std::stod(line.substr(pos, NOTA_LEN));
    pos += NOTA_LEN;

    Disciplina d;
    d.setId(id);
    d.setMatricula(matricula);
    d.setNome(nome);
    d.setSemestre(semestre);
    d.setAno(ano);
    d.setCreditos(creditos);
    d.setNota1(nota1);
    d.setNota2(nota2);

    return d;
}

// --------------------------------------------------------
// Contagem: baseado em tamanho do arquivo
// --------------------------------------------------------

int FixedDisciplinaRepository::getRecordCount() const
{
    std::ifstream in(filename, ios::binary);
    if (!in.good())
        return 0;

    in.seekg(0, ios::end);
    std::streampos size = in.tellg();
    if (size < 0)
        throw InfraError("Falha ao obter tamanho do arquivo fixed.");

    if (size % RECORD_LEN != 0)
        throw InfraError("Arquivo fixed corrompido (tamanho inconsistente).");

    return static_cast<int>(size / RECORD_LEN);
}

// --------------------------------------------------------
// IO direto por posição (1-based id)
// --------------------------------------------------------

void FixedDisciplinaRepository::readLineAt(int id, std::string& outLine) const
{
    if (id <= 0)
        throw InfraError("Id invalido em readLineAt.");

    std::ifstream in(filename, ios::binary);
    if (!in)
        throw InfraError("Falha ao abrir arquivo fixed para leitura.");

    std::streamoff offset = static_cast<std::streamoff>(id - 1) * RECORD_LEN;
    in.seekg(offset, ios::beg);

    char buffer[RECORD_NO_EOL_LEN];
    in.read(buffer, RECORD_NO_EOL_LEN);
    if (!in)
        throw InfraError("Falha ao ler registro em readLineAt (id=" + std::to_string(id) + ")");

    // consome o '\n'
    char nl;
    in.read(&nl, 1);
    if (!in)
        throw InfraError("Falha ao ler newline em readLineAt.");

    outLine.assign(buffer, RECORD_NO_EOL_LEN);
}

void FixedDisciplinaRepository::writeLineAt(int id, const std::string& line)
{
    if (id <= 0)
        throw InfraError("Id invalido em writeLineAt.");
    if ((int)line.size() != RECORD_NO_EOL_LEN)
        throw InfraError("Linha invalida em writeLineAt.");

    std::fstream file(filename, ios::binary | ios::in | ios::out);
    if (!file)
        throw InfraError("Falha ao abrir arquivo fixed para escrita.");

    std::streamoff offset = static_cast<std::streamoff>(id - 1) * RECORD_LEN;
    file.seekp(offset, ios::beg);

    file.write(line.data(), RECORD_NO_EOL_LEN);
    if (!file)
        throw InfraError("Falha ao escrever registro em writeLineAt.");

    // mantém o '\n' existente; não precisa reescrever
}

// --------------------------------------------------------
// Métodos do repositório
// --------------------------------------------------------

Disciplina FixedDisciplinaRepository::get(int id) const
{
    LOG_DBG("fixed.get id=", id);

    int total = getRecordCount();
    if (id <= 0 || id > total)
        throw InfraError("Disciplina nao encontrada (id=" + std::to_string(id) + ")");

    std::string line;
    readLineAt(id, line);
    auto d = fromLine(line, id);

    LOG_DBG("fixed.get ok id=", id, " nome=", d.getNome());
    return d;
}

int FixedDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("fixed.insert nome=", disciplina.getNome());

    // garante que o arquivo exista
    {
        std::fstream test(filename, ios::binary | ios::in | ios::out);
        if (!test)
        {
            std::ofstream create(filename, ios::binary);
            if (!create)
                throw InfraError("Nao foi possivel criar arquivo fixed.");
        }
    }

    int total = getRecordCount();
    int newId = total + 1;

    std::ofstream out(filename, ios::binary | ios::app);
    if (!out)
        throw InfraError("Falha ao abrir arquivo fixed para append.");

    std::string line = toLine(disciplina);
    out.write(line.data(), RECORD_NO_EOL_LEN);
    out.put('\n');

    if (!out)
        throw InfraError("Falha ao gravar nova disciplina no arquivo fixed.");

    LOG_DBG("fixed.insert ok id=", newId);
    return newId;
}

void FixedDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("fixed.update id=", id, " novo_nome=", disciplina.getNome());

    int total = getRecordCount();
    if (id <= 0 || id > total)
        throw InfraError("Disciplina nao encontrada para atualizacao (id=" + std::to_string(id) + ")");

    std::string line = toLine(disciplina);
    writeLineAt(id, line);

    LOG_DBG("fixed.update ok id=", id);
}

void FixedDisciplinaRepository::remove(int id)
{
    LOG_DBG("fixed.remove id=", id);

    int total = getRecordCount();
    if (id <= 0 || id > total)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");

    if (total == 0)
        throw InfraError("Arquivo fixed vazio na remocao.");

    const int lastId = total;

    if (id != lastId)
    {
        // lê última linha
        std::string lastLine;
        readLineAt(lastId, lastLine);

        // sobrescreve a posição id com a última
        writeLineAt(id, lastLine);
    }

    // trunca arquivo removendo última linha
    std::error_code ec;
    std::uintmax_t newSize = static_cast<std::uintmax_t>((total - 1) * RECORD_LEN);
    std::filesystem::resize_file(filename, newSize, ec);
    if (ec)
        throw InfraError("Falha ao truncar arquivo fixed na remocao.");

    LOG_DBG("fixed.remove ok id=", id, " total_antigo=", total, " total_novo=", total - 1);
}

std::vector<Disciplina> FixedDisciplinaRepository::list() const
{
    LOG_DBG("fixed.list");

    std::vector<Disciplina> lst;

    int total = getRecordCount();
    if (total == 0)
    {
        LOG_DBG("fixed.list vazio");
        return lst;
    }

    lst.reserve(total);

    std::ifstream in(filename, ios::binary);
    if (!in)
        throw InfraError("Falha ao abrir arquivo fixed para listagem.");

    for (int id = 1; id <= total; ++id)
    {
        char buffer[RECORD_NO_EOL_LEN];
        char nl;

        in.read(buffer, RECORD_NO_EOL_LEN);
        if (!in)
            throw InfraError("Falha ao ler registro em list() id=" + std::to_string(id));

        in.read(&nl, 1);
        if (!in)
            throw InfraError("Falha ao ler newline em list() id=" + std::to_string(id));

        std::string line(buffer, RECORD_NO_EOL_LEN);
        lst.push_back(fromLine(line, id));
    }

    LOG_DBG("fixed.list retornou=", lst.size());
    return lst;
}

bool FixedDisciplinaRepository::exist(const std::string& matricula,
                                      int ano,
                                      int semestre) const
{
    LOG_DBG("fixed.exist matricula=", matricula,
            " ano=", ano, " semestre=", semestre);

    int total = getRecordCount();
    if (total == 0)
        return false;

    std::ifstream in(filename, ios::binary);
    if (!in)
        return false;

    for (int id = 1; id <= total; ++id)
    {
        char buffer[RECORD_NO_EOL_LEN];
        char nl;

        in.read(buffer, RECORD_NO_EOL_LEN);
        if (!in)
            throw InfraError("Erro ao ler registro em exist().");

        in.read(&nl, 1);
        if (!in)
            throw InfraError("Erro ao ler newline em exist().");

        std::string line(buffer, RECORD_NO_EOL_LEN);
        Disciplina d = fromLine(line, id);

        if (d.getMatricula() == matricula &&
            d.getAno()       == ano &&
            d.getSemestre()  == semestre)
        {
            LOG_DBG("fixed.exist true id=", id);
            return true;
        }
    }

    LOG_DBG("fixed.exist false");
    return false;
}

bool FixedDisciplinaRepository::exist(int id) const
{
    LOG_DBG("fixed.exist(id) id=", id);
    int total = getRecordCount();
    bool ok = (id > 0 && id <= total);
    LOG_DBG(ok ? "true" : "false");
    return ok;
}
