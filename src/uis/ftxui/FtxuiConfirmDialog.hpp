#ifndef _FTXUI_CONFIRM_DIALOG_HPP_
#define _FTXUI_CONFIRM_DIALOG_HPP_

#include <functional>
#include <string>
#include <memory>
#include <ftxui/component/component.hpp> // ftxui::Component

#include"Disciplina.hpp"

namespace ui {
namespace ftxuiui {

struct FtxuiConfirmDialogOptions {
    std::string title;
    std::string message;
    std::string confirm_label;  // default "Sim"
    std::string cancel_label;   // default "Nao"
    bool danger = false;

    std::optional<Disciplina> disciplina;

    std::function<void()> on_confirm;
    std::function<void()> on_cancel;
};

ftxui::Component MakeConfirmDialog(const FtxuiConfirmDialogOptions& options);

} // namespace ftxuiui
} // namespace ui

#endif
