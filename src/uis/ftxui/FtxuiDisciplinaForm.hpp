#ifndef _FTXUI_DISCIPLINA_FORM_HPP_
#define _FTXUI_DISCIPLINA_FORM_HPP_

#include <functional>
#include <memory>
#include <string>

#include <ftxui/component/component.hpp>

#include "Disciplina.hpp"

namespace ui {
namespace ftxuiui {

struct FtxuiDisciplinaFormOptions {
    std::string title;
    bool editing = false;
    Disciplina initial;

    std::function<void(const Disciplina&)> on_submit;
    std::function<void()> on_cancel;
};

ftxui::Component MakeDisciplinaForm(const FtxuiDisciplinaFormOptions& options);

} // namespace ftxuiui
} // namespace ui

#endif
