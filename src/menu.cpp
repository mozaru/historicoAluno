#include<iostream>

using namespace std;

int menu()
{
    int opc;
    do{
      cout << "\n===== MENU =====\n";
      cout << "1. Inserir disciplina\n";
      cout << "2. Remover disciplina (por matricula)\n";
      cout << "3. Alterar disciplina (por matricula)\n";
      cout << "4. Listar todas as disciplinas\n";
      cout << "5. Mostrar CR do aluno\n";
      cout << "6. Sair\n";
      cout << "Entre com sua escolha: ";
      cin >> opc;
      cin.ignore();
    } while (opc<1 || opc>6);
    return opc;
}
