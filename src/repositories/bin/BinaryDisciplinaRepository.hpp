#ifndef BINARY_DISCIPLINA_REPOSITORY_HPP
#define BINARY_DISCIPLINA_REPOSITORY_HPP

#include <string>
#include <vector>
#include <cstdint>
#include "ILogger.hpp"
#include "IDisciplinaRepository.hpp"
#include "Disciplina.hpp"
#include "Configuracao.hpp"

class BinaryDisciplinaRepository : public IDisciplinaRepository
{
public:
    BinaryDisciplinaRepository(ILogger& aLog, const Configuracao& conf);
    ~BinaryDisciplinaRepository() override;

    // Implementações do contrato
    Disciplina get(int id) const override;
    int insert(const Disciplina& disciplina) override;
    void update(int id, const Disciplina& disciplina) override;
    void remove(int id) override;
    std::vector<Disciplina> list() const override;
    bool exist(const std::string& matricula, int ano, int semestre) const override;

    // Se o IDisciplinaRepository no seu projeto já tiver essa sobrecarga,
    // isto é a implementação natural para o modo binário.
    bool exist(int id) const;

private:
    ILogger& log;
    std::string filename;

#pragma pack(push, 1)
    struct Record
    {
        char     matricula[21]; // ajustável: 20 + '\0'
        char     nome[61];      // ajustável: 60 + '\0'
        int32_t  ano;
        int32_t  semestre;
        int32_t  creditos;
        double   nota1;
        double   nota2;
    };
#pragma pack(pop)

    // Helpers
    static void writeStringFixed(char* dest, std::size_t maxLen, const std::string& src);
    static std::string readStringFixed(const char* src, std::size_t maxLen);

    static Record toRecord(const Disciplina& d);
    static Disciplina fromRecord(const Record& r, int id);

    int getRecordCount() const;
};

#endif // BINARY_DISCIPLINA_REPOSITORY_HPP
