#include<iostream>
#include<string>
#include<iomanip>

#include"disciplina.hpp"

using namespace std;


string Disciplina::getNome() const		{ return nome; 		}
string Disciplina::getMatricula() const		{ return matricula; 	}
int Disciplina::getCreditos() const		{ return creditos; 	}
int Disciplina::getSemestre() const		{ return semestre; 	}
int Disciplina::getAno() const 			{ return ano; 		}
double Disciplina::getNota1() const		{ return nota1; 	}
double Disciplina::getNota2() const		{ return nota2; 	}

// Métodos set
void Disciplina::setNome(string valor) 		{ nome = valor; 	}
void Disciplina::setMatricula(string valor) 	{ matricula = valor; 	}
void Disciplina::setCreditos(int valor) 	{ creditos = valor; 	}
void Disciplina::setSemestre(int valor) 	{ semestre = valor; 	}
void Disciplina::setAno(int valor) 		{ ano = valor; 		}
void Disciplina::setNota1(double valor) 	{ nota1 = valor; 	}
void Disciplina::setNota2(double valor) 	{ nota2 = valor; 	}

// Cálculo da média
double Disciplina::media() const                { return (nota1 + nota2) / 2.0; }


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

// Leitura dos dados da disciplina
void Disciplina::ler()
{
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
}

// Exibição dos dados
void Disciplina::mostrar() const
{
    cout << left << setw(12) << matricula
         << setw(25) << nome
         << setw(10) << creditos
         << setw(10) << ano
         << setw(10) << semestre
         << setw(10) << fixed << setprecision(2) << media()
         << endl;
}
