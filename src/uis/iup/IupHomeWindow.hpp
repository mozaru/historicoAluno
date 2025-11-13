#ifndef _IUP_HOME_WINDOW_HPP_
#define _IUP_HOME_WINDOW_HPP_

extern "C" {
#include <iup.h>
#include <iupcontrols.h>
}

#include <string>
#include <vector>

#include "IHistoricoService.hpp"
#include "ILogger.hpp"
#include "Disciplina.hpp"

// Janela principal (Home) do Histórico Acadêmico usando IUP.
//
// Responsabilidades:
// - Montar a janela principal (filtro, tabela, botões, status).
// - Carregar lista de disciplinas via IHistoricoService.
// - Calcular e exibir CR via IHistoricoService.
// - Abrir dialogs de inserção, edição, remoção, mensagens.
// - Tratar erros de negócio/infra, logando via ILogger.
//
// Não controla IupOpen/IupClose nem IupMainLoop.
// A UIIup (futura) deve:
//   - criar uma instância de IupHomeWindow;
//   - chamar getDialog() e IupShowXY / IupMainLoop.

class IupHomeWindow {
public:
    IupHomeWindow(IHistoricoService& service, ILogger& logger);
    ~IupHomeWindow();

    // Retorna o Ihandle do dialog principal, para ser usado por UIIup.
    Ihandle* getDialog() const { return dialog; }

    // Recarrega a lista de disciplinas e o CR, aplicando filtro opcional.
    void refresh(const std::string& filter = std::string());

private:
    IHistoricoService& historicoService;
    ILogger& log;

    Ihandle* dialog        = nullptr;
    Ihandle* filterText    = nullptr;
    Ihandle* filterButton  = nullptr;
    Ihandle* insertButton  = nullptr;
    Ihandle* exitButton    = nullptr;
    Ihandle* matrix        = nullptr;
    Ihandle* crLabel       = nullptr;
    Ihandle* infoLabelAutor= nullptr;
    Ihandle* infoLabelData = nullptr;
    Ihandle* infoLabelURL  = nullptr;


    int selectedRow = 0;              // 1-based (linha na matrix)
    std::string currentFilter;
    std::vector<Disciplina> currentData;

    // Monta todos os widgets e o dialog principal.
    void buildDialog();

    // Atualiza o texto do label de CR (usa historicoService.calculateCR()).
    void updateCRLabel();

    // Ações de UI (chamadas pelos callbacks estáticos)
    void handleFilter();
    void handleInsert();
    void handleEdit();
    void handleRemove();
    void handleExit();
    void handleRowSelected(int row);

    // ==== Callbacks estáticos para IUP ====
    static int onFilterButtonAction(Ihandle* ih);
    static int onInsertButtonAction(Ihandle* ih);
    static int onEditButtonAction(Ihandle* ih);
    static int onRemoveButtonAction(Ihandle* ih);
    static int onExitButtonAction(Ihandle* ih);
    static int onMatrixClick(Ihandle* ih, int lin, int col, char* status);
};

#endif
