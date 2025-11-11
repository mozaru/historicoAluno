#ifndef _FTXUI_APP_STATE_HPP_
#define _FTXUI_APP_STATE_HPP_

#include <string>
#include <vector>

#include "Disciplina.hpp"

namespace ui {
namespace ftxuiui {

// Estado compartilhado entre os componentes FTXUI.
// - Não chama service, não faz I/O, não loga.
// - Mantém cache em memória e filtros aplicados localmente.
// - Nunca lança exceção para fora: métodos são "safe".
class FtxuiAppState {
public:
    // Define a lista completa (vinda da camada de serviço).
    // Reaplica o filtro atual.
    void SetAll(const std::vector<Disciplina>& list);

    // Acesso às coleções.
    const std::vector<Disciplina>& All() const noexcept;
    const std::vector<Disciplina>& Filtered() const noexcept;

    // Campos de filtro (strings para facilitar entrada do usuário).
    std::string filter_matricula;
    std::string filter_nome;
    std::string filter_ano;
    std::string filter_semestre;
    std::string filter_min_media;
    std::string filter_max_media;

    // Aplica os filtros atuais sobre o cache em memória.
    // Em caso de erro de conversão, mantém lista completa filtrada de forma segura
    // e registra mensagem em error_message.
    void ApplyFilter() noexcept;

    // Seleção na lista filtrada (por índice).
    void SelectIndex(int index) noexcept;
    int SelectedIndex() const noexcept;

    // Obtém disciplina selecionada (ou nullptr).
    const Disciplina* GetSelected() const noexcept;
    Disciplina* GetSelected() noexcept;

    // Obtém id da disciplina selecionada (ou -1 se nada selecionado).
    int GetSelectedId() const noexcept;

    // Mensagens de status/erro para o usuário.
    void ClearMessages() noexcept;
    void SetStatus(const std::string& msg) noexcept;
    void SetError(const std::string& msg) noexcept;

    const std::string& StatusMessage() const noexcept;
    const std::string& ErrorMessage() const noexcept;

    // Controle de saída da aplicação.
    void RequestQuit() noexcept;
    bool ShouldQuit() const noexcept;

    // Atualizações locais após operações bem-sucedidas na camada de serviço.
    // (Chamados pelo controlador/UIFtxui.)
    void AddOrUpdateDisciplina(const Disciplina& d) noexcept;
    void RemoveDisciplinaById(int id) noexcept;

private:
    std::vector<Disciplina> all_disciplines_;
    std::vector<Disciplina> filtered_disciplines_;

    int selected_index_ = -1;

    std::string status_message_;
    std::string error_message_;
    bool request_quit_ = false;

    void RebuildSelectionAfterFilter(int previous_selected_id) noexcept;
};

} // namespace ftxuiui
} // namespace ui

#endif
