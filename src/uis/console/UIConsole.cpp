#include "UIConsole.hpp"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Errors.hpp"
#include "Disciplina.hpp"
#include "IHistoricoService.hpp"

using namespace std;

UIConsole::UIConsole(IHistoricoService& aHistoricoService, ILogger& aLog)
    : historicoService(aHistoricoService), log(aLog)
{}

void UIConsole::run()
{
    int id;
    Disciplina d;
    bool fim = false;

    LOG_INF("UIConsole iniciada")

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

        case 5: // Mostrar CR
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

        case 6: // Sair
            LOG_INF("UIConsole encerrada pelo usuario")
            mostrarInfo("Encerrando o programa...");
            fim = true;
            break;

        default:
            LOG_DBG("opcao invalida no menu: ", opcao)
            mostrarAlerta("Opcao invalida!");
            break;
        }
    } while (!fim);
}

int UIConsole::menu() const
{
    int opc;
    while (true) {
        cout << "\n===== MENU =====\n";
        cout << "1. Inserir disciplina\n";
        cout << "2. Remover disciplina (por id)\n";
        cout << "3. Alterar disciplina (por id)\n";
        cout << "4. Listar todas as disciplinas\n";
        cout << "5. Mostrar CR do aluno\n";
        cout << "6. Sair\n";
        cout << "Entre com sua escolha: ";

        if (!(cin >> opc)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            LOG_DBG("entrada invalida no menu")
            mostrarAlerta("Entrada invalida. Digite um numero entre 1 e 6.");
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (opc >= 1 && opc <= 6)
            break;

        LOG_DBG("opcao fora do intervalo: ", opc)
        mostrarAlerta("Opcao invalida. Digite um numero entre 1 e 6.");
    }

    return opc;
}

bool UIConsole::editar(Disciplina& d) const
{
    string nome, matricula;
    int ano, semestre, creditos;
    double nota1, nota2;

    cout << "Nome da disciplina: ";
    getline(cin, nome);

    cout << "Matricula: ";
    getline(cin, matricula);

    cout << "Creditos: ";
    while (!(cin >> creditos)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        LOG_DBG("editar: creditos invalidos")
        mostrarAlerta("Creditos invalidos. Digite um numero inteiro.");
        cout << "Creditos: ";
    }

    cout << "Ano: ";
    while (!(cin >> ano)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        LOG_DBG("editar: ano invalido")
        mostrarAlerta("Ano invalido. Digite um numero inteiro.");
        cout << "Ano: ";
    }

    cout << "Semestre: ";
    while (!(cin >> semestre)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        LOG_DBG("editar: semestre invalido")
        mostrarAlerta("Semestre invalido. Digite um numero inteiro.");
        cout << "Semestre: ";
    }

    cout << "Nota1: ";
    while (!(cin >> nota1)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        LOG_DBG("editar: nota1 invalida")
        mostrarAlerta("Nota 1 invalida. Digite um numero.");
        cout << "Nota1: ";
    }

    cout << "Nota2: ";
    while (!(cin >> nota2)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        LOG_DBG("editar: nota2 invalida")
        mostrarAlerta("Nota 2 invalida. Digite um numero.");
        cout << "Nota2: ";
    }

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    d.setNome(nome);
    d.setMatricula(matricula);
    d.setCreditos(creditos);
    d.setNota1(nota1);
    d.setNota2(nota2);
    d.setAno(ano);
    d.setSemestre(semestre);

    LOG_DBG("editar concluido: matricula=", matricula,
            " ano=", ano, " semestre=", semestre)

    return true;
}

int UIConsole::solicitarId() const
{
    cout << "Entre com o id: ";
    int id;
    if (!(cin >> id)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        LOG_ERR("solicitarId: entrada invalida")
        mostrarAlerta("O id deve ser um numero inteiro.");
        return -1;
    }

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    LOG_DBG("solicitarId: informado=", id)
    return id;
}

void UIConsole::listar() const
{
    const auto lst = historicoService.list();
    const double cr = historicoService.calculateCR();

    LOG_DBG("listar: qtd=", lst.size(), " cr=", cr)

    cout << left << setw(3)  << "id"
         << left << setw(12) << "Matricula"
         << setw(25) << "Nome"
         << setw(10) << "Creditos"
         << setw(10) << "Ano"
         << setw(10) << "Semestre"
         << setw(10) << "Media"
         << '\n';

    cout << string(77, '-') << '\n';

    for (const Disciplina& d : lst) {
        cout << right << setw(3)  << d.getId()
             << left  << setw(12) << d.getMatricula()
             << setw(25) << d.getNome()
             << setw(10) << d.getCreditos()
             << setw(10) << d.getAno()
             << setw(10) << d.getSemestre()
             << setw(10) << fixed << setprecision(2) << d.getMedia()
             << '\n';
    }

    cout << string(77, '-') << '\n';
    cout << "CR do aluno: " << fixed << setprecision(2) << cr << '\n';
    cout << string(77, '-') << '\n';
}

void UIConsole::mostrarInfo(string info) const
{
    LOG_INF("UIConsole Info: ", info)
    cout << info << endl;
}

void UIConsole::mostrarAlerta(string info) const
{
    LOG_INF("UIConsole Alerta: ", info)
    cout << "Erro: " << info << endl;
}
