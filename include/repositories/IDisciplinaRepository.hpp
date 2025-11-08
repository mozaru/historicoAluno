#ifndef _IDISCIPLINA_REPOSITORY_HPP_
#define _IDISCIPLINA_REPOSITORY_HPP_

#include <vector>
#include <string>
#include "Disciplina.hpp"

// Interface de acesso a dados para Disciplina.
//
// Regras importantes:
//
// - O "id" é um identificador técnico interno ao sistema:
//     - Em SQLite, pode ser a PK real da tabela.
//     - Em arquivos, pode ser o índice/ordem em memória (não é gravado como dado de domínio).
// - A regra de unicidade de negócio é baseada em (matricula, ano, semestre)
//   e deve ser garantida pela camada de serviço, não pelo repositório.
// - Em caso de erro:
//     - Erros de conversão de dados: ConversionError (definido em core/Errors.hpp).
//     - Erros de infraestrutura (arquivo, banco, etc.): InfraError.
//   A interface não força o tipo de exceção, mas as implementações concretas
//   devem seguir esse contrato para manter o comportamento consistente.

class IDisciplinaRepository {
    public:
        virtual ~IDisciplinaRepository() = default;

        // Insere uma nova disciplina.
        // Retorna o id técnico atribuído à disciplina inserida.
        virtual int insert(const Disciplina& disciplina) = 0;

        // Obtém uma disciplina pelo id técnico.
        // Deve lançar exceção (ex.: InfraError) se o id não existir ou em caso de falha grave.
        virtual Disciplina get(int id) const = 0;

        // Atualiza a disciplina associada ao id técnico informado.
        // Deve lançar exceção se o id não existir ou em caso de falha.
        virtual void update(int id, const Disciplina& disciplina) = 0;

        // Remove a disciplina associada ao id técnico informado.
        // Deve lançar exceção se o id não existir ou em caso de falha.
        virtual void remove(int id) = 0;

        // Retorna a lista completa de disciplinas persistidas.
        // As disciplinas retornadas devem conter seus ids técnicos válidos.
        virtual std::vector<Disciplina> list() const = 0;

        // Verifica se existe alguma disciplina cadastrada com a combinação
        // (matricula, ano, semestre).
        //
        // Usado pela camada de serviço para validar unicidade de negócio.
        virtual bool exist(const std::string& matricula, int ano, int semestre) const = 0;

        // Verifica se existe alguma disciplina cadastrada com esse 
        // (id).
        //
        // Usado pela camada de serviço para validar se existe ou nao de negócio.
        virtual bool exist(int id) const = 0;
};

#endif