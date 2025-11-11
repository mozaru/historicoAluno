#include "FtxuiConfirmDialog.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>

#include "Conversion.hpp"

namespace ui {
namespace ftxuiui {

using namespace ftxui;

namespace {

// helper local, igual ao do form, mas só leitura.
Element Labeled(const std::string& label, const std::string& value, int width = 14) {
    return hbox({
        text(label + ": ") | size(WIDTH, EQUAL, width),
        text(value),
    });
}

} // namespace

ftxui::Component MakeConfirmDialog(const FtxuiConfirmDialogOptions& options) {
    const std::string title =
        options.title.empty() ? "Confirmar" : options.title;

    const std::string message =
        options.message.empty()
            ? "Deseja realmente continuar?"
            : options.message;

    const std::string confirm_label =
        options.confirm_label.empty() ? "Sim" : options.confirm_label;

    const std::string cancel_label =
        options.cancel_label.empty() ? "Nao" : options.cancel_label;

    auto on_confirm = options.on_confirm;
    auto on_cancel  = options.on_cancel;

    // Copia segura da disciplina (se existir) para uso no renderer.
    const bool has_disc = options.disciplina.has_value();
    Disciplina disc;
    if (has_disc) {
        disc = *options.disciplina;
    }

    auto yes_button = Button(confirm_label, [on_confirm] {
        if (on_confirm) on_confirm();
    });

    auto no_button = Button(cancel_label, [on_cancel] {
        if (on_cancel) on_cancel();
    });

    auto buttons = Container::Horizontal({
        yes_button,
        no_button,
    });

    auto dialog = Renderer(buttons,
        [title,
         message,
         yes_button,
         no_button,
         has_disc,
         disc,
         danger = options.danger]() {
            Element title_el = text(title) | bold;
            if (danger)
                title_el = title_el | color(Color::RedLight);

            std::vector<Element> rows;
            rows.push_back(title_el | center);
            rows.push_back(separator());

            // Se tiver disciplina, mostra os dados em modo leitura.
            if (has_disc) {
                rows.push_back(Labeled("id",        toString(disc.getId())));
                rows.push_back(Labeled("Matricula", disc.getMatricula()));
                rows.push_back(Labeled("Nome",      disc.getNome()));
                rows.push_back(Labeled("Creditos",  toString(disc.getCreditos())));
                rows.push_back(Labeled("Ano",       toString(disc.getAno())));
                rows.push_back(Labeled("Semestre",  toString(disc.getSemestre())));
                rows.push_back(Labeled("Media",     toString(disc.getMedia())));
                rows.push_back(separator());
            }

            // Mensagem principal ("Deseja remover..." etc.)
            if (!message.empty()) {
                Element msg_el = paragraph(message);
                if (danger)
                    msg_el = msg_el | color(Color::RedLight);
                rows.push_back(msg_el);
                rows.push_back(separator());
            }

            // Botões
            rows.push_back(
                hbox({
                    yes_button->Render(),
                    text(" "),
                    no_button->Render(),
                }) | center
            );

            Element form =
                vbox(std::move(rows))
                | border
                | size(WIDTH,  GREATER_THAN, 60)
                | size(HEIGHT, GREATER_THAN, 15);

            // Limpa só abaixo da área do dialog; root continua visível ao redor.
            return form | clear_under | center;
        }
    );

    auto wrapped = CatchEvent(dialog,
        [on_cancel](Event event) {
            if (event == Event::Escape) {
                if (on_cancel) on_cancel();
                return true;
            }
            return false;
        }
    );

    return wrapped;
}

} // namespace ftxuiui
} // namespace ui
