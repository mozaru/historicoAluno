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

#include"menu.hpp"
#include"utils.hpp"
#include"disciplina.hpp"
#include"historico.hpp"


using namespace std;


int main()
{
    Historico historico;
    string matricula;
    Disciplina d;
    double resultado;
    bool fim = false;
    do
    {
        switch (menu())
        {
          case 1:
            d.ler();
            historico.inserir(d);
            break;
          case 2:
            cout << "Digite a matricula da disciplina a remover: ";
            getline(cin, matricula);
            if (!historico.existe(matricula))
               cout << "disciplina não encontrada para remover\n";
            else
               historico.remover(matricula);
            break;
          case 3:
            cout << "Digite a matricula da disciplina a alterar: ";
            getline(cin, matricula);
            if (!historico.existe(matricula))
               cout << "disciplina não encontrada para alteracao\n";
            else
            {
               cout << "Digite os novos dados da disciplina:\n";
               d.ler();
               historico.alterar(matricula, d);
            }
            break;
          case 4:
            historico.listar();
            pausar();
            break;
          case 5:
            resultado = historico.cr();
            cout << "CR do aluno: " << fixed << setprecision(2) << resultado << endl;
            pausar();
            break;
          case 6:
            cout << "Encerrando o programa...\n";
            fim = true;
            break;
          default:
            cout << "Opcao invalida!\n";
            break;
        }
    } while (!fim);

    return 0;
}