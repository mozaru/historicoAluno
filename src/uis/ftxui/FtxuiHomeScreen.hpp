#ifndef _FTXUI_HOME_SCREEN_HPP_
#define _FTXUI_HOME_SCREEN_HPP_

#include <functional>
#include <memory>

#include <ftxui/component/component.hpp>

namespace ui {
namespace ftxuiui {

class FtxuiAppState;

struct FtxuiHomeScreenCallbacks {
    std::function<void()>      on_new_disciplina;
    std::function<void(int)>   on_edit_disciplina;
    std::function<void(int)>   on_delete_disciplina;
    std::function<void()>      on_show_cr;
    std::function<void()>      on_quit;
};

ftxui::Component MakeHomeScreen(FtxuiAppState& state,
                                const FtxuiHomeScreenCallbacks& callbacks);

} // namespace ftxuiui
} // namespace ui

#endif
