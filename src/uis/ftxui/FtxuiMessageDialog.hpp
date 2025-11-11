#ifndef _FTXUI_MESSAGE_DIALOG_HPP_
#define _FTXUI_MESSAGE_DIALOG_HPP_

#include <functional>
#include <string>
#include <memory>

#include <ftxui/component/component.hpp> // ftxui::Component

namespace ui {
namespace ftxuiui {

// Opções para criação de um diálogo de mensagem simples.
struct FtxuiMessageDialogOptions {
    std::string title;        // Ex: "Informacao", "Erro"
    std::string message;      // Texto a ser exibido.
    bool is_error = false;    // Define estilo (erro x info).

    // Chamado quando o usuário fecha o diálogo.
    std::function<void()> on_close;
};

// Cria um componente FTXUI representando um diálogo modal simples.
ftxui::Component MakeMessageDialog(const FtxuiMessageDialogOptions& options);

} // namespace ftxuiui
} // namespace ui

#endif
