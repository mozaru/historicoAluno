#ifndef _UI_CPP_TERMINAL_HPP_
#define _UI_CPP_TERMINAL_HPP_

#include <string>

#include "IUserInterface.hpp"
#include "IHistoricoService.hpp"
#include "ILogger.hpp"
#include "Disciplina.hpp"
#include "CppTermTypes.hpp"

// Forward declarations das telas específicas.
// Implementações em arquivos próprios.
class CppTermHomeScreen;
class CppTermDisciplinaForm;
class CppTermConfirmDialog;
class CppTermMessageDialog;

// UICppTerminal
//  - Implementa IUserInterface.
//  - É o coordenador da UI baseada em cpp-terminal.
//  - Usa IHistoricoService como única porta para regras de negócio.
//  - Usa ILogger via macros LOG_INF / LOG_DBG / LOG_ERR.
//  - Delega desenho e interação detalhada para componentes CppTerm*.
class UICppTerminal : public IUserInterface
{
public:
    UICppTerminal(IHistoricoService& service, ILogger& logger);
    virtual ~UICppTerminal();

    void run() override;

private:
    IHistoricoService& historicoService;
    ILogger&           log;

    bool deveSair;

    // Loop principal:
    //  - carrega dados pelo service,
    //  - chama tela Home,
    //  - interpreta resultado,
    //  - aciona forms/dialogs e operações no service,
    //  - trata erros e logs.
    void mainLoop();

    // Ações derivadas da Home:
    void handleInserir();
    void handleEditar(int idDisciplina);
    void handleRemover(int idDisciplina);

    // Helpers de mensagem:
    void showSuccess(const std::string& mensagem);
    void showError(const std::string& mensagem);

    // Helpers de integração com o service (com tratamento de erro + log):
    void safeInsert(Disciplina& d);
    void safeUpdate(int id, Disciplina& d);
    void safeRemove(int id);
    bool tryLoadDisciplina(int id, Disciplina& out);
    double safeCalculateCR();
    void safeList(std::vector<Disciplina>&listaCompleta);
    void safeFilter(const std::vector<Disciplina>&list, const std::string& filter, std::vector<Disciplina>&filtered);
};

#endif // _UI_CPP_TERMINAL_HPP_
