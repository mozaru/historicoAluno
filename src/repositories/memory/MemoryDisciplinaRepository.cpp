#include <vector>
#include "Errors.hpp"
#include "Disciplina.hpp"
#include "IDisciplinaRepository.hpp"
#include "MemoryDisciplinaRepository.hpp"

using namespace std;

MemoryDisciplinaRepository::MemoryDisciplinaRepository(ILogger& aLog)
    : log(aLog), qtd(0), lastId(0)
{
}

MemoryDisciplinaRepository::~MemoryDisciplinaRepository() = default;

Disciplina MemoryDisciplinaRepository::get(int id) const
{
    LOG_DBG("id=",id)
    int idx = obterIndice(id);
    if (idx < 0) {
        LOG_DBG("nao encontrado")
        throw InfraError("Disciplina nao encontrada (id=" + std::to_string(id) + ")");
    }
    LOG_DBG("idx=", idx, " retornando elemento")
    return vet[idx];
}

int MemoryDisciplinaRepository::obterIndice(int id) const
{
    LOG_DBG("id=", id, " qtd=", qtd)
    for (int i = 0; i < qtd; ++i) {
        if (vet[i].getId() == id) {
            LOG_DBG("encontrado idx=", i)
            return i;
        }
    }
    LOG_DBG("nao achou o elemento")
    return -1; // nao encontrada
}

int MemoryDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("nome=", disciplina.getNome(), " qtd_atual=", qtd)
    if (qtd >= MAX_DISCIPLINAS)
    {
        LOG_DBG("limite maximo atingido: ", MAX_DISCIPLINAS)
        throw InfraError("Limite de disciplinas em memoria atingido.");
    }

    vet[qtd] = disciplina;
    vet[qtd].setId(++lastId);
    LOG_DBG("pos=", qtd, " id=", lastId, " qtd_nova=", qtd + 1)
    qtd++;
    return lastId;
}

void MemoryDisciplinaRepository::remove(int id)
{
    LOG_DBG("id=", id)
    int idx = obterIndice(id);
    if (idx < 0)
    {
        LOG_DBG("nao encontrado")
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");
    }

    LOG_DBG("idx=", idx, " compactando vetor")
    for (int i = idx; i < qtd - 1; i++)
        vet[i] = vet[i + 1];
    qtd--;
    LOG_DBG("qtd_restante=", qtd)
}

void MemoryDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("id=", id, " novo_nome=", disciplina.getNome())
    int idx = obterIndice(id);
    if (idx < 0)
    {
        LOG_DBG("nao encontrado")
        throw InfraError("Disciplina nao encontrada para atualizacao (id=" + std::to_string(id) + ")");
    }
    LOG_DBG("idx=", idx)
    vet[idx] = disciplina;
    vet[idx].setId(id);
    LOG_DBG("ok")
}

bool MemoryDisciplinaRepository::exist(const std::string& matricula, int ano, int semestre) const
{
    LOG_DBG("matricula=", matricula, " ano=", ano, " semestre=", semestre)
    for (int i = 0; i < qtd; i++)
        if (vet[i].getMatricula() == matricula &&
            vet[i].getAno()       == ano &&
            vet[i].getSemestre()  == semestre)
        {
            LOG_DBG("retorna true, achou idx=", i, " id=", vet[i].getId())
            return true;
        }
    LOG_DBG("retorna false, nao achou")
    return false;
}

bool MemoryDisciplinaRepository::exist(int id) const
{
    LOG_DBG("id=", id)
    bool ok = (obterIndice(id) >= 0);
    LOG_DBG(ok ? "true" : "false")
    return ok;
}

std::vector<Disciplina> MemoryDisciplinaRepository::list() const
{
    LOG_DBG("qtd=", qtd)
    std::vector<Disciplina> lst;
    lst.reserve(qtd);
    for (int i = 0; i < qtd; i++)
        lst.push_back(vet[i]);
    LOG_DBG("retornou=", lst.size())
    return lst;
}

/*
// Calcular CR (coeficiente de rendimento)
double Historico::cr() const
{
    double somaPonderada = 0.0;
    int somaCreditos = 0;

    for (int i = 0; i < qtd; i++)
    {
        somaPonderada += vet[i].media() * vet[i].getCreditos();
        somaCreditos += vet[i].getCreditos();
    }

    return (somaCreditos == 0)? 0.0 : somaPonderada / somaCreditos;
}
*/