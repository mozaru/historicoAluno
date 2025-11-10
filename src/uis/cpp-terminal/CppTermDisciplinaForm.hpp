#ifndef _CPP_TERM_DISCIPLINA_FORM_HPP_
#define _CPP_TERM_DISCIPLINA_FORM_HPP_

#include <string>
#include <vector>

#include "Disciplina.hpp"
#include "ILogger.hpp"
#include "CppTermTypes.hpp"
#include "CppTermWindowUtils.hpp"

// Formulário modal para Inserir/Editar Disciplina.
//
// Responsabilidades:
// - Mostrar uma "janela" com campos da Disciplina.
// - Permitir edição navegável por teclado.
// - Validar tipos básicos (int/double).
// - Em caso de OK, aplicar valores em Disciplina& d.
// - Em caso de Cancelar (Esc), não alterar d.
//
// Não acessa IHistoricoService nem repositórios.
// Não faz regras de negócio (só conversão de tipos).
class CppTermDisciplinaForm
{
public:
    explicit CppTermDisciplinaForm(ILogger& logger);

    // Exibe o formulário.
    //
    //  d        : em modo edição, vem preenchido; em inserção, normalmente vazio.
    //  isEdicao : true para "Editar Disciplina", false para "Inserir Disciplina".
    //
    // Retorno:
    //  - true  -> usuário confirmou; 'd' é atualizado com os valores do formulário.
    //  - false -> usuário cancelou ou houve erro crítico; 'd' NÃO é modificado.
    bool show(Disciplina& d, bool isEdicao);

private:
    ILogger& log;

    struct Field {
        std::string label;
        std::string value;
        int         maxLen;
        bool        numeric;      // apenas dígitos + (p/ double, '.' e ',')
        bool        real;         // se true e numeric, permite ponto decimal
    };

    // Edita os campos em memória. Se retornar true, 'fields' contém valores finais.
    bool editFields(std::vector<Field>& fields,
                    const std::string& titulo);

    // Converte os valores dos campos para dentro de Disciplina.
    // Retorna true em sucesso; false se conversão falhar.
    bool applyToDisciplina(const std::vector<Field>& fields,
                           Disciplina& d);
};

#endif // _CPP_TERM_DISCIPLINA_FORM_HPP_
