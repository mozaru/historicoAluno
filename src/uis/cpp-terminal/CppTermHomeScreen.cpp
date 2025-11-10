#include "CppTermHomeScreen.hpp"

#include <algorithm>
#include <sstream>

// cpp-terminal
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/exception.hpp"

CppTermHomeScreen::CppTermHomeScreen(ILogger& logger)
    : log(logger),
      scrollOffset(0),
      selectedIndex(0),
      filterText(),
      focus(Focus::FilterInput)
{
}

CppTermHomeResult CppTermHomeScreen::show(const std::vector<Disciplina>& disciplinas,
                                          double cr,
                                          const std::string& currentFilter)
{
    CppTermHomeResult result;
    result.action       = CppTermHomeAction::None;
    result.filtroRetornado = currentFilter;
    result.disciplinaId = -1;

    filterText   = currentFilter;
    scrollOffset = 0;
    selectedIndex = 0;
    focus        = Focus::FilterInput;

    try {
        Term::Screen term_size = Term::screen_size();
        Term::Window win(term_size);

        // Layout: margens fixas simples
        const std::size_t headerLines = 3;  // título + borda
        const std::size_t filterLines = 2;  // linha filtro
        const std::size_t footerLines = 2;  // créditos
        const std::size_t tableStart  = headerLines + filterLines + 1;

        const std::size_t usableRows =
            (term_size.rows() > headerLines + filterLines + footerLines + 3)
            ? term_size.rows() - headerLines - filterLines - footerLines - 3
            : 3;

        bool running     = true;
        bool needRender  = true;

        LOG_INF("HomeScreen aberta: disciplinas=", disciplinas.size(), " cr=", cr)

        while (running) {
            if (needRender) {
                render(win, disciplinas, cr, term_size.rows(), term_size.columns());
                Term::cout << win.render(1, 1, false) << std::flush;
                needRender = false;
            }

            const Term::Key key = Term::read_event();

            // Em caso de terminal em raw: algumas teclas podem vir como Key::Unknown;
            // ignoramos silenciosamente.
            if (key == Term::Key::NoKey) {
                continue;
            }

            if (handleKey(key, disciplinas, usableRows, result)) {
                running = false;
            } else {
                needRender = true;
            }
        }

        LOG_INF("HomeScreen encerrada: action=", static_cast<int>(result.action),
                " disciplinaId=", result.disciplinaId, " filtro='", result.filtroRetornado, "'")
    }
    catch (const Term::Exception& e) {
        LOG_ERR("HomeScreen: erro cpp-terminal: ", e.what())
        result.action = CppTermHomeAction::Exit;
    }
    catch (const std::exception& e) {
        LOG_ERR("HomeScreen: erro inesperado: ", e.what())
        result.action = CppTermHomeAction::Exit;
    }

    return result;
}

void CppTermHomeScreen::render(Term::Window& win,
                               const std::vector<Disciplina>& disciplinas,
                               double cr,
                               std::size_t rows,
                               std::size_t cols)
{
    win.clear();

    renderHeader(win, cols);
    renderFilterBar(win, cols);
    renderTable(win, disciplinas, cr, rows, cols);
    renderFooter(win, rows, cols);
}

void CppTermHomeScreen::renderHeader(Term::Window& win,
                                     std::size_t cols) const
{
    // Linha superior com título centralizado
    CppTermUI::draw_horizontal_line(win, 1, 1, cols);
    const std::string title = "HISTORICO ALUNO";
    CppTermUI::drawCenteredText(win, 2, title);
    CppTermUI::draw_horizontal_line(win, 3, 1, cols);
}

void CppTermHomeScreen::renderFilterBar(Term::Window& win,
                                        std::size_t cols) const
{
    const std::size_t y = 4;
    const std::string label = "Filtro: ";
    win.print_str(2, y, label);

    // Campo de filtro (conceitual)
    const std::size_t fieldWidth = cols > 10 ? cols - 40 : 10;
    const std::size_t fieldX = 2 + label.size();
    std::string shown = filterText;
    if (shown.size() > fieldWidth - 2) {
        shown = shown.substr(shown.size() - (fieldWidth - 2));
    }

    CppTermUI::draw_input_box(win, fieldX, y - 0, fieldWidth, shown,
                                       (focus == Focus::FilterInput));

    // Botões [Filtrar] [Inserir]
    std::size_t bx = fieldX + fieldWidth + 4;
    const std::string btFilter = "Filtrar";
    const std::string btInsert = "Inserir";

    CppTermUI::draw_button(win, bx, y, btFilter,
                                    (focus == Focus::FilterButton));
    bx += btFilter.size() + 10;
    CppTermUI::draw_button(win, bx, y, btInsert,
                                    (focus == Focus::InsertButton));

    CppTermUI::draw_horizontal_line(win, y + 1, 1, cols);
}

void CppTermHomeScreen::renderTable(Term::Window& win,
                                    const std::vector<Disciplina>& disciplinas,
                                    double cr,
                                    std::size_t rows,
                                    std::size_t cols) 
{
    const std::size_t headerY = 6;
    const std::size_t startY  = headerY + 2;
    const std::size_t endY    = rows - 3; // deixa espaço pro footer
    if (endY <= startY) return;

    // Cabeçalho da tabela
    win.print_str(2, headerY, "ID");
    win.print_str(7, headerY, "Matricula");
    win.print_str(20, headerY, "Nome");
    win.print_str(50, headerY, "Crd");
    win.print_str(55, headerY, "Ano");
    win.print_str(60, headerY, "Sem");
    win.print_str(65, headerY, "Media");
    win.print_str(73, headerY, "Acoes");

    CppTermUI::draw_horizontal_line(win, headerY + 1, 1, cols);

    const std::size_t maxVisible = (endY > startY) ? (endY - startY) : 0;

    // Garantir limites de scroll/seleção
    const std::size_t total = disciplinas.size();
    if (scrollOffset > total) {
        scrollOffset = 0;
    }
    std::size_t idx = scrollOffset;

    for (std::size_t i = 0; i < maxVisible && idx < total; ++i, ++idx) {
        const Disciplina& d = disciplinas[idx];
        const std::size_t y = startY + i;

        const bool selected = (focus == Focus::Table && idx == selectedIndex);

        // Linha básica
        std::ostringstream mediaStr;
        mediaStr.setf(std::ios::fixed);
        mediaStr.precision(2);
        mediaStr << d.getMedia();

        win.print_str(2,  y, std::to_string(d.getId()));
        win.print_str(7,  y, d.getMatricula());
        win.print_str(20, y, CppTermUI::truncate(d.getNome(), 28));
        win.print_str(50, y, std::to_string(d.getCreditos()));
        win.print_str(55, y, std::to_string(d.getAno()));
        win.print_str(60, y, std::to_string(d.getSemestre()));
        win.print_str(65, y, mediaStr.str());
        win.print_str(73, y, "[Editar] [Remover]");

        if (selected) {
            // Destaca linha selecionada (bg simples)
            CppTermUI::highlight_line(win, y, 1, cols);
        }
    }

    // Linha com CR
    std::ostringstream crStr;
    crStr.setf(std::ios::fixed);
    crStr.precision(2);
    crStr << "CR do aluno: " << cr;

    const std::size_t crY = std::min(endY + 1, rows - 4);
    win.print_str(2, crY, crStr.str());
    CppTermUI::draw_horizontal_line(win, crY + 1, 1, cols);
}

void CppTermHomeScreen::renderFooter(Term::Window& win,
                                     std::size_t rows,
                                     std::size_t cols) const
{
    const std::size_t y = rows - 1;

    const std::string footer =
        "Autor: Mozar Baptista da Silva | 11/2025 | "
        "mozar.silva@gmail.com | www.11tech.com.br";

    CppTermUI::draw_truncated_centered(win, y, cols, footer);
}

void CppTermHomeScreen::adjustSelection(std::size_t totalRows,
                                        std::size_t visibleRows)
{
    if (totalRows == 0) {
        selectedIndex = 0;
        scrollOffset  = 0;
        return;
    }

    if (selectedIndex >= totalRows) {
        selectedIndex = totalRows - 1;
    }

    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + visibleRows) {
        scrollOffset = selectedIndex - visibleRows + 1;
    }
}

bool CppTermHomeScreen::handleKey(const Term::Key& key,
                                  const std::vector<Disciplina>& disciplinas,
                                  std::size_t visibleRows,
                                  CppTermHomeResult& outResult)
{
    const std::size_t total = disciplinas.size();

    char ch = static_cast<std::int32_t>(key);
    if ((ch == 8 || ch==127 || ch>31)  && focus == Focus::FilterInput)
    {
        // Backspace manual se vier como caractere (caso API entregue assim)
        if (ch == 8 || ch == 127)
        {
            if (!filterText.empty()) {
                filterText.pop_back();
            }
        }
        else if (filterText.size() < 30 && ch>31)
        {
            filterText.push_back(ch);
        }
        outResult.action          = CppTermHomeAction::Filter;
        outResult.filtroRetornado = filterText;
        return true;
    }
    

    switch (key) {
    // Navegação básica entre áreas com Tab (assumindo Term::Key::Tab exista;
    // se o nome na sua versão for diferente, ajuste aqui).
    case Term::Key::Tab:
        if (focus == Focus::FilterInput)      focus = Focus::FilterButton;
        else if (focus == Focus::FilterButton) focus = Focus::InsertButton;
        else if (focus == Focus::InsertButton) focus = Focus::Table;
        else                                   focus = Focus::FilterInput;
        LOG_DBG("HomeScreen: focus ciclo Tab=", static_cast<int>(focus))
        return false;

    /*case Term::Key::Shift_Tab:
        if (focus == Focus::FilterInput)       focus = Focus::Table;
        else if (focus == Focus::Table)        focus = Focus::InsertButton;
        else if (focus == Focus::InsertButton) focus = Focus::FilterButton;
        else                                   focus = Focus::FilterInput;
        LOG_DBG("HomeScreen: focus ciclo Shift+Tab=", static_cast<int>(focus))
        return false;
    */
    // Atalhos diretos
    case Term::Key::F:
    case Term::Key::f:
        // aplicar filtro atual
        outResult.action     = CppTermHomeAction::Filter;
        outResult.filtroRetornado = filterText;
        LOG_INF("HomeScreen: acao=Filter, filtro='", filterText, "'")
        return true;

    case Term::Key::I:
    case Term::Key::i:
        outResult.action     = CppTermHomeAction::Insert;
        outResult.filtroRetornado = filterText;
        LOG_INF("HomeScreen: atalho Insert")
        return true;

    // Sair da UI a partir da home (opcional)
    case Term::Key::q:
    case Term::Key::Esc:
    case Term::Key::Ctrl_C:
        outResult.action = CppTermHomeAction::Exit;
        LOG_INF("HomeScreen: solicitacao sair")
        return true;

    // Enter: depende do foco
    case Term::Key::Enter:
    case Term::Key::Space:
        switch (focus) {
        case Focus::FilterInput:
        case Focus::FilterButton:
            outResult.action     = CppTermHomeAction::Filter;
            outResult.filtroRetornado = filterText;
            LOG_INF("HomeScreen: Enter em Filter")
            return true;
        case Focus::InsertButton:
            outResult.action     = CppTermHomeAction::Insert;
            outResult.filtroRetornado = filterText;
            LOG_INF("HomeScreen: Enter em Insert")
            return true;
        case Focus::Table:
            if (!disciplinas.empty() && selectedIndex < disciplinas.size()) {
                outResult.action     = CppTermHomeAction::Edit;
                outResult.disciplinaId = disciplinas[selectedIndex].getId();
                outResult.filtroRetornado = filterText;
                LOG_INF("HomeScreen: Enter em linha -> Edit id=", outResult.disciplinaId)
                return true;
            }
            return false;
        }
        return false;

    // Navegação na tabela
    case Term::Key::ArrowUp:
        if (focus == Focus::Table && total > 0) {
            if (selectedIndex > 0) {
                --selectedIndex;
                adjustSelection(total, visibleRows);
                LOG_DBG("HomeScreen: ArrowUp sel=", selectedIndex)
            }
        }
        return false;

    case Term::Key::ArrowDown:
        if (focus == Focus::Table && total > 0) {
            if (selectedIndex + 1 < total) {
                ++selectedIndex;
                adjustSelection(total, visibleRows);
                LOG_DBG("HomeScreen: ArrowDown sel=", selectedIndex)
            }
        }
        return false;

    case Term::Key::PageUp:
        if (focus == Focus::Table && total > 0) {
            if (selectedIndex >= visibleRows)
                selectedIndex -= visibleRows;
            else
                selectedIndex = 0;
            adjustSelection(total, visibleRows);
            LOG_DBG("HomeScreen: PageUp sel=", selectedIndex)
        }
        return false;

    case Term::Key::PageDown:
        if (focus == Focus::Table && total > 0) {
            if (selectedIndex + visibleRows < total)
                selectedIndex += visibleRows;
            else
                selectedIndex = total - 1;
            adjustSelection(total, visibleRows);
            LOG_DBG("HomeScreen: PageDown sel=", selectedIndex)
        }
        return false;

    // Remover rápido: 'r' quando em tabela
    case Term::Key::R:
    case Term::Key::r:
    case Term::Key::Del:
        if (focus == Focus::Table && !disciplinas.empty()
            && selectedIndex < disciplinas.size()) {
            outResult.action     = CppTermHomeAction::Remove;
            outResult.disciplinaId = disciplinas[selectedIndex].getId();
            outResult.filtroRetornado = filterText;
            LOG_INF("HomeScreen: atalho Remover id=", outResult.disciplinaId)
            return true;
        }
        return false;

    // Editar rápido: 'e' quando em tabela
    case Term::Key::E:
    case Term::Key::e:
        if (focus == Focus::Table && !disciplinas.empty()
            && selectedIndex < disciplinas.size()) {
            outResult.action     = CppTermHomeAction::Edit;
            outResult.disciplinaId = disciplinas[selectedIndex].getId();
            outResult.filtroRetornado = filterText;
            LOG_INF("HomeScreen: atalho Edit id=", outResult.disciplinaId)
            return true;
        }
        return false;

    default:
        return false;
    }
}
