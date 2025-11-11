#ifndef SQLITE_DISCIPLINA_REPOSITORY_HPP
#define SQLITE_DISCIPLINA_REPOSITORY_HPP

#include <string>
#include <vector>

#include "ILogger.hpp"
#include "IDisciplinaRepository.hpp"
#include "Disciplina.hpp"
#include "Configuracao.hpp"

// Declaracao forward para evitar incluir sqlite3.h no header se quiser.
// Mas aqui podemos incluir direto para simplicidade.
#include "sqlite3.h"

class SQLiteDisciplinaRepository : public IDisciplinaRepository
{
public:
    SQLiteDisciplinaRepository(ILogger& aLog, const Configuracao& conf);
    ~SQLiteDisciplinaRepository() override;

    Disciplina get(int id) const override;
    int insert(const Disciplina& disciplina) override;
    void update(int id, const Disciplina& disciplina) override;
    void remove(int id) override;
    std::vector<Disciplina> list() const override;
    bool exist(const std::string& matricula, int ano, int semestre) const override;

    bool exist(int id) const; // se estiver no contrato base, isso implementa

private:
    ILogger&    log;
    std::string filename;
    sqlite3*    db;

    void openDatabase();
    void closeDatabase();
    void ensureSchema() const;

    static Disciplina mapRowToDisciplina(sqlite3_stmt* stmt);
};

#endif // SQLITE_DISCIPLINA_REPOSITORY_HPP
