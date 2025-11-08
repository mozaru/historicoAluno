#include "UITerminal.hpp"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "UITerminalMenu.hpp"
#include "UITerminalForm.hpp"
#include "UITerminalMessage.hpp"
#include "UITerminalList.hpp"
#include "Conversion.hpp"
#include "Errors.hpp"

using namespace std;

UITerminal::UITerminal(IHistoricoService& aHistoricoService, ILogger& aLog)
    : historicoService(aHistoricoService), log(aLog)
{
    Terminal::init();
    atexit(Terminal::restore);
}

void UITerminal::run()
{
    int id;
    Disciplina d;
    bool fim = false;

    LOG_INF("UI iniciada")

    do {
        int opcao = menu();
        LOG_DBG("menu selecionado=", opcao)

        switch (opcao) {
        case 1: // Inserir
            d.clear();
            if (editar(d)) {
                try {
                    int novoId = historicoService.insert(d);
                    LOG_INF("insercao concluida id=", novoId)
                    mostrarInfo("Disciplina inserida com sucesso (id=" + std::to_string(novoId) + ")");
                }
                catch (const BusinessError& e) {
                    LOG_ERR("erro negocio insert: ", e.what())
                    mostrarAlerta(e.what());
                }
                catch (const InfraError& e) {
                    LOG_ERR("erro infra insert: ", e.what())
                    mostrarAlerta("Erro de infraestrutura ao inserir disciplina.");
                }
                catch (const std::exception& e) {
                    LOG_ERR("erro inesperado insert: ", e.what())
                    mostrarAlerta("Erro inesperado ao inserir disciplina.");
                }
            } else {
                LOG_DBG("insercao cancelada pelo usuario")
            }
            break;

        case 2: // Remover
            id = solicitarId();
            if (id > 0) {
                try {
                    historicoService.remove(id);
                    LOG_INF("remocao concluida id=", id)
                    mostrarInfo("Disciplina removida com sucesso.");
                }
                catch (const BusinessError& e) {
                    LOG_ERR("erro negocio remove: ", e.what())
                    mostrarAlerta(e.what());
                }
                catch (const InfraError& e) {
                    LOG_ERR("erro infra remove: ", e.what())
                    mostrarAlerta("Erro de infraestrutura ao remover disciplina.");
                }
                catch (const std::exception& e) {
                    LOG_ERR("erro inesperado remove: ", e.what())
                    mostrarAlerta("Erro inesperado ao remover disciplina.");
                }
            } else {
                LOG_DBG("remocao cancelada ou id invalido")
            }
            break;

        case 3: // Alterar
            id = solicitarId();
            if (id > 0) {
                try {
                    d = historicoService.get(id);
                    if (editar(d)) {
                        historicoService.update(id, d);
                        LOG_INF("atualizacao concluida id=", id)
                        mostrarInfo("Disciplina atualizada com sucesso.");
                    } else {
                        LOG_DBG("atualizacao cancelada pelo usuario id=", id)
                    }
                }
                catch (const BusinessError& e) {
                    LOG_ERR("erro negocio update: ", e.what())
                    mostrarAlerta(e.what());
                }
                catch (const InfraError& e) {
                    LOG_ERR("erro infra get/update: ", e.what())
                    mostrarAlerta("Erro de infraestrutura ao consultar/atualizar disciplina.");
                }
                catch (const std::exception& e) {
                    LOG_ERR("erro inesperado update: ", e.what())
                    mostrarAlerta("Erro inesperado ao atualizar disciplina.");
                }
            } else {
                LOG_DBG("update cancelado ou id invalido")
            }
            break;

        case 4: // Listar
            try {
                listar();
            }
            catch (const InfraError& e) {
                LOG_ERR("erro infra listar: ", e.what())
                mostrarAlerta("Erro ao listar historico.");
            }
            catch (const std::exception& e) {
                LOG_ERR("erro inesperado listar: ", e.what())
                mostrarAlerta("Erro inesperado ao listar historico.");
            }
            break;

        case 5: { // Mostrar CR
            try {
                double cr = historicoService.calculateCR();
                LOG_INF("CR calculado: ", cr)
                std::stringstream ss;
                ss << "CR do aluno: " << fixed << setprecision(2) << cr;
                mostrarInfo(ss.str());
            }
            catch (const InfraError& e) {
                LOG_ERR("erro infra calcular CR: ", e.what())
                mostrarAlerta("Erro ao calcular CR.");
            }
            catch (const std::exception& e) {
                LOG_ERR("erro inesperado calcular CR: ", e.what())
                mostrarAlerta("Erro inesperado ao calcular CR.");
            }
            break;
        }

        case 6: // Sair
            LOG_INF("UI encerrada pelo usuario")
            mostrarInfo("Encerrando o programa...\n");
            fim = true;
            break;

        default:
            LOG_DBG("opcao invalida no menu: ", opcao)
            mostrarAlerta("Opcao invalida!\n");
            break;
        }
    } while (!fim);
}

int UITerminal::menu() const
{
    UITerminalMenu menu("Historico do Aluno", {
        "Inserir aluno",
        "Remover aluno",
        "Alterar aluno",
        "Listar historico",
        "Mostrar CR",
        "Sair"
    });

    bool naTela = true;
    while (naTela) {
        menu.desenhar();
        naTela = menu.processarInput();
    }

    auto acao = menu.getAcaoSelecionada();
    return !acao.has_value() ? 6 : *acao + 1;
}

bool UITerminal::editar(Disciplina& d) const
{
    std::vector<CampoFormulario> campos = {
        {"Matricula",    20, d.getMatricula()},
        {"Nome",         50, d.getNome()},
        {"Creditos",      2, to_string(d.getCreditos())},
        {"Semestre",      2, to_string(d.getSemestre())},
        {"Ano Ingresso",  4, to_string(d.getAno())},
        {"Nota 1",        5, to_string(d.getNota1())},
        {"Nota 2",        5, to_string(d.getNota2())}
    };

    UITerminalForm tela(d.getMatricula().empty() ? "Inserir Aluno" : "Editar Aluno", campos);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }

    if (tela.foiConfirmado()) {
        const auto& res = tela.getCampos();
        try {
            d.setMatricula(res[0].valor);
            d.setNome(res[1].valor);
            d.setCreditos(toInt(res[2].valor));
            d.setSemestre(toInt(res[3].valor));
            d.setAno(toInt(res[4].valor));
            d.setNota1(toDouble(res[5].valor));
            d.setNota2(toDouble(res[6].valor));
            LOG_DBG("editar confirmado")
            return true;
        }
        catch (const ConversionError& e) {
            LOG_ERR("editar: erro conversao: ", e.what())
            mostrarAlerta("Valores invalidos no formulario.");
            return false;
        }
        catch (const std::exception& e) {
            LOG_ERR("editar: erro inesperado: ", e.what())
            mostrarAlerta("Erro inesperado ao ler o formulario.");
            return false;
        }
    } else {
        LOG_DBG("editar cancelado pelo usuario")
        return false;
    }
}

int UITerminal::solicitarId() const
{
    try {
        std::vector<CampoFormulario> campos = { {"Id", 20} };

        UITerminalForm tela("Solicitacao Id da disciplina", campos);

        bool rodando = true;
        while (rodando) {
            tela.desenhar();
            rodando = tela.processarInput();
        }

        if (tela.foiConfirmado()) {
            const auto& res = tela.getCampos();
            int id = toInt(res[0].valor);
            LOG_DBG("solicitarId: informado=", id)
            return id;
        } else {
            LOG_DBG("solicitarId: cancelado pelo usuario")
            return -1;
        }
    }
    catch (const ConversionError& e) {
        LOG_ERR("solicitarId: conversao invalida: ", e.what())
        mostrarAlerta("O id deve ser um numero inteiro.");
        return -1;
    }
    catch (const BusinessError& e) {
        LOG_ERR("solicitarId: erro negocio: ", e.what())
        mostrarAlerta(e.what());
        return -1;
    }
    catch (const InfraError& e) {
        LOG_ERR("solicitarId: erro infra: ", e.what())
        mostrarAlerta("Erro de infraestrutura ao ler o id.");
        return -1;
    }
    catch (const std::exception& e) {
        LOG_ERR("solicitarId: erro inesperado: ", e.what())
        mostrarAlerta("Erro inesperado ao ler o id.");
        return -1;
    }
    catch (...) {
        LOG_ERR("solicitarId: erro desconhecido")
        mostrarAlerta("Erro desconhecido ao ler o id.");
        return -1;
    }
}

void UITerminal::listar() const
{
    const auto lst = historicoService.list();
    const double cr = historicoService.calculateCR();
    LOG_DBG("listar: qtd=", lst.size(), " cr=", cr)

    UITerminalList tela(lst, cr, "Historico");
    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }
}

void UITerminal::mostrarInfo(string info) const
{
    LOG_INF("UI INFO: ", info)

    UITerminalMessage tela("Info", info);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }
}

void UITerminal::mostrarAlerta(string info) const
{
    LOG_ERR("UI ALERTA: ", info)

    UITerminalMessage tela("Erro", info);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }
}
