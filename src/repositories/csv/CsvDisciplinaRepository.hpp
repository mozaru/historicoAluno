#ifndef CSV_DISCIPLINA_REPOSITORY_HPP
#define CSV_DISCIPLINA_REPOSITORY_HPP

#include <string>
#include <vector>

#include "ILogger.hpp"
#include "IDisciplinaRepository.hpp"
#include "Disciplina.hpp"
#include "Configuracao.hpp"

class CsvDisciplinaRepository : public IDisciplinaRepository
{
public:
    CsvDisciplinaRepository(ILogger& aLog, const Configuracao& conf);
    ~CsvDisciplinaRepository() override;

    Disciplina get(int id) const override;
    int insert(const Disciplina& disciplina) override;
    void update(int id, const Disciplina& disciplina) override;
    void remove(int id) override;
    std::vector<Disciplina> list() const override;
    bool exist(const std::string& matricula, int ano, int semestre) const override;

    bool exist(int id) const; // se tiver no contrato, essa é a implementação natural

private:
    ILogger& log;
    std::string filename;

    int  getRecordCount() const;
    bool readLineById(int id, std::string& line) const;

    static std::string disciplinaToCsv(const Disciplina& d);
    static Disciplina csvToDisciplina(const std::string& line, int id);

    static std::vector<std::string> splitCsv(const std::string& line);
};

#endif // CSV_DISCIPLINA_REPOSITORY_HPP
