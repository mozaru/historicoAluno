#include "IupHomeWindow.hpp"

#include <algorithm>
#include <cctype>

#include "Conversion.hpp"
#include "Errors.hpp"
#include "IupHelpers.hpp"
#include "IupDisciplinaFormDialog.hpp"
#include "IupConfirmDialog.hpp"
#include "IupMessageDialog.hpp"

#include<iostream>
using namespace std;

// Ponteiro global para a instância única da Home.
// Simples e suficiente, dado que teremos apenas uma UI.
namespace {
    IupHomeWindow* g_homeWindow = nullptr;
}

// ==== Função auxiliar para lowercase ====
static std::string toLowerCopy(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        out.push_back(static_cast<char>(std::tolower(c)));
    }
    return out;
}

IupHomeWindow::IupHomeWindow(IHistoricoService& service, ILogger& logger)
    : historicoService(service),
      log(logger)
{
    g_homeWindow = this;

    buildDialog();
    // Carrega dados iniciais
    refresh();
}

IupHomeWindow::~IupHomeWindow()
{
    if (dialog) {
        IupDestroy(dialog);
        dialog = nullptr;
    }
    if (g_homeWindow == this) {
        g_homeWindow = nullptr;
    }
}

void IupHomeWindow::buildDialog()
{
    // --- Barra de filtro + ações principais ---

    Ihandle* filtroLabel = IupLabel("Filtro:");
    filterText = IupText(nullptr);
    IupSetAttribute(filterText, "SIZE", "150x");
    IupSetAttribute(filterText, "EXPAND", "HORIZONTAL");

    filterButton = IupHelpers::createButton("Filtrar", &IupHomeWindow::onFilterButtonAction, "btn_filtrar");
    insertButton = IupHelpers::createButton("Inserir", &IupHomeWindow::onInsertButtonAction, "btn_inserir");
    exitButton   = IupHelpers::createButton("Sair",    &IupHomeWindow::onExitButtonAction,   "btn_sair");

    Ihandle* topBar = IupHbox(
        filtroLabel,
        filterText,
        filterButton,
        insertButton,
        exitButton,
        nullptr
    );
    IupSetAttribute(topBar, "MARGIN", "5x5");
    IupSetAttribute(topBar, "GAP", "5");

    // --- Tabela (IupMatrix) ---

    matrix = IupMatrix(nullptr);
    IupHelpers::configureDisciplinaMatrix(matrix);

    // Callback de clique na tabela para selecionar linha
    IupSetCallback(matrix, "CLICK_CB", (Icallback)IupHomeWindow::onMatrixClick);

    Ihandle* matrixBox = IupFrame(matrix);
    IupSetAttribute(matrixBox, "TITLE", "Disciplinas");

    // --- Barra de status (CR + autor/info) ---

    crLabel  = IupLabel("CR: -");
    infoLabelAutor = IupLabel("Autor: Mozar Baptista da Silva");
    infoLabelData = IupLabel("11/2025");
    infoLabelURL = IupLabel("www.11tech.com.br");
    IupSetAttribute(infoLabelAutor, "EXPAND", "HORIZONTAL");
    IupSetAttribute(infoLabelData, "EXPAND", "HORIZONTAL");
    //IupSetAttribute(infoLabelURL, "EXPAND", "HORIZONTAL");

    Ihandle* statusBar = IupHbox(
        crLabel,
        IupFill(),
        infoLabelAutor,
        infoLabelData,
        infoLabelURL,
        nullptr
    );
    IupSetAttribute(statusBar, "MARGIN", "5x5");
    IupSetAttribute(statusBar, "GAP", "10");

    // --- Organiza tudo em um VBox ---

    Ihandle* vbox = IupVbox(
        topBar,
        matrixBox,
        statusBar,
        nullptr
    );
    IupSetAttribute(vbox, "MARGIN", "5x5");
    IupSetAttribute(vbox, "GAP", "5");

    dialog = IupDialog(vbox);
    IupSetAttribute(dialog, "TITLE", "HISTORICO ALUNO");

    // Configuração padrão de dialog (centralizado, etc.)
    IupHelpers::configureDialogPlacement(dialog);
    IupSetAttribute(dialog, "SIZE", "800x400");
    //IupSetAttribute(dialog, "RASTERSIZE", "1900x600");

    LOG_INF("IupHomeWindow criada");
}

void IupHomeWindow::refresh(const std::string& filter)
{
    currentFilter = filter;
    selectedRow = 0;
    currentData.clear();

    try {
        LOG_DBG("Recarregando lista de disciplinas");

        auto all = historicoService.list();

        if (currentFilter.empty()) {
            currentData = std::move(all);
        } else {
            std::string f = toLowerCopy(currentFilter);
            currentData.reserve(all.size());
            for (const auto& d : all) {
                std::string nomeLower = toLowerCopy(d.getNome());
                std::string matLower  = toLowerCopy(d.getMatricula());
                std::string anoStr    = toString(d.getAno());
                std::string semStr    = toString(d.getSemestre());

                bool match = false;
                if (nomeLower.find(f) != std::string::npos) {
                    match = true;
                } else if (matLower.find(f) != std::string::npos) {
                    match = true;
                } else if (f == anoStr || f == semStr) {
                    match = true;
                }

                if (match) {
                    currentData.push_back(d);
                }
            }
        }

        // Limpa matrix e preenche com currentData
        IupHelpers::clearMatrixData(matrix);

        int row = 1;
        for (const auto& d : currentData) {
            cout << "nome:" << d.getNome() <<" matricula:" << d.getMatricula()<< " ano:" << d.getAno() << " semestres:" << d.getSemestre() << " creditos:"<<d.getCreditos() << endl;
            IupHelpers::fillDisciplinaRow(matrix, row, d);
            ++row;
         }

        updateCRLabel();
    }
    catch (const InfraError& e) {
        LOG_ERR("erro de infraestrutura ao carregar lista: ", e.what());
        IupMessageDialog::showError("Erro de infraestrutura ao carregar a lista. Tente novamente ou consulte o log.");
    }
    catch (const std::exception& e) {
        LOG_ERR("erro inesperado ao carregar lista: ", e.what());
        IupMessageDialog::showError("Erro inesperado ao carregar a lista. Consulte o log.");
    }
}

void IupHomeWindow::updateCRLabel()
{
    try {
        double cr = historicoService.calculateCR();
        std::string crText = "CR: " + toString(cr);
        IupSetStrf(crLabel, "TITLE", crText.c_str());
    }
    catch (const InfraError& e) {
        LOG_ERR("erro de infraestrutura ao calcular CR: ", e.what());
        IupSetAttribute(crLabel, "TITLE", "CR: erro");
    }
    catch (const std::exception& e) {
        LOG_ERR("erro inesperado ao calcular CR: ", e.what());
        IupSetAttribute(crLabel, "TITLE", "CR: erro");
    }
}

// ==== Ações de UI ====

void IupHomeWindow::handleFilter()
{
    const char* val = IupGetAttribute(filterText, "VALUE");
    std::string f = val ? val : "";
    LOG_INF("Aplicando filtro: ", f);
    refresh(f);
}

void IupHomeWindow::handleInsert()
{
    LOG_INF("Acao Inserir disciplina iniciada");

    Disciplina nova;
    IupDisciplinaFormDialog form(IupDisciplinaFormDialog::Mode::Insert);

    if (!form.showModal(nova)) {
        LOG_DBG("Insercao cancelada pelo usuario");
        return;
    }

    try {
        int newId = historicoService.insert(nova);
        LOG_INF("Disciplina inserida com id=", newId);

        IupMessageDialog::showSuccess("Disciplina inserida com sucesso.");
        // Recarrega mantendo filtro atual
        refresh(currentFilter);
    }
    catch (const BusinessError& e) {
        LOG_ERR("erro de negocio ao inserir disciplina: ", e.what());
        IupMessageDialog::showError(e.what());
    }
    catch (const InfraError& e) {
        LOG_ERR("erro de infra ao inserir disciplina: ", e.what());
        IupMessageDialog::showError("Erro de infraestrutura ao inserir disciplina. Consulte o log.");
    }
    catch (const std::exception& e) {
        LOG_ERR("erro inesperado ao inserir disciplina: ", e.what());
        IupMessageDialog::showError("Erro inesperado ao inserir disciplina. Consulte o log.");
    }
}

void IupHomeWindow::handleEdit()
{
    if (selectedRow <= 0 || selectedRow > static_cast<int>(currentData.size())) {
        IupMessageDialog::showError("Nenhuma disciplina selecionada.");
        return;
    }

    Disciplina original = currentData[selectedRow - 1];
    LOG_INF("Acao Editar disciplina id=", original.getId());

    Disciplina editada;
    IupDisciplinaFormDialog form(IupDisciplinaFormDialog::Mode::Edit, &original);

    if (!form.showModal(editada)) {
        LOG_DBG("Edicao cancelada pelo usuario");
        return;
    }

    try {
        historicoService.update(original.getId(), editada);
        LOG_INF("Disciplina id=", original.getId(), " atualizada com sucesso");

        IupMessageDialog::showSuccess("Disciplina atualizada com sucesso.");
        refresh(currentFilter);
    }
    catch (const BusinessError& e) {
        LOG_ERR("erro de negocio ao atualizar disciplina: ", e.what());
        IupMessageDialog::showError(e.what());
    }
    catch (const InfraError& e) {
        LOG_ERR("erro de infra ao atualizar disciplina: ", e.what());
        IupMessageDialog::showError("Erro de infraestrutura ao atualizar disciplina. Consulte o log.");
    }
    catch (const std::exception& e) {
        LOG_ERR("erro inesperado ao atualizar disciplina: ", e.what());
        IupMessageDialog::showError("Erro inesperado ao atualizar disciplina. Consulte o log.");
    }
}

void IupHomeWindow::handleRemove()
{
    if (selectedRow <= 0 || selectedRow > static_cast<int>(currentData.size())) {
        IupMessageDialog::showError("Nenhuma disciplina selecionada.");
        return;
    }

    Disciplina d = currentData[selectedRow - 1];
    LOG_INF("Acao Remover disciplina id=", d.getId());

    if (!IupConfirmDialog::confirmDisciplinaRemoval(d)) {
        LOG_DBG("Remocao cancelada pelo usuario");
        return;
    }

    try {
        historicoService.remove(d.getId());
        LOG_INF("Disciplina id=", d.getId(), " removida com sucesso");

        IupMessageDialog::showSuccess("Disciplina removida com sucesso.");
        refresh(currentFilter);
    }
    catch (const BusinessError& e) {
        LOG_ERR("erro de negocio ao remover disciplina: ", e.what());
        IupMessageDialog::showError(e.what());
    }
    catch (const InfraError& e) {
        LOG_ERR("erro de infra ao remover disciplina: ", e.what());
        IupMessageDialog::showError("Erro de infraestrutura ao remover disciplina. Consulte o log.");
    }
    catch (const std::exception& e) {
        LOG_ERR("erro inesperado ao remover disciplina: ", e.what());
        IupMessageDialog::showError("Erro inesperado ao remover disciplina. Consulte o log.");
    }
}

void IupHomeWindow::handleExit()
{
    LOG_INF("Usuario solicitou sair da aplicacao");
    // Sai do loop principal do IUP.
    IupExitLoop();
}

void IupHomeWindow::handleRowSelected(int row)
{
    if (row <= 0 || row > static_cast<int>(currentData.size())) {
        selectedRow = 0;
        return;
    }

    selectedRow = row;
    LOG_DBG("Linha selecionada na matriz: ", row,
            " (id=", currentData[row - 1].getId(), ")");
}

// ==== Callbacks estáticos (IUP) ====

int IupHomeWindow::onFilterButtonAction(Ihandle* ih)
{
    (void)ih;
    if (!g_homeWindow) return IUP_DEFAULT;
    g_homeWindow->handleFilter();
    return IUP_DEFAULT;
}

int IupHomeWindow::onInsertButtonAction(Ihandle* ih)
{
    (void)ih;
    if (!g_homeWindow) return IUP_DEFAULT;
    g_homeWindow->handleInsert();
    return IUP_DEFAULT;
}

int IupHomeWindow::onEditButtonAction(Ihandle* ih)
{
    (void)ih;
    if (!g_homeWindow) return IUP_DEFAULT;
    g_homeWindow->handleEdit();
    return IUP_DEFAULT;
}

int IupHomeWindow::onRemoveButtonAction(Ihandle* ih)
{
    (void)ih;
    if (!g_homeWindow) return IUP_DEFAULT;
    g_homeWindow->handleRemove();
    return IUP_DEFAULT;
}

int IupHomeWindow::onExitButtonAction(Ihandle* ih)
{
    (void)ih;
    if (!g_homeWindow) return IUP_DEFAULT;
    g_homeWindow->handleExit();
    return IUP_DEFAULT;
}

// CLICK_CB da IupMatrix: atualiza linha selecionada.
int IupHomeWindow::onMatrixClick(Ihandle* ih, int lin, int col, char* status)
{
    (void)ih;
    (void)status;

    if (!g_homeWindow)
        return IUP_DEFAULT;

    // linha 0 é cabeçalho
    if (lin <= 0)
        return IUP_DEFAULT;

    g_homeWindow->handleRowSelected(lin);

    // colunas 8 e 9 são as de ação
    if (col == 8) {
        g_homeWindow->handleEdit();
    }
    else if (col == 9) {
        g_homeWindow->handleRemove();
    }

    return IUP_DEFAULT;
}
