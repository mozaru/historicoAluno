/*
fazer um programa para gerenciar o histórico de um aluno
o programa deve mostrar um menu com as seguintes opções

1. inserir
2. remover dado o matricula
3. alterar uma disciplina dado a matricula da mesma.
4. listar todas as disciplinas.  (mostrar matricula, nome, créditos, ano, semestre e media)
5. mostrar o cr do aluno.
6. sair

dados da disciplina:
nome:string, matricula:string, créditos:int, semestre:int, ano:int, nota1:real e nota2:real.
media = (nota1+nota2)/2

entidades: 
  disciplina: representa uma disciplina cursada pelo aluno
  histórico:  conjunto de disciplinas cursada pelo aluno
*/

#include<iostream>
#include<string>
#include<iomanip>
#include<sstream>

#include"utils.hpp"
#include"disciplina.hpp"
#include"historico.hpp"

#include"UIConsole.hpp"
#include"UIConsoleTosca.hpp"
#include"UIConsoleMaisOuMenos.hpp"

using namespace std;

int main()
{
    Historico historico;
    string matricula;
    Disciplina d;
    bool fim = false;
    stringstream ss;
    UIConsoleMaisOuMenos GUI;
    do
    {
        switch (GUI.menu())
        {
          case 1:
            d.clear();
            if (GUI.editar(d))
              historico.inserir(d);
            break;
          case 2:
            matricula = GUI.solicitarMatricula();
            if (!historico.existe(matricula))
               GUI.mostrarAlerta("disciplina não encontrada para remover");
            else
               historico.remover(matricula);
            break;
          case 3:
            matricula = GUI.solicitarMatricula();
            if (!historico.existe(matricula))
               GUI.mostrarAlerta("disciplina não encontrada para alteracao");
            else
            {
              d = historico.getDisciplina(matricula);
              if (GUI.editar(d))
                historico.alterar(matricula, d);
            }
            break;
          case 4:
            GUI.listar(historico);
            break;
          case 5:
            ss << "CR do aluno: " << fixed << setprecision(2) << historico.cr();
            GUI.mostrarInfo(ss.str());
            break;
          case 6:
            GUI.mostrarInfo("Encerrando o programa...\n");
            fim = true;
            break;
          default:
            GUI.mostrarAlerta("Opcao invalida!\n");
            break;
        }
    } while (!fim);

    return 0;
}