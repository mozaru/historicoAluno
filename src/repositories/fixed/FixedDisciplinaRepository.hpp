#ifndef FIXED_DISCIPLINA_REPOSITORY_HPP
#define FIXED_DISCIPLINA_REPOSITORY_HPP

#include <string>
#include <vector>

#include "ILogger.hpp"
#include "IDisciplinaRepository.hpp"
#include "Disciplina.hpp"
#include "Configuracao.hpp"

class FixedDisciplinaRepository : public IDisciplinaRepository
{
public:
    FixedDisciplinaRepository(ILogger& aLog, const Configuracao& conf);
    ~FixedDisciplinaRepository() override;

    Disciplina get(int id) const override;
    int insert(const Disciplina& disciplina) override;
    void update(int id, const Disciplina& disciplina) override;
    void remove(int id) override;
    std::vector<Disciplina> list() const override;
    bool exist(const std::string& matricula, int ano, int semestre) const override;
    bool exist(int id) const; // se já existir no contrato, isso implementa

private:
    ILogger& log;
    std::string filename;

    static constexpr int MATRICULA_LEN = 20;
    static constexpr int NOME_LEN      = 60;
    static constexpr int SEMESTRE_LEN  = 1;
    static constexpr int ANO_LEN       = 4;
    static constexpr int CREDITOS_LEN  = 2;
    static constexpr int NOTA_LEN      = 5;

    static constexpr int RECORD_NO_EOL_LEN =
        MATRICULA_LEN +
        NOME_LEN +
        SEMESTRE_LEN +
        ANO_LEN +
        CREDITOS_LEN +
        NOTA_LEN + // nota1
        NOTA_LEN;  // nota2

    static constexpr int RECORD_LEN = RECORD_NO_EOL_LEN + 1; // + '\n'

    // Helpers principais
    static std::string toLine(const Disciplina& d);
    static Disciplina fromLine(const std::string& line, int id);

    static std::string padOrTrim(const std::string& s, int width);
    static std::string rtrim(const std::string& s);

    int  getRecordCount() const;
    void readLineAt(int id, std::string& outLine) const;
    void writeLineAt(int id, const std::string& line);
};

#endif // FIXED_DISCIPLINA_REPOSITORY_HPP
