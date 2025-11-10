#ifndef _CPP_TERM_CONFIRM_DIALOG_HPP_
#define _CPP_TERM_CONFIRM_DIALOG_HPP_

#include <string>
#include <vector>

#include "ILogger.hpp"
#include "CppTermTypes.hpp"
#include "CppTermWindowUtils.hpp"

// Dialog modal de confirmação padrão:
//
// - Título (ex: "REMOVER DISCIPLINA")
// - Linhas de texto (detalhes da ação)
// - Botões: [Sim] [Nao]
// - Navegação por teclado:
//     - ←/→ ou Tab alternam foco entre Sim/Nao
//     - Enter confirma o botão focado
//     - Esc cancela (retorna Cancel)
//     - 's' / 'S' = Sim
//     - 'n' / 'N' = Nao
//
// Responsabilidades:
// - Desenhar usando utilitários de janela (CppTermUI).
// - Não acessar repositórios nem serviços.
class CppTermConfirmDialog
{
public:
    explicit CppTermConfirmDialog(ILogger& logger);

    // Exibe um dialog modal de confirmação.
    //
    //  titulo: linha de título da janela
    //  linhas: conteúdo explicativo (já quebrado em linhas)
    //
    // Retorno:
    //  - CppTermConfirmResult::Yes    -> usuário confirmou (Sim)
    //  - CppTermConfirmResult::No     -> usuário escolheu Não
    //  - CppTermConfirmResult::Cancel -> usuário cancelou via Esc/outro
    CppTermConfirmResult show(const std::string& titulo,
                              const std::vector<std::string>& linhas);

private:
    ILogger& log;
};

#endif // _CPP_TERM_CONFIRM_DIALOG_HPP_
