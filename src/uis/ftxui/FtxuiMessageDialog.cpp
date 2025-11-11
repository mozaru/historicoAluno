#include "FtxuiMessageDialog.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

namespace ui {
namespace ftxuiui {

using namespace ftxui;

ftxui::Component MakeMessageDialog(const FtxuiMessageDialogOptions& options) {
    const std::string title =
        options.title.empty()
            ? (options.is_error ? "Erro" : "Informacao")
            : options.title;

    const std::string message =
        options.message.empty()
            ? (options.is_error ? "Ocorreu um erro." : "Operacao concluida.")
            : options.message;

    auto on_close = options.on_close;

    auto ok_button = Button(" OK ", [on_close] {
        if (on_close) on_close();
    });

    auto container = Container::Vertical({
        ok_button,
    });

    auto dialog = Renderer(container, [title, message, on_close, ok_button, is_error = options.is_error]() {
        Element title_el = text(title) | bold;
        if (is_error)
            title_el = title_el | color(Color::RedLight);
        else
            title_el = title_el | color(Color::GreenLight);

        Element msg_el = paragraph(message);
        if (is_error)
            msg_el = msg_el | color(Color::RedLight);

        Element btn_el = ok_button->Render();

        Element form = vbox({
            title_el | center,
            separator(),
            msg_el | flex,
            separator(),
            hbox({ btn_el }) | center,
        }) | border | 
        size(WIDTH, GREATER_THAN, 70) | size(HEIGHT, GREATER_THAN, 12);

        return form | clear_under | center;
    });

    auto wrapped = CatchEvent(dialog, [on_close](Event event) {
        if (event == ftxui::Event::Escape) {
            if (on_close) on_close();
            return true;
        }
        return false;
    });

    return wrapped;
}

} // namespace ftxuiui
} // namespace ui
