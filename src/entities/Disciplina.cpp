#include<iostream>
#include<string>
#include<iomanip>

#include"Disciplina.hpp"

using namespace std;

int Disciplina::getId() const 			{ return id; 		}
string Disciplina::getNome() const		{ return nome; 		}
string Disciplina::getMatricula() const	{ return matricula; 	}
int Disciplina::getCreditos() const		{ return creditos; 	}
int Disciplina::getSemestre() const		{ return semestre; 	}
int Disciplina::getAno() const 			{ return ano; 		}
double Disciplina::getNota1() const		{ return nota1; 	}
double Disciplina::getNota2() const		{ return nota2; 	}
double Disciplina::getMedia() const		{ return media; 	}

// Métodos set
void Disciplina::setId(int valor) 	        { id = valor; 		    }
void Disciplina::setNome(string valor) 		{ nome = valor; 	    }
void Disciplina::setMatricula(string valor) { matricula = valor; 	}
void Disciplina::setCreditos(int valor) 	{ creditos = valor; 	}
void Disciplina::setSemestre(int valor) 	{ semestre = valor; 	}
void Disciplina::setAno(int valor) 		    { ano = valor; 		    }
void Disciplina::setNota1(double valor) 	{ nota1 = valor; 	    }
void Disciplina::setNota2(double valor) 	{ nota2 = valor; 	    }
void Disciplina::setMedia(double valor) 	{ media = valor; 	    }

void Disciplina::clear()
{
    nome = matricula = "";
    id = ano = creditos = semestre = 0;
    nota1 = nota2 = media = 0;
}