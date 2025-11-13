#ifndef _IUP_MESSAGE_DIALOG_HPP_
#define _IUP_MESSAGE_DIALOG_HPP_

extern "C" {
#include <iup.h>
}

#include <string>

// Helpers para exibir dialogs modais simples de mensagem.
// São completamente stateless: quem chama decide quando logar e o que mostrar.
//
// Padrão alinhado à especificação:
// - SUCESSO: título "SUCESSO", texto customizado, botão OK.
// - ERRO: título "ERRO", texto amigável, botão OK.
// - INFO: título customizável, texto customizável.

namespace IupMessageDialog {

    // Dialog genérico de informação (título e mensagem arbitrários).
    // Bloqueia até o usuário clicar em OK.
    void showInfo(const std::string& title, const std::string& message);

    // Dialog padrão de sucesso.
    // Título fixo: "SUCESSO".
    void showSuccess(const std::string& message);

    // Dialog padrão de erro.
    // Título fixo: "ERRO".
    void showError(const std::string& message);

} // namespace IupMessageDialog

#endif
