#ifndef _FTXUI_TABLE_UTILS_HPP_
#define _FTXUI_TABLE_UTILS_HPP_

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>

#include "Disciplina.hpp"

// Utilitários para montar a tabela de disciplinas na UI FTXUI.
// Responsabilidades:
// - Formatação consistente de colunas.
// - Criação de header, linhas e tabela completa.
// - Sem chamadas a camada de serviço ou logging aqui.
// - Conversão numérica via Conversion.hpp (aplicado no .cpp).

namespace ui {
namespace ftxuiui {

struct TableConfig {
    bool show_id = true;

    int width_id        = 4;   // "id"
    int width_matricula = 12;  // "Matricula"
    int width_nome      = 26;  // "Nome"
    int width_creditos  = 9;   // "Creditos"
    int width_ano       = 6;   // "Ano"
    int width_semestre  = 9;   // "Semestre"
    int width_media     = 7;   // "Media"
};

// Cria o header da tabela (linha de títulos).
ftxui::Element MakeTableHeader(const TableConfig& config = {});

// Cria uma linha da tabela para uma disciplina.
ftxui::Element MakeTableRow(const Disciplina& d, const TableConfig& config = {});

// Cria a tabela completa (header + linhas).
ftxui::Element MakeTable(const std::vector<Disciplina>& list,
                         const TableConfig& config = {});

// Gera um resumo textual curto da disciplina, útil para tooltips / diálogos.
std::string MakeDisciplinaSummary(const Disciplina& d);

} // namespace ftxuiui
} // namespace ui

#endif
