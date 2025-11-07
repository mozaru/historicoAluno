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


void Disciplina::clear()
{
    nome = matricula = "";
    ano = creditos = semestre = 0;
    nota1=nota2 = 0;
}