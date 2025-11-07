#include"UIConsoleMaisOuMenos.hpp"
#include<string>
#include<iostream>
#include<iomanip>

#include "tela_menu.hpp"
#include "tela_formulario.hpp"
#include "tela_message.hpp"
#include "tela_lista.hpp"

using namespace std;

UIConsoleMaisOuMenos::UIConsoleMaisOuMenos()
{
    Terminal::init();
    atexit(Terminal::restore); // garante restauração
}

int UIConsoleMaisOuMenos::menu()
{
    TelaMenu menu("Historico do Aluno", {
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
  return  !acao.has_value()?6:*acao+1;
}

bool UIConsoleMaisOuMenos::editar(Disciplina &d)
{
    std::vector<CampoFormulario> campos = {
        {"Matricula", 20, d.getMatricula()},
        {"Nome", 50, d.getNome()},
        {"Creditos", 2, to_string(d.getCreditos())},
        {"Semestre", 2, to_string(d.getSemestre())},
        {"Ano Ingresso", 4, to_string(d.getAno())},
        {"Nota 1", 5, to_string(d.getNota1())},
        {"Nota 2", 5, to_string(d.getNota2())}
    };

    TelaFormulario tela(d.getMatricula()==""?"Inserir Aluno":"Editar Aluno", campos);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }

    if (tela.foiConfirmado()) {
        const auto& res = tela.getCampos();
        d.setMatricula(res[0].valor);
        d.setNome(res[1].valor);
        d.setCreditos(stoi(res[2].valor));
        d.setSemestre(stoi(res[3].valor));
        d.setAno(stoi(res[4].valor));
        d.setNota1(stod(res[5].valor));
        d.setNota2(stod(res[6].valor));
        return true;
    } else {
        return false;
    }
}
    
string UIConsoleMaisOuMenos::solicitarMatricula()
{
    string matricula;
    std::vector<CampoFormulario> campos = {
        {"Matricula", 20}
    };

    TelaFormulario tela("Solicitacao de Matricula", campos);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }

    if (tela.foiConfirmado()) {
        const auto& res = tela.getCampos();
        return res[0].valor;
    } else {
        return "";
    }
}

void UIConsoleMaisOuMenos::listar(const Historico &h)
{
    TelaLista tela(h, "Historico");
    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }
}
    
void UIConsoleMaisOuMenos::mostrarInfo(string info)
{
    TelaMessage tela("Info", info);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }
}     

void UIConsoleMaisOuMenos::mostrarAlerta(string info)
{
    TelaMessage tela("Erro", info);

    bool rodando = true;
    while (rodando) {
        tela.desenhar();
        rodando = tela.processarInput();
    }
}     


