#ifndef _MEMORY_DISCIPLINA_REPOSITORY_HPP_
#define _MEMORY_DISCIPLINA_REPOSITORY_HPP_

#include<string>

#include "ILogger.hpp"
#include"Disciplina.hpp"
#include"IDisciplinaRepository.hpp"

const int MAX_DISCIPLINAS=100; 

class MemoryDisciplinaRepository : public IDisciplinaRepository
{
    private:
        ILogger& log;
        Disciplina vet[MAX_DISCIPLINAS];
        int        qtd;
        int        lastId;
        int obterIndice(int id) const; 
    public:
        MemoryDisciplinaRepository(ILogger& aLog);
        ~MemoryDisciplinaRepository();

        int insert(const Disciplina& disciplina);
        Disciplina get(int id) const;
        void update(int id, const Disciplina& disciplina);
        void remove(int id);
        std::vector<Disciplina> list() const;
        bool exist(const std::string& matricula, int ano, int semestre) const;
        bool exist(int id) const;
};
#endif