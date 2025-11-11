#include "JsonDisciplinaRepository.hpp"

#include <fstream>
#include <stdexcept>

#include "Errors.hpp"

using namespace std;

JsonDisciplinaRepository::JsonDisciplinaRepository(ILogger& aLog,
                                                   const Configuracao& conf)
    : log(aLog)
    , filename(conf.getFileName("json"))
{
    LOG_INF("JsonDisciplinaRepository arquivo=", filename);
}

JsonDisciplinaRepository::~JsonDisciplinaRepository() = default;

// --------------------------------------------------------
// Helpers load/save
// --------------------------------------------------------

JsonDisciplinaRepository::Json JsonDisciplinaRepository::loadAll() const
{
    std::ifstream in(filename);
    if (!in.good())
    {
        // arquivo não existe ou vazio → array vazio
        return Json::array();
    }

    try
    {
        Json data = Json::parse(in);
        if (!data.is_array())
            throw ConversionError("Arquivo JSON invalido: raiz nao eh array.");
        return data;
    }
    catch (const std::exception& e)
    {
        throw ConversionError(std::string("Falha ao ler JSON de disciplinas: ") + e.what());
    }
}

void JsonDisciplinaRepository::saveAll(const Json& data) const
{
    std::ofstream out(filename, std::ios::trunc);
    if (!out)
        throw InfraError("Falha ao abrir arquivo JSON para escrita.");

    try
    {
        out << data.dump(2); // indentado, didático
    }
    catch (const std::exception& e)
    {
        throw InfraError(std::string("Falha ao gravar JSON de disciplinas: ") + e.what());
    }

    if (!out)
        throw InfraError("Falha ao finalizar escrita do arquivo JSON de disciplinas.");
}

// --------------------------------------------------------
// Map Disciplina <-> Json
// --------------------------------------------------------

JsonDisciplinaRepository::Json
JsonDisciplinaRepository::toJson(const Disciplina& d)
{
    Json j;
    j["matricula"] = d.getMatricula();
    j["nome"]      = d.getNome();
    j["semestre"]  = d.getSemestre();
    j["ano"]       = d.getAno();
    j["creditos"]  = d.getCreditos();
    j["nota1"]     = d.getNota1();
    j["nota2"]     = d.getNota2();
    return j;
}

Disciplina JsonDisciplinaRepository::fromJson(const Json& j, int id)
{
    // Validação mínima defensiva
    if (!j.is_object())
        throw ConversionError("Registro JSON invalido (nao eh objeto).");

    Disciplina d;
    d.setId(id);

    try
    {
        d.setMatricula(j.at("matricula").get<std::string>());
        d.setNome     (j.at("nome").get<std::string>());
        d.setSemestre (j.at("semestre").get<int>());
        d.setAno      (j.at("ano").get<int>());
        d.setCreditos (j.at("creditos").get<int>());
        d.setNota1    (j.at("nota1").get<double>());
        d.setNota2    (j.at("nota2").get<double>());
    }
    catch (const std::exception& e)
    {
        throw ConversionError(std::string("Falha ao converter registro JSON de disciplina: ") + e.what());
    }

    return d;
}

// --------------------------------------------------------
// Util
// --------------------------------------------------------

int JsonDisciplinaRepository::getRecordCount() const
{
    Json data = loadAll();
    return static_cast<int>(data.size());
}

// --------------------------------------------------------
// Métodos do repositório
// --------------------------------------------------------

Disciplina JsonDisciplinaRepository::get(int id) const
{
    LOG_DBG("json.get id=", id);

    if (id <= 0)
        throw InfraError("Id invalido para leitura (id=" + std::to_string(id) + ")");

    Json data = loadAll();
    if (id > static_cast<int>(data.size()))
        throw InfraError("Disciplina nao encontrada (id=" + std::to_string(id) + ")");

    Disciplina d = fromJson(data[static_cast<size_t>(id - 1)], id);
    LOG_DBG("json.get ok id=", id, " nome=", d.getNome());
    return d;
}

int JsonDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("json.insert nome=", disciplina.getNome());

    Json data = loadAll();

    data.push_back(toJson(disciplina));
    int newId = static_cast<int>(data.size());

    saveAll(data);

    LOG_DBG("json.insert ok id=", newId);
    return newId;
}

void JsonDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("json.update id=", id, " novo_nome=", disciplina.getNome());

    if (id <= 0)
        throw InfraError("Id invalido para update (id=" + std::to_string(id) + ")");

    Json data = loadAll();
    if (id > static_cast<int>(data.size()))
        throw InfraError("Disciplina nao encontrada para update (id=" + std::to_string(id) + ")");

    data[static_cast<size_t>(id - 1)] = toJson(disciplina);
    saveAll(data);

    LOG_DBG("json.update ok id=", id);
}

void JsonDisciplinaRepository::remove(int id)
{
    LOG_DBG("json.remove id=", id);

    if (id <= 0)
        throw InfraError("Id invalido para remocao (id=" + std::to_string(id) + ")");

    Json data = loadAll();
    int total = static_cast<int>(data.size());

    if (total == 0 || id > total)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");

    if (total > 1 && id != total)
    {
        // swap com o ultimo para manter ids densos
        data[static_cast<size_t>(id - 1)] = data[static_cast<size_t>(total - 1)];
    }

    data.erase(data.begin() + (total - 1));

    saveAll(data);

    LOG_DBG("json.remove ok id=", id, " total_antigo=", total, " total_novo=", total - 1);
}

std::vector<Disciplina> JsonDisciplinaRepository::list() const
{
    LOG_DBG("json.list");

    std::vector<Disciplina> out;
    Json data = loadAll();

    out.reserve(data.size());
    int id = 1;
    for (const auto& item : data)
    {
        out.push_back(fromJson(item, id));
        ++id;
    }

    LOG_DBG("json.list retornou=", out.size());
    return out;
}

bool JsonDisciplinaRepository::exist(const std::string& matricula,
                                     int ano,
                                     int semestre) const
{
    LOG_DBG("json.exist m=", matricula,
            " ano=", ano, " sem=", semestre);

    Json data = loadAll();
    int id = 1;
    for (const auto& item : data)
    {
        Disciplina d = fromJson(item, id);
        if (d.getMatricula() == matricula &&
            d.getAno()       == ano &&
            d.getSemestre()  == semestre)
        {
            LOG_DBG("json.exist true id=", id);
            return true;
        }
        ++id;
    }

    LOG_DBG("json.exist false");
    return false;
}

bool JsonDisciplinaRepository::exist(int id) const
{
    LOG_DBG("json.exist(id) id=", id);
    int total = getRecordCount();
    bool ok = (id > 0 && id <= total);
    LOG_DBG(ok ? "true" : "false");
    return ok;
}
