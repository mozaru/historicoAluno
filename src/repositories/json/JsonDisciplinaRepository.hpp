#ifndef JSON_DISCIPLINA_REPOSITORY_HPP
#define JSON_DISCIPLINA_REPOSITORY_HPP

#include <string>
#include <vector>

#include "ILogger.hpp"
#include "IDisciplinaRepository.hpp"
#include "Disciplina.hpp"
#include "Configuracao.hpp"

// nlohmann::json (header-only) em external/json/json.hpp
#include "json.hpp"

class JsonDisciplinaRepository : public IDisciplinaRepository
{
public:
    JsonDisciplinaRepository(ILogger& aLog, const Configuracao& conf);
    ~JsonDisciplinaRepository() override;

    Disciplina get(int id) const override;
    int insert(const Disciplina& disciplina) override;
    void update(int id, const Disciplina& disciplina) override;
    void remove(int id) override;
    std::vector<Disciplina> list() const override;
    bool exist(const std::string& matricula, int ano, int semestre) const override;

    bool exist(int id) const; // se estiver no contrato base, isso implementa

private:
    using Json = nlohmann::json;

    ILogger&    log;
    std::string filename;

    Json loadAll() const;
    void saveAll(const Json& data) const;

    static Json        toJson(const Disciplina& d);
    static Disciplina  fromJson(const Json& j, int id);

    int getRecordCount() const;
};

#endif // JSON_DISCIPLINA_REPOSITORY_HPP
