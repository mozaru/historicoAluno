#include "FtxuiHomeScreen.hpp"

#include <algorithm>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>

#include "FtxuiAppState.hpp"
#include "FtxuiTableUtils.hpp"
#include "Conversion.hpp"

namespace ui {
namespace ftxuiui {

using ftxui::Box;
using ftxui::Button;
using ftxui::Component;
using ftxui::Element;
using ftxui::Event;
using ftxui::Renderer;
using ftxui::border;
using ftxui::bold;
using ftxui::center;
using ftxui::color;
using ftxui::dim;
using ftxui::flex;
using ftxui::frame;
using ftxui::hbox;
using ftxui::inverted;
using ftxui::paragraph;
using ftxui::separator;
using ftxui::separatorHeavy;
using ftxui::size;
using ftxui::text;
using ftxui::vbox;
using ftxui::filler;
using ftxui::EQUAL;
using ftxui::WIDTH;

namespace {

// Botão 1-linha no estilo [Label], sem borda de janela.
// Usa ButtonOption para aproveitar foco/teclado/mouse do FTXUI.
ftxui::Component MakeFlatBracketButton(const std::string& label,
                                       std::function<void()> on_click) {
    using namespace ftxui;

    auto opt = ButtonOption::Simple();
    //opt.border = false; // sem moldura 3xN

    opt.transform = [label](const EntryState& state) {
        // Desenha sempre em UMA linha: [Label]
        Element e = text("[" + label + "]");
        if (state.focused) {
            e = e | inverted; // destaque quando focado
        }
        return e;
    };

    // Label visual vem do transform; aqui passamos vazio.
    return Button("", std::move(on_click), opt);
}

} // namespace

// -----------------------------------------------------------------------------
// Componente concreto da Home Screen
// -----------------------------------------------------------------------------
class HomeScreenComponent : public ftxui::ComponentBase {
public:
    HomeScreenComponent(FtxuiAppState& state,
                        const FtxuiHomeScreenCallbacks& callbacks)
        : state_(state)
        , callbacks_(callbacks)
    {
        // Vamos usar um único campo de filtro textual.
        // Se o FtxuiAppState tiver apenas filtros separados,
        // reaproveitamos filter_nome como "filtro geral".
        filter_text_ptr_ = &state_.filter_nome;

        input_filter_ = ftxui::Input(filter_text_ptr_, "Filtrar por matricula, nome,...");

        btn_filtrar_ = MakeFlatBracketButton("Filtrar", [this] {
            state_.ApplyFilter();
        });

        btn_inserir_ = MakeFlatBracketButton("Inserir", [this] {
            if (callbacks_.on_new_disciplina)
                callbacks_.on_new_disciplina();
        });

        // Container principal focável: filtro + botões.
        main_container_ = ftxui::Container::Horizontal({
            input_filter_,
            btn_filtrar_,
            btn_inserir_,
        });

        Add(main_container_);

        state_.ApplyFilter();
    }

    Element OnRender() override {
        const auto& list = state_.Filtered();
        const int selected = state_.SelectedIndex();

        // ---------------------------------------------------------------------
        // Título
        // ---------------------------------------------------------------------
        Element title = text("Historico do Aluno - FTXUI") | bold | center;

        // ---------------------------------------------------------------------
        // Linha de filtro (uma só)
        //  Filtro: [.................] [Filtrar] [Inserir]
        // ---------------------------------------------------------------------
        Element filter_line =
            hbox({
                text("Filtro: "),
                input_filter_->Render() | flex,
                text("  "),
                btn_filtrar_->Render(),
                text("  "),
                btn_inserir_->Render(),
            });

        // ---------------------------------------------------------------------
        // Tabela com coluna de ações
        // ---------------------------------------------------------------------
        TableConfig cfg; // usa larguras padrão do helper

        const int width_id        = cfg.show_id ? cfg.width_id : 0;
        const int width_mat       = cfg.width_matricula;
        const int width_nome      = cfg.width_nome;
        const int width_creditos  = cfg.width_creditos;
        const int width_ano       = cfg.width_ano;
        const int width_sem       = cfg.width_semestre;
        const int width_media     = cfg.width_media;
        const int width_actions   = 18; // espaço para "[Editar][Remov]"

        auto cell = [](const std::string& s, int w, bool bold_flag = false) {
            Element e = text(s) | size(WIDTH, EQUAL, w);
            if (bold_flag) e = e | bold;
            return e;
        };

        // Header
        std::vector<Element> header_cells;
        if (cfg.show_id)
            header_cells.push_back(cell("ID", width_id, true));
        header_cells.push_back(cell("Matricula", width_mat, true));
        header_cells.push_back(cell("Nome",      width_nome, true));
        header_cells.push_back(cell("Crd",       width_creditos, true));
        header_cells.push_back(cell("Ano",       width_ano, true));
        header_cells.push_back(cell("Sem",       width_sem, true));
        header_cells.push_back(cell("Media", width_media, true));

        // filler empurra a próxima célula (Acoes) para o final da linha
        header_cells.push_back(filler());

        header_cells.push_back(
            text("Acoes") | bold | size(WIDTH, EQUAL, width_actions)
        );

        

        std::vector<Element> table_rows;
        table_rows.reserve(list.size() + 2);
        table_rows.push_back(
            vbox({
                hbox(std::move(header_cells)),
                separatorHeavy(),
            })
        );

        // Linhas
        for (std::size_t i = 0; i < list.size(); ++i) {
            const auto& d = list[i];
            std::vector<Element> cols;

            if (cfg.show_id)
                cols.push_back(cell(toString(d.getId()), width_id));

            cols.push_back(cell(d.getMatricula(), width_mat));
            cols.push_back(cell(d.getNome(),      width_nome));
            cols.push_back(cell(toString(d.getCreditos()), width_creditos));
            cols.push_back(cell(toString(d.getAno()),      width_ano));
            cols.push_back(cell(toString(d.getSemestre()), width_sem));
            cols.push_back(cell(toString(d.getMedia()),    width_media));

            bool is_hover_row = (static_cast<int>(i) == hovered_action_row_);

            Element edit_el  = text("[Editar]");
            Element remov_el = text("[Remov]");

            if (is_hover_row) {
                if (hovered_action_type_ == HoveredAction::Edit) {
                    edit_el = edit_el | inverted;
                } else if (hovered_action_type_ == HoveredAction::Remove) {
                    remov_el = remov_el | inverted;
                }
            }

            Element actions = hbox({
                edit_el,
                text(" "),
                remov_el,
            }) | size(WIDTH, EQUAL, width_actions);

            cols.push_back(filler());
            cols.push_back(actions);

            Element row = hbox(std::move(cols));
            if (static_cast<int>(i) == selected) {
                row = row | inverted;
            }

            table_rows.push_back(row);
        }

        if (list.empty()) {
            table_rows.push_back(text("Nenhuma disciplina encontrada.") | dim);
        }

        Element table =
            vbox(std::move(table_rows))
            | frame                 // permite scroll vertical
            | ftxui::reflect(table_box_);

        // ---------------------------------------------------------------------
        // Rodapé: status + ajuda
        // ---------------------------------------------------------------------
       
        // Linha 1: coluna 1 = dica de scroll (sempre), coluna 2 = status/erro (ocupa resto)
        Element scroll_hint =
            text("^ scroll com ↑/↓ / PgUp/PgDn / (mouse se disponivel)") | dim;

        Element status_or_error;
        if (!state_.ErrorMessage().empty()) {
            status_or_error = text(state_.ErrorMessage()) | color(ftxui::Color::RedLight);
        } else if (!state_.StatusMessage().empty()) {
            status_or_error = text(state_.StatusMessage());
        } else {
            status_or_error = text("");
        }

        Element footer_line1 = hbox({
            scroll_hint,
            hbox({ filler(), status_or_error }) | flex,
        });

        // Linha 2: 4 colunas, última alinhada à direita
        Element col1 = text("Autor: Mozar Baptista da Silva");
        Element col2 = text("11/2025");
        Element col3 = text("mozar.silva@gmail.com");
        Element col4 = text("www.11tech.com.br");

        // Cada coluna com flex ~igual; a última com texto grudado à direita da sua "faixa".
        Element footer_line2 = hbox({
            col1 | flex,
            col2 | flex,
            col3 | flex,
            hbox({ filler(), col4 }) | flex,  // filler empurra col4 pra direita
        }) | dim;

        Element footer = vbox({
            footer_line1,
            footer_line2,
        });

        // Layout geral
        Element layout =
            vbox({
                title,
                separator(),
                filter_line,
                separator(),
                table | flex,
                separator(),
                footer,
            })
            | border;

        return layout;
    }

    bool OnEvent(Event event) override {
        // Eventos nos componentes de filtro/botões

        if (event == Event::Tab) {
            if (main_container_->OnEvent(event)) {
                // Tab circulou entre filtro / [Filtrar] / [Inserir]
                table_focused_ = false;
                return true;
            }
            // Se não há mais onde focar na barra, Tab joga o foco na tabela
            table_focused_ = true;
            return true;
        }
        if (event == Event::Insert) {
            if (callbacks_.on_new_disciplina)
                callbacks_.on_new_disciplina();
            return true;
        }
        if ((!table_focused_ || event.is_mouse()) && main_container_->OnEvent(event)) {
            state_.ApplyFilter();
            table_focused_ = false;
            return true;
        }

        if (table_focused_) {
            // Navegação na lista
            if (event == Event::ArrowUp) {
                MoveSelection(-1);
                return true;
            }
            if (event == Event::ArrowDown) {
                MoveSelection(+1);
                return true;
            }
            if (event == Event::PageUp) {
                MoveSelection(-5);
                return true;
            }
            if (event == Event::PageDown) {
                MoveSelection(+5);
                return true;
            }
            if (event == Event::Delete) {
                TriggerDeleteSelected();
                return true;
            }
            // Enter = editar selecionado
            if (event == Event::Return) {
                TriggerEditSelected();
                return true;
            }

            // Atalhos de teclado
            if (event.is_character()) {
                const char ch = event.character()[0];
                switch (ch) {
                    case 'e':
                    case 'E':
                        TriggerEditSelected();
                        return true;

                    case 'd':
                    case 'D':
                        TriggerDeleteSelected();
                        return true;

                    default:
                        break;
                }
            }
        }


        // Insert = novo

        if (event.is_character()) {
            const char ch = event.character()[0];
            switch (ch) {
                case 'n':
                case 'N':
                    if (callbacks_.on_new_disciplina)
                        callbacks_.on_new_disciplina();
                    return true;

                case 'c':
                case 'C':
                    if (callbacks_.on_show_cr)
                        callbacks_.on_show_cr();
                    return true;

                case 'q':
                case 'Q':
                    if (callbacks_.on_quit)
                        callbacks_.on_quit();
                    return true;

                default:
                    break;
            }
        }


        // Clique do mouse na tabela:
        if (event.is_mouse()) {
            const auto& m = event.mouse();
            
            hovered_action_row_ = -1;
            hovered_action_type_ = HoveredAction::None;

            if (table_box_.Contain(m.x, m.y)) {

                if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed)
                    table_focused_ = true;

                const auto& list = state_.Filtered();
                if (list.empty())
                    return true;

                // linha relativa dentro da tabela
                int relative_y = m.y - table_box_.y_min;
                int row_index = relative_y - 2; // -21 para pular header

                if (row_index >= 0 &&
                    row_index < static_cast<int>(list.size())) {
                    
                    if (m.button == ftxui::Mouse::None || m.motion == ftxui::Mouse::Moved) {
                        state_.SelectIndex(row_index);
                    }

                    // Detectar clique na coluna de ações:
                    // reserva ~18 colunas no fim para [Editar][Remov]
                    int actions_width = 18;
                    int actions_start = table_box_.x_max - actions_width+1;

                    if (m.x >= actions_start) {
                        int mid = actions_start + actions_width / 2;
                        hovered_action_row_ = row_index;
                        if (m.x < mid) {
                            hovered_action_type_ = HoveredAction::Edit;
                        } else {
                            hovered_action_type_ = HoveredAction::Remove;
                        }
                    }


                    if (m.button == ftxui::Mouse::Left &&
                        m.motion == ftxui::Mouse::Pressed &&
                        m.x >= actions_start) {
                        // divide metade: esquerda = Editar, direita = Remover
                        int mid = actions_start + actions_width / 2;
                        state_.SelectIndex(row_index);
                        if (m.x < mid) {
                            TriggerEditSelected();
                        } else {
                            TriggerDeleteSelected();
                        }
                    }
                }
                return true;
            }
        }

        return false;
    }

private:
    FtxuiAppState& state_;
    FtxuiHomeScreenCallbacks callbacks_;

    Component main_container_;

    bool table_focused_ = false;

    int hovered_action_row_ = -1;
    enum class HoveredAction { None, Edit, Remove };
    HoveredAction hovered_action_type_ = HoveredAction::None;

    // Filtro único (usamos filter_nome como backing store)
    std::string* filter_text_ptr_ = nullptr;
    Component input_filter_;
    Component btn_filtrar_;
    Component btn_inserir_;

    Box table_box_;

    void MoveSelection(int delta) {
        const auto& list = state_.Filtered();
        if (list.empty()) {
            state_.SelectIndex(-1);
            return;
        }

        int idx = state_.SelectedIndex();
        if (idx < 0) idx = 0;
        idx += delta;

        if (idx < 0) idx = 0;
        if (idx >= static_cast<int>(list.size()))
            idx = static_cast<int>(list.size()) - 1;

        state_.SelectIndex(idx);
    }

    void TriggerEditSelected() {
        if (!callbacks_.on_edit_disciplina)
            return;

        int id = state_.GetSelectedId();
        if (id > 0)
            callbacks_.on_edit_disciplina(id);
    }

    void TriggerDeleteSelected() {
        if (!callbacks_.on_delete_disciplina)
            return;

        int id = state_.GetSelectedId();
        if (id > 0)
            callbacks_.on_delete_disciplina(id);
    }
};

Component MakeHomeScreen(FtxuiAppState& state,
                         const FtxuiHomeScreenCallbacks& callbacks) {
    return ftxui::Make<HomeScreenComponent>(state, callbacks);
}

} // namespace ftxuiui
} // namespace ui