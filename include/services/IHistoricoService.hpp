#ifndef _IHISTORICO_SERVICE_HPP_
#define _IHISTORICO_SERVICE_HPP_

#include <vector>
#include "Disciplina.hpp"

// Interface de regras de negócio para o histórico acadêmico.
//
// Responsabilidades:
// - Orquestrar operações sobre disciplinas do histórico.
// - Aplicar TODAS as validações de negócio antes de chamar o repositório:
//     - (matricula, ano, semestre) únicos
//     - ano entre 2000 e ano corrente
//     - semestre em {1, 2}
//     - créditos entre 1 e 20
//     - nome entre 3 e 30 caracteres
//     - notas entre 0.0 e 10.0
// - Calcular o coeficiente de rendimento (CR).
//
// Erros:
// - Violações de regra de negócio -> BusinessError (core/Errors.hpp).
// - Problemas de infra/conversão são propagados das camadas abaixo.

class IHistoricoService {
    public:
        virtual ~IHistoricoService() = default;

        // Adiciona uma nova disciplina ao histórico.
        // Aplica todas as validações de negócio.
        // Retorna o id técnico atribuído à disciplina.
        virtual int insert(const Disciplina& disciplina) = 0;

        // Atualiza uma disciplina existente identificada por id.
        // Aplica validações de negócio (incluindo unicidade).
        virtual void update(int id, const Disciplina& disciplina) = 0;

        // Remove uma disciplina existente pelo id técnico.
        virtual void remove(int id) = 0;

        // Obtém uma disciplina específica pelo id técnico.
        // Deve lançar exceção se não encontrada.
        virtual Disciplina get(int id) const = 0;

        // Lista todas as disciplinas do histórico.
        // A filtragem específica (por ano, semestre, etc.) pode ser feita na UI
        // a partir desta lista, se desejado.
        virtual std::vector<Disciplina> list() const = 0;

        // Calcula o coeficiente de rendimento (CR) com base
        // nas disciplinas válidas cadastradas.
        // A regra exata de cálculo é documentada na implementação.
        virtual double calculateCR() const = 0;
};

#endif