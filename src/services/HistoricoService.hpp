#ifndef  _HISTORICO_SERVICE_HPP_
#define _HISTORICO_SERVICE_HPP_

#include "ILogger.hpp"
#include "IHistoricoService.hpp"
#include "IDisciplinaRepository.hpp"

// Implementação concreta de IHistoricoService.
//
// Aplica TODAS as regras de negócio antes de chamar o repositório:
// - (matricula, ano, semestre) únicos
// - ano entre 2000 e ano corrente
// - semestre em {1, 2}
// - créditos entre 1 e 20
// - nome entre 3 e 30 caracteres
// - notas entre 0.0 e 10.0
//
// Em violação de regra -> BusinessError.
// Erros de infra/conversão do repositório são propagados.

class HistoricoService final : public IHistoricoService {
    private:
        IDisciplinaRepository& repo;
        ILogger& log;
        int anoCorrente();
        std::string trim(const std::string& s);
        void validarDisciplina(const Disciplina& d);
    public:
        explicit HistoricoService(IDisciplinaRepository& aRepo, ILogger& aLog);

        int insert(const Disciplina& disciplina) override;
        void update(int id, const Disciplina& disciplina) override;
        void remove(int id) override;
        Disciplina get(int id) const override;
        std::vector<Disciplina> list() const override;
        double calculateCR() const override;
};

#endif
