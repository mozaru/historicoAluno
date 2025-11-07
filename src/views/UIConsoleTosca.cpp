#include"UIConsoleTosca.hpp"
#include<string>
#include<iostream>
#include<iomanip>

using namespace std;

int UIConsoleTosca::menu()
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

double lerNota(string label)
{
    double nota;
    do {
      cout << label;
      cin >> nota;
      if (nota<0 || nota>10)
        cout << "nota invalida ela deve estar entre 0 e 10" << endl;
    }while (nota<0 || nota>10);
    return nota;
}

bool UIConsoleTosca::editar(Disciplina &d)
{
    string nome,matricula;
    int ano,semestre,creditos;
    double nota1,nota2;
    cout << "Nome da disciplina: ";
    getline(cin, nome);
    cout << "Matricula: ";
    getline(cin, matricula);
    cout << "Creditos: ";
    cin >> creditos;
    cout << "Ano: ";
    cin >> ano;
    cout << "Semestre: ";
    cin >> semestre;
    nota1 = lerNota("Nota1: ");
    nota2 = lerNota("Nota2: ");
    cin.ignore();

    d.setNome(nome);
    d.setMatricula(matricula);
    d.setCreditos(creditos);
    d.setNota1(nota1);
    d.setNota2(nota2);
    d.setAno(ano);
    d.setSemestre(semestre);
    return true;
}
    
string UIConsoleTosca::solicitarMatricula()
{
    string matricula;
    cout << "Entre com a matricula" << endl;
    getline(cin, matricula);
    return matricula;
}

void UIConsoleTosca::listar(const Historico &h)
{
    cout << left << setw(12) << "Matricula"
            << setw(25) << "Nome"
            << setw(10) << "Creditos"
            << setw(10) << "Ano"
            << setw(10) << "Semestre"
            << setw(10) << "Media"
            << endl;
    cout << string(77, '-') << endl;

    for (int i = 0; i < h.getQtd(); i++)
    {
        const Disciplina &d = h.getDisciplina(i);
        cout << left << setw(12) << d.getMatricula()
         << setw(25) << d.getNome()
         << setw(10) << d.getCreditos()
         << setw(10) << d.getAno()
         << setw(10) << d.getSemestre()
         << setw(10) << fixed << setprecision(2) << d.media()
         << endl;
    }
    cout << string(77, '-') << endl;
    cout << "CR do aluno: " << fixed << setprecision(2) << h.cr() << endl;
    cout << string(77, '-') << endl;         
}
    
void UIConsoleTosca::mostrarInfo(string info)
{
    cout << info << endl;
}     

void UIConsoleTosca::mostrarAlerta(string info)
{
    cout << "Erro: " << info << endl;
}     

