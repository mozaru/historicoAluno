#include "UICppTerminal.hpp"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <cctype>

// cpp-terminal
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/options.hpp"

// Componentes de UI específicos
#include "CppTermTypes.hpp"
#include "CppTermHomeScreen.hpp"
#include "CppTermDisciplinaForm.hpp"
#include "CppTermConfirmDialog.hpp"
#include "CppTermMessageDialog.hpp"

#include "Errors.hpp"
#include "Disciplina.hpp"

// ---------------------------------------------------------------------
// Construtor / Destrutor
// ---------------------------------------------------------------------

UICppTerminal::UICppTerminal(IHistoricoService& service, ILogger& logger)
    : historicoService(service),
      log(logger),
      deveSair(false)
{
    try
    {
        // Configuração básica do terminal para TUI.
        // Ajuste conforme a versão da cpp-terminal usada no projeto.
        /*Term::Terminal terminal{
            Term::Option::Raw,
            Term::Option::ClearScreen,
            Term::Option::NoCursor,
            Term::Option::SignalKeys
        };
        (void)terminal;*/
        //Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::SignalKeys, Term::Option::NoCursor, Term::Option::Raw);
        Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw);

        LOG_INF("UICppTerminal inicializada (terminal em modo TUI)");
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Falha ao inicializar terminal para UICppTerminal: ", e.what());
        throw;
    }
}

UICppTerminal::~UICppTerminal()
{
    try
    {
        // Restaura o terminal.
        /*Term::Terminal terminal{
            Term::Option::Cooked,
            Term::Option::Cursor,
            Term::Option::ClearScreen
        };
        (void)terminal;*/
        Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::Cooked, Term::Option::Cursor);
        LOG_INF("UICppTerminal finalizada (terminal restaurado)");
    }
    catch (...)
    {
        // nunca lançar do destrutor
    }
}

// ---------------------------------------------------------------------
// run()
// ---------------------------------------------------------------------

void UICppTerminal::run()
{
    LOG_INF("UICppTerminal::run iniciado");

    try
    {
        mainLoop();
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Excecao nao tratada em UICppTerminal::run: ", e.what());
        std::cerr << "Erro fatal na interface (cpp-terminal): "
                  << e.what() << std::endl;
    }

    LOG_INF("UICppTerminal::run encerrado");
}

// ---------------------------------------------------------------------
// Loop principal
// ---------------------------------------------------------------------

void UICppTerminal::mainLoop()
{
    std::string filtroAtual;
    std::vector<Disciplina> lista;
    std::vector<Disciplina> listaCompleta;
    double cr = 0.0;
    safeList(listaCompleta);
    cr    = safeCalculateCR();
    while (!deveSair)
    {
        // Aqui você pode futuramente ter um list(filtro).
        safeFilter(listaCompleta, filtroAtual,lista);

        // Exibe Home e lê ação do usuário.
        CppTermHomeScreen home(log);
        CppTermHomeResult res = home.show(lista, cr, filtroAtual);

        switch (res.action)
        {
        case CppTermHomeAction::Filter:
            safeList(listaCompleta);
            filtroAtual = res.filtroRetornado;
            break;

        case CppTermHomeAction::Insert:
            handleInserir();
            safeList(listaCompleta);
            break;

        case CppTermHomeAction::Edit:
            if (res.disciplinaId > 0)
            {
                handleEditar(res.disciplinaId);
                safeList(listaCompleta);
            }
            break;
        case CppTermHomeAction::Remove:
            if (res.disciplinaId > 0)
            {
                handleRemover(res.disciplinaId);
                safeList(listaCompleta);
            }
            break;
        case CppTermHomeAction::Exit:
            deveSair = true;
            break;
        case CppTermHomeAction::None:
        default:
            break;
        }
    }
}

// ---------------------------------------------------------------------
// Ações de alto nível chamadas a partir do resultado da Home
// ---------------------------------------------------------------------

void UICppTerminal::handleInserir()
{
    LOG_INF("Acao INSERIR iniciada");

    Disciplina d;
    d.clear();

    CppTermDisciplinaForm form(log);
    const bool confirmado = form.show(d, false); // false = inserção

    if (!confirmado)
    {
        LOG_DBG("Insercao cancelada pelo usuario");
        return;
    }

    safeInsert(d);
}

void UICppTerminal::handleEditar(int idDisciplina)
{
    LOG_INF("Acao EDITAR iniciada id=", idDisciplina);

    Disciplina d;
    if (!tryLoadDisciplina(idDisciplina, d))
        return; // erro já reportado

    CppTermDisciplinaForm form(log);
    const bool confirmado = form.show(d, true); // true = edicao

    if (!confirmado)
    {
        LOG_DBG("Edicao cancelada pelo usuario id=", idDisciplina);
        return;
    }

    safeUpdate(idDisciplina, d);
}

void UICppTerminal::handleRemover(int idDisciplina)
{
    LOG_INF("Acao REMOVER iniciada id=", idDisciplina);

    Disciplina d;
    if (!tryLoadDisciplina(idDisciplina, d))
        return;

    CppTermConfirmDialog confirm(log);

    std::vector<std::string> linhas = {
        "ID: "        + std::to_string(d.getId()),
        "Nome: "      + d.getNome(),
        "Matricula: " + d.getMatricula(),
        "Ano/Sem: "   + std::to_string(d.getAno()) + "/" + std::to_string(d.getSemestre()),
        "Media: "     + std::to_string(d.getMedia()),
        "",
        "Deseja realmente remover esta disciplina?"
    };

    const CppTermConfirmResult r =
        confirm.show("REMOVER DISCIPLINA", linhas);

    if (r != CppTermConfirmResult::Yes)
    {
        LOG_DBG("Remocao cancelada pelo usuario id=", idDisciplina);
        return;
    }

    safeRemove(idDisciplina);
}

// ---------------------------------------------------------------------
// Helpers de integração com service + logs + erros
// ---------------------------------------------------------------------
void UICppTerminal::safeList(std::vector<Disciplina>&listaCompleta)
{
    try
    {
        listaCompleta=historicoService.list();
        LOG_INF("Listar Historico qtd=", listaCompleta.size());
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio list: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra ao listar: ", e.what());
        showError("Erro ao acessar dados do historico.");
        // segue o loop para tentar novamente
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado ao listar: ", e.what());
        showError("Erro inesperado ao carregar dados.");
    }
}




namespace {
    bool containsCaseInsensitive(std::string_view text, std::string_view pattern) {
        if (pattern.empty()) return true;
        if (pattern.size() > text.size()) return false;

        auto it = std::search(
            text.begin(), text.end(),
            pattern.begin(), pattern.end(),
            [](char ch1, char ch2) {
                return std::tolower((unsigned char)ch1) == std::tolower((unsigned char)ch2);
            }
        );

        return it != text.end();
    }
}
void UICppTerminal::safeFilter(const std::vector<Disciplina>&list, const std::string& filter, std::vector<Disciplina>&filtered)
{
    try
    {
        filtered.clear();
        for(const Disciplina& d : list)
           if (containsCaseInsensitive(d.getNome(),filter) || 
               containsCaseInsensitive(d.getMatricula(),filter) ||
               std::to_string(d.getCreditos()).find(filter)!=std::string::npos ||
               std::to_string(d.getAno()).find(filter)!=std::string::npos ||
               std::to_string(d.getSemestre()).find(filter)!=std::string::npos ||
               std::to_string(d.getNota1()).find(filter)!=std::string::npos ||
               std::to_string(d.getNota2()).find(filter)!=std::string::npos ||
               std::to_string(d.getMedia()).find(filter)!=std::string::npos)
               filtered.push_back(d);
        LOG_INF("Filtro ",filter," aplicado qtdTotal=", list.size(), " qtdFiltrado=",filtered.size());
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio filter: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra ao filtrar: ", e.what());
        showError("Erro ao acessar dados do historico.");
        // segue o loop para tentar novamente
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado ao filtrar: ", e.what());
        showError("Erro inesperado ao carregar dados.");
    }
}

double UICppTerminal::safeCalculateCR()
{
    try
    {
        return historicoService.calculateCR();
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio calculateCR: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra ao calcularCR: ", e.what());
        showError("Erro ao acessar calcular o CR do historico.");
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado ao calcular o CR: ", e.what());
        showError("Erro inesperado ao calcular o CR do historico.");
    }    
    return 0;
}

void UICppTerminal::safeInsert(Disciplina& d)
{
    try
    {
        int id = historicoService.insert(d);
        LOG_INF("Insercao concluida id=", id);

        showSuccess("Disciplina inserida com sucesso (id=" + std::to_string(id) + ").");
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio insert: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra insert: ", e.what());
        showError("Erro de infraestrutura ao inserir disciplina.");
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado insert: ", e.what());
        showError("Erro inesperado ao inserir disciplina.");
    }
}

void UICppTerminal::safeUpdate(int id, Disciplina& d)
{
    try
    {
        historicoService.update(id, d);
        LOG_INF("Atualizacao concluida id=", id);

        showSuccess("Disciplina atualizada com sucesso.");
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio update: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra update: ", e.what());
        showError("Erro de infraestrutura ao atualizar disciplina.");
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado update: ", e.what());
        showError("Erro inesperado ao atualizar disciplina.");
    }
}

void UICppTerminal::safeRemove(int id)
{
    try
    {
        historicoService.remove(id);
        LOG_INF("Remocao concluida id=", id);

        showSuccess("Disciplina removida com sucesso.");
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio remove: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra remove: ", e.what());
        showError("Erro de infraestrutura ao remover disciplina.");
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado remove: ", e.what());
        showError("Erro inesperado ao remover disciplina.");
    }
}

bool UICppTerminal::tryLoadDisciplina(int id, Disciplina& out)
{
    try
    {
        out = historicoService.get(id);
        return true;
    }
    catch (const BusinessError& e)
    {
        LOG_ERR("Erro negocio get: ", e.what());
        showError(e.what());
    }
    catch (const InfraError& e)
    {
        LOG_ERR("Erro infra get: ", e.what());
        showError("Erro de infraestrutura ao consultar disciplina.");
    }
    catch (const std::exception& e)
    {
        LOG_ERR("Erro inesperado get: ", e.what());
        showError("Erro inesperado ao consultar disciplina.");
    }
    return false;
}

// ---------------------------------------------------------------------
// Mensagens
// ---------------------------------------------------------------------

void UICppTerminal::showSuccess(const std::string& mensagem)
{
    CppTermMessageDialog dlg(log);
    dlg.show("SUCESSO", mensagem);
}

void UICppTerminal::showError(const std::string& mensagem)
{
    CppTermMessageDialog dlg(log);
    dlg.show("ERRO", mensagem);
}
