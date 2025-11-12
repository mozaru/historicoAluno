#include "HistoricoService.hpp"

#include "IDisciplinaRepository.hpp"
#include "Errors.hpp"

#include <ctime>
#include <cctype>
#include <string>

HistoricoService::HistoricoService(IDisciplinaRepository& aRepo, ILogger& aLog)
    : repo(aRepo), log(aLog)
{
}
int HistoricoService::insert(const Disciplina& disciplina)
{
    LOG_DBG("insert: inicio, matricula=", disciplina.getMatricula(), " ano=", disciplina.getAno(), " semestre=", disciplina.getSemestre())
    validarDisciplina(disciplina);

    // Regra de unicidade (matricula, ano, semestre)
    if (repo.exist(disciplina.getMatricula(),
                    disciplina.getAno(),
                    disciplina.getSemestre())) {
        LOG_DBG("insert: violacao de unicidade (matricula/ano/semestre ja existente)")
        throw BusinessError(
            "Ja existe disciplina com esta matricula/ano/semestre.");
    }

    // Repositorio define o id tecnico
    int id = repo.insert(disciplina);
    LOG_INF("insert: ok, id=", id, " matricula=", disciplina.getMatricula(), " ano=", disciplina.getAno(), " semestre=", disciplina.getSemestre())
    return id;
}

void HistoricoService::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("update: inicio, id=", id, " nova_matricula=", disciplina.getMatricula(), " ano=", disciplina.getAno(), " semestre=", disciplina.getSemestre())
    Disciplina atual = repo.get(id);

    validarDisciplina(disciplina);

    // Se (matricula, ano, semestre) mudaram, garantir que nao conflitam
    const bool chaveMudou =
        disciplina.getMatricula() != atual.getMatricula() ||
        disciplina.getAno()       != atual.getAno() ||
        disciplina.getSemestre()  != atual.getSemestre();

    if (chaveMudou) {
        if (repo.exist(disciplina.getMatricula(),
                        disciplina.getAno(),
                        disciplina.getSemestre())) {
            LOG_DBG("update: violacao unicidade para id=", id)
            throw BusinessError(
                "Ja existe outra disciplina com esta matricula/ano/semestre.");
        }
    }

    // Mantem o id tecnico
    Disciplina copia = disciplina;
    copia.setId(id);

    repo.update(id, copia);
    LOG_INF("update: ok, id=", id)
}

void HistoricoService::remove(int id)
{
    LOG_DBG("remove: inicio, id=", id)
    // Se nao existir, o repositorio lanca InfraError
    repo.remove(id);
    LOG_INF("remove: ok, id=", id)
}

namespace {
    double calcularMedia(const Disciplina& d)
    {
        return (d.getNota1()+d.getNota2())/2;
    }
}

Disciplina HistoricoService::get(int id) const
{
    LOG_DBG("get: id=", id)
    // Se nao existir, o repositorio lanca InfraError
    Disciplina d = repo.get(id);
    d.setMedia(calcularMedia(d));
    LOG_INF("get: retornou nome=", d.getNome())
    return d;
}

std::vector<Disciplina> HistoricoService::list() const
{
    LOG_DBG("list: inicio");
    auto lst = repo.list();
    for(Disciplina& d : lst)
       d.setMedia(calcularMedia(d));
    LOG_INF("list: retornou ", lst.size(), " disciplinas");
    return lst;
}

double HistoricoService::calculateCR() const
{
    LOG_DBG("calculateCR: inicio")
    const auto disciplinas = repo.list();

    if (disciplinas.empty()) {
        LOG_DBG("calculateCR: sem disciplinas, retorno 0.0")
        return 0.0;
    }

    double somaPonderada = 0.0;
    int somaCreditos = 0;

    for (const auto& d : disciplinas) {
        const int creditos = d.getCreditos();
        if (creditos <= 0) {
            continue; // ignora registros com creditos invalidos
        }

        const double media = (d.getNota1() + d.getNota2()) / 2.0;
        somaPonderada += media * static_cast<double>(creditos);
        somaCreditos  += creditos;
    }

    if (somaCreditos == 0) {
        LOG_DBG("calculateCR: somaCreditos=0, retorno 0.0")
        return 0.0;
    }
    double cr = somaPonderada / static_cast<double>(somaCreditos);
    LOG_INF("calculateCR: ok, CR=", cr, " (disciplinas=", disciplinas.size(), ", somaCreditos=", somaCreditos, ")")
    return cr;
}

// ---------- privados ----------

int HistoricoService::anoCorrente()
{
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm.tm_year + 1900;
}

std::string HistoricoService::trim(const std::string& s)
{
    auto begin = s.begin();
    while (begin != s.end() &&
           std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }

    auto end = s.end();
    while (end != begin) {
        auto prev = end;
        --prev;
        if (!std::isspace(static_cast<unsigned char>(*prev))) {
            break;
        }
        end = prev;
    }

    return std::string(begin, end);
}

void HistoricoService::validarDisciplina(const Disciplina& d)
{
    const int anoMin = 1900;
    const int anoMax = anoCorrente();

    // Nome
    const std::string nome = trim(d.getNome());
    if (nome.size() < 3 || nome.size() > 50) {
        LOG_DBG("validarDisciplina: nome invalido (len=", nome.size(), ")")
        throw BusinessError(
            "Nome da disciplina deve ter entre 3 e 50 caracteres.");
    }

    const std::string matricula = trim(d.getMatricula());
    if (matricula.empty() || matricula.size() > 20) {
        LOG_DBG("validarDisciplina: matricula invalida (len=", matricula.size(), ")")
        throw BusinessError(
            "Matricula da disciplina nao pode ser vazia e nao pode ter mais de 20 caracteres.");
    }

    // Ano
    if (d.getAno() < anoMin || d.getAno() > anoMax) {
        LOG_DBG("validarDisciplina: ano invalido=", d.getAno())
        throw BusinessError(
            "Ano invalido. Deve estar entre 1900 e o ano corrente.");
    }

    // Semestre
    if (d.getSemestre() != 1 && d.getSemestre() != 2) {
        LOG_DBG("validarDisciplina: semestre invalido=", d.getSemestre())
        throw BusinessError(
            "Semestre invalido. Deve ser 1 ou 2.");
    }

    // Creditos
    if (d.getCreditos() < 1 || d.getCreditos() > 20) {
        LOG_DBG("validarDisciplina: creditos invalidos=", d.getCreditos())
        throw BusinessError(
            "Creditos invalidos. Deve estar entre 1 e 20.");
    }

    // Notas
    if (d.getNota1() < 0.0 || d.getNota1() > 10.0) {
        LOG_DBG("validarDisciplina: nota 1 invalida=", d.getNota1());
        throw BusinessError(
            "Nota1 invalida. Deve estar entre 0.0 e 10.0.");
    }

    if (d.getNota2() < 0.0 || d.getNota2() > 10.0) {
        LOG_DBG("validarDisciplina: nota 2 invalida=", d.getNota2());
        throw BusinessError(
            "Nota2 invalida. Deve estar entre 0.0 e 10.0.");
    }
}
