#include "BinaryDisciplinaRepository.hpp"

#include <fstream>
#include <filesystem>
#include <cstring>
#include "Errors.hpp"

using namespace std;

BinaryDisciplinaRepository::BinaryDisciplinaRepository(ILogger& aLog, const Configuracao& conf)
    : log(aLog), filename(conf.getFileName("bin"))
{
    LOG_INF("BinaryDisciplinaRepository criado em arquivo: ", filename);
}

BinaryDisciplinaRepository::~BinaryDisciplinaRepository() = default;

// --------------------------------------------------------
// Helpers de string fixa
// --------------------------------------------------------

void BinaryDisciplinaRepository::writeStringFixed(char* dest, std::size_t maxLen, const std::string& src)
{
    std::size_t len = src.size();
    if (len >= maxLen)
        len = maxLen - 1;

    std::memcpy(dest, src.data(), len);
    if (len < maxLen)
        std::memset(dest + len, 0, maxLen - len);
    else
        dest[maxLen - 1] = '\0';
}

std::string BinaryDisciplinaRepository::readStringFixed(const char* src, std::size_t maxLen)
{
    // Garante que não lê lixo além do buffer
    std::size_t len = 0;
    while (len < maxLen && src[len] != '\0')
        ++len;
    return std::string(src, len);
}

// --------------------------------------------------------
// Conversão Disciplina <-> Record
// --------------------------------------------------------

BinaryDisciplinaRepository::Record BinaryDisciplinaRepository::toRecord(const Disciplina& d)
{
    Record r{};
    writeStringFixed(r.matricula, sizeof(r.matricula), d.getMatricula());
    writeStringFixed(r.nome,      sizeof(r.nome),      d.getNome());
    r.ano      = d.getAno();
    r.semestre = d.getSemestre();
    r.creditos = d.getCreditos();
    r.nota1    = d.getNota1();
    r.nota2    = d.getNota2();
    return r;
}

Disciplina BinaryDisciplinaRepository::fromRecord(const Record& r, int id)
{
    Disciplina d;
    d.setId(id);
    d.setMatricula(readStringFixed(r.matricula, sizeof(r.matricula)));
    d.setNome(readStringFixed(r.nome, sizeof(r.nome)));
    d.setAno(r.ano);
    d.setSemestre(r.semestre);
    d.setCreditos(r.creditos);
    d.setNota1(r.nota1);
    d.setNota2(r.nota2);
    return d;
}

// --------------------------------------------------------
// Contagem de registros (sem cabeçalho, tamanho fixo)
// --------------------------------------------------------

int BinaryDisciplinaRepository::getRecordCount() const
{
    std::ifstream in(filename, ios::binary);
    if (!in.good())
        return 0; // arquivo ainda não existe

    in.seekg(0, ios::end);
    std::streampos size = in.tellg();
    if (size < 0)
        throw InfraError("Falha ao obter tamanho do arquivo binario de disciplinas.");

    const auto recSize = static_cast<std::streamoff>(sizeof(Record));
    if (size % recSize != 0)
        throw InfraError("Arquivo binario de disciplinas corrompido (tamanho inconsistente).");

    return static_cast<int>(size / recSize);
}

// --------------------------------------------------------
// GET
// --------------------------------------------------------

Disciplina BinaryDisciplinaRepository::get(int id) const
{
    LOG_DBG("bin.get id=", id);

    if (id <= 0)
        throw InfraError("Id invalido para leitura de disciplina (id=" + to_string(id) + ")");

    int total = getRecordCount();
    if (id > total)
        throw InfraError("Disciplina nao encontrada (id=" + to_string(id) + ")");

    std::ifstream in(filename, ios::binary);
    if (!in)
        throw InfraError("Falha ao abrir arquivo binario de disciplinas para leitura.");

    std::streamoff offset = static_cast<std::streamoff>(id - 1) * static_cast<std::streamoff>(sizeof(Record));
    in.seekg(offset, ios::beg);

    Record r{};
    in.read(reinterpret_cast<char*>(&r), sizeof(r));
    if (!in)
        throw InfraError("Falha ao ler registro de disciplina (id=" + to_string(id) + ")");

    Disciplina d = fromRecord(r, id);
    LOG_DBG("bin.get ok id=", id, " nome=", d.getNome());
    return d;
}

// --------------------------------------------------------
// INSERT
// --------------------------------------------------------

int BinaryDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("bin.insert nome=", disciplina.getNome());

    // Garante existencia do arquivo
    {
        std::fstream test(filename, ios::binary | ios::in | ios::out);
        if (!test)
        {
            std::ofstream create(filename, ios::binary);
            if (!create)
                throw InfraError("Nao foi possivel criar arquivo binario de disciplinas.");
        }
    }

    int total = getRecordCount();
    int newId = total + 1;

    std::ofstream out(filename, ios::binary | ios::app);
    if (!out)
        throw InfraError("Falha ao abrir arquivo binario de disciplinas para append.");

    Record r = toRecord(disciplina);

    out.write(reinterpret_cast<const char*>(&r), sizeof(r));
    if (!out)
        throw InfraError("Falha ao gravar nova disciplina no arquivo binario.");

    LOG_DBG("bin.insert ok id=", newId);
    return newId;
}

// --------------------------------------------------------
// UPDATE
// --------------------------------------------------------

void BinaryDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("bin.update id=", id, " novo_nome=", disciplina.getNome());

    int total = getRecordCount();
    if (id <= 0 || id > total)
        throw InfraError("Disciplina nao encontrada para atualizacao (id=" + to_string(id) + ")");

    std::fstream file(filename, ios::binary | ios::in | ios::out);
    if (!file)
        throw InfraError("Falha ao abrir arquivo binario de disciplinas para atualizacao.");

    Record r = toRecord(disciplina);
    std::streamoff offset = static_cast<std::streamoff>(id - 1) * static_cast<std::streamoff>(sizeof(Record));

    file.seekp(offset, ios::beg);
    file.write(reinterpret_cast<const char*>(&r), sizeof(r));

    if (!file)
        throw InfraError("Falha ao escrever disciplina atualizada no arquivo binario.");

    LOG_DBG("bin.update ok id=", id);
}

// --------------------------------------------------------
// REMOVE
// --------------------------------------------------------

void BinaryDisciplinaRepository::remove(int id)
{
    LOG_DBG("bin.remove id=", id);

    int total = getRecordCount();
    if (id <= 0 || id > total)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + to_string(id) + ")");

    if (total == 0)
        throw InfraError("Arquivo binario vazio na remocao (estado inconsistente).");

    std::fstream file(filename, ios::binary | ios::in | ios::out);
    if (!file)
        throw InfraError("Falha ao abrir arquivo binario de disciplinas para remocao.");

    const auto recSize = static_cast<std::streamoff>(sizeof(Record));
    const int lastId = total;

    if (id != lastId)
    {
        // Le ultimo registro
        Record last{};
        std::streamoff lastOffset = static_cast<std::streamoff>(lastId - 1) * recSize;
        file.seekg(lastOffset, ios::beg);
        file.read(reinterpret_cast<char*>(&last), sizeof(last));
        if (!file)
            throw InfraError("Falha ao ler ultimo registro durante remocao.");

        // Sobrescreve o registro a remover com o ultimo
        std::streamoff delOffset = static_cast<std::streamoff>(id - 1) * recSize;
        file.seekp(delOffset, ios::beg);
        file.write(reinterpret_cast<const char*>(&last), sizeof(last));
        if (!file)
            throw InfraError("Falha ao sobrescrever registro na remocao.");
    }

    file.close();

    // Trunca o arquivo removendo o ultimo bloco
    std::error_code ec;
    std::uintmax_t newSize = static_cast<std::uintmax_t>((total - 1) * sizeof(Record));
    std::filesystem::resize_file(filename, newSize, ec);
    if (ec)
        throw InfraError("Falha ao truncar arquivo binario de disciplinas na remocao.");

    LOG_DBG("bin.remove ok id=", id, " total_antigo=", total, " total_novo=", total - 1);
}

// --------------------------------------------------------
// EXIST
// --------------------------------------------------------

bool BinaryDisciplinaRepository::exist(const std::string& matricula, int ano, int semestre) const
{
    LOG_DBG("bin.exist matricula=", matricula, " ano=", ano, " semestre=", semestre);

    std::ifstream in(filename, ios::binary);
    if (!in)
        return false;

    Record r{};
    int id = 1;

    while (in.read(reinterpret_cast<char*>(&r), sizeof(r)))
    {
        if (readStringFixed(r.matricula, sizeof(r.matricula)) == matricula &&
            r.ano      == ano &&
            r.semestre == semestre)
        {
            LOG_DBG("bin.exist true id=", id);
            return true;
        }
        ++id;
    }

    LOG_DBG("bin.exist false");
    return false;
}

bool BinaryDisciplinaRepository::exist(int id) const
{
    LOG_DBG("bin.exist(id) id=", id);
    int total = getRecordCount();
    bool ok = (id > 0 && id <= total);
    LOG_DBG(ok ? "true" : "false");
    return ok;
}

// --------------------------------------------------------
// LIST
// --------------------------------------------------------

std::vector<Disciplina> BinaryDisciplinaRepository::list() const
{
    LOG_DBG("bin.list");

    std::vector<Disciplina> lst;

    std::ifstream in(filename, ios::binary);
    if (!in)
    {
        LOG_DBG("bin.list arquivo inexistente, retornando lista vazia");
        return lst;
    }

    // calcula total via tamanho do arquivo
    in.seekg(0, ios::end);
    std::streampos size = in.tellg();
    if (size < 0)
        throw InfraError("Falha ao obter tamanho do arquivo binario em list().");

    const auto recSize = static_cast<std::streamoff>(sizeof(Record));
    if (size % recSize != 0)
        throw InfraError("Arquivo binario de disciplinas corrompido em list().");

    int total = static_cast<int>(size / recSize);
    lst.reserve(total);

    in.seekg(0, ios::beg);

    Record r{};
    int id = 1;
    while (id <= total && in.read(reinterpret_cast<char*>(&r), sizeof(r)))
    {
        lst.push_back(fromRecord(r, id));
        ++id;
    }

    if ((id - 1) != total)
        throw InfraError("Falha de leitura completa na listagem de disciplinas.");

    LOG_DBG("bin.list retornou=", lst.size());
    return lst;
}
