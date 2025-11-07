#include<iostream>
#include<string>
#include<iomanip>

#include"historico.hpp"
#include"disciplina.hpp"

using namespace std;

Historico::Historico()
{
    qtd = 0;
}
// Destrutor
Historico::~Historico() {}

const Disciplina &Historico::getDisciplina(int pos) const
{
    return vet[pos];
}
int Historico::getQtd() const 
{
    return qtd;
}
const Disciplina& Historico::getDisciplina(string matricula) const
{
    return vet[obterIndice(matricula)];
}

int Historico::obterIndice(string matricula) const
{
    for (int i = 0; i < qtd; i++)
        if (vet[i].getMatricula() == matricula)
            return i;
    return -1; // não encontrada
}

void Historico::inserir(const Disciplina &d)
{
    if (qtd >= 100)
        cout << "Erro: limite de disciplinas atingido.\n";
    else if (obterIndice(d.getMatricula()) != -1)
        cout << "Erro: disciplina com essa matricula ja existe.\n";
    else 
    {
       vet[qtd] = d;
       qtd++;
       cout << "Disciplina inserida com sucesso.\n";
    }
}

// Remover disciplina pela matrícula
void Historico::remover(string matricula)
{
    int idx = obterIndice(matricula);
    if (idx == -1)
        cout << "Disciplina nao encontrada.\n";
    else
    {
       for (int i = idx; i < qtd - 1; i++)
          vet[i] = vet[i + 1];
       qtd--;
       cout << "Disciplina removida com sucesso.\n";
    }
}

// Alterar disciplina existente
void Historico::alterar(string matricula, const Disciplina &d)
{
    int idx = obterIndice(matricula);
    if (idx == -1)
        cout << "Disciplina nao encontrada.\n";
    else
    {
       vet[idx] = d;
       cout << "Disciplina alterada com sucesso.\n";
    }
}

bool Historico::existe(string matricula) const
{
   return obterIndice(matricula)!=-1;
}

// Calcular CR (coeficiente de rendimento)
double Historico::cr() const
{
    double somaPonderada = 0.0;
    int somaCreditos = 0;

    for (int i = 0; i < qtd; i++)
    {
        somaPonderada += vet[i].media() * vet[i].getCreditos();
        somaCreditos += vet[i].getCreditos();
    }

    return (somaCreditos == 0)? 0.0 : somaPonderada / somaCreditos;
}
