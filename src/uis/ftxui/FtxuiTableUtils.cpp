#include "FtxuiTableUtils.hpp"

#include "Conversion.hpp"

#include <ftxui/dom/elements.hpp>

namespace ui {
namespace ftxuiui {

using namespace ftxui;

namespace {

Element MakeCell(const std::string& text, int width, bool make_bold = false) {
    auto e = ftxui::text(text) | size(WIDTH, EQUAL, width);
    if (make_bold) {
        e = e | ftxui::bold;
    }
    return e;
}

std::string safeLeft(const std::string& s, std::size_t max_len) {
    if (s.size() <= max_len) {
        return s;
    }
    if (max_len <= 3) {
        return s.substr(0, max_len);
    }
    return s.substr(0, max_len - 3) + "...";
}

} // namespace

Element MakeTableHeader(const TableConfig& config) {
    std::vector<Element> cells;

    if (config.show_id) {
        cells.push_back(MakeCell("id", config.width_id, true));
    }

    cells.push_back(MakeCell("Matricula", config.width_matricula, true));
    cells.push_back(MakeCell("Nome",      config.width_nome,      true));
    cells.push_back(MakeCell("Creditos",  config.width_creditos,  true));
    cells.push_back(MakeCell("Ano",       config.width_ano,       true));
    cells.push_back(MakeCell("Semestre",  config.width_semestre,  true));
    cells.push_back(MakeCell("Media",     config.width_media,     true));

    return vbox({
        hbox(std::move(cells)),
        separatorHeavy()
    });
}

Element MakeTableRow(const Disciplina& d, const TableConfig& config) {
    std::vector<Element> cells;

    if (config.show_id) {
        cells.push_back(MakeCell(toString(d.getId()), config.width_id));
    }

    cells.push_back(MakeCell(
        safeLeft(d.getMatricula(), static_cast<std::size_t>(config.width_matricula)),
        config.width_matricula
    ));

    cells.push_back(MakeCell(
        safeLeft(d.getNome(), static_cast<std::size_t>(config.width_nome)),
        config.width_nome
    ));

    cells.push_back(MakeCell(
        toString(d.getCreditos()),
        config.width_creditos
    ));

    cells.push_back(MakeCell(
        toString(d.getAno()),
        config.width_ano
    ));

    cells.push_back(MakeCell(
        toString(d.getSemestre()),
        config.width_semestre
    ));

    cells.push_back(MakeCell(
        toString(d.getMedia()),
        config.width_media
    ));

    return hbox(std::move(cells));
}

Element MakeTable(const std::vector<Disciplina>& list,
                  const TableConfig& config) {
    std::vector<Element> rows;
    rows.reserve(list.size() + 2);

    rows.push_back(MakeTableHeader(config));

    for (const auto& d : list) {
        rows.push_back(MakeTableRow(d, config));
    }

    if (list.empty()) {
        rows.push_back(text("Nenhuma disciplina encontrada.") | dim);
    }

    return vbox(std::move(rows));
}

std::string MakeDisciplinaSummary(const Disciplina& d) {
    std::string resumo;
    resumo.reserve(128);

    resumo += "id=" + toString(d.getId());
    resumo += " | Matricula=" + d.getMatricula();
    resumo += " | Nome=" + d.getNome();
    resumo += " | Ano/Sem=" + toString(d.getAno()) + "/" + toString(d.getSemestre());
    resumo += " | Creditos=" + toString(d.getCreditos());
    resumo += " | Media=" + toString(d.getMedia());

    return resumo;
}

} // namespace ftxuiui
} // namespace ui
