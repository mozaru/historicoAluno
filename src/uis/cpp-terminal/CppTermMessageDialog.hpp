#ifndef _CPP_TERM_MESSAGE_DIALOG_HPP_
#define _CPP_TERM_MESSAGE_DIALOG_HPP_

#include <string>

#include "ILogger.hpp"
#include "CppTermTypes.hpp"
#include "CppTermWindowUtils.hpp"

// Dialog simples de mensagem modal:
// - título
// - texto centralizado (ou linhas)
// - botão [OK]
// - fecha com Enter, Espaço ou Esc
//
// Responsabilidades:
// - Desenhar usando cpp-terminal (via WindowUtils).
// - Registrar logs de exibição quando fizer sentido.
// - NÃO acessar serviços nem repositórios.
class CppTermMessageDialog
{
public:
    explicit CppTermMessageDialog(ILogger& logger);

    // Exibe um dialog modal com título e mensagem.
    // Bloqueia até o usuário pressionar OK (Enter/Espaco/Esc).
    // Retorna sempre CppTermMessageResult::Ok (mantido para consistência/expansão).
    CppTermMessageResult show(const std::string& titulo,
                              const std::string& mensagem);

private:
    ILogger& log;
};

#endif // _CPP_TERM_MESSAGE_DIALOG_HPP_
