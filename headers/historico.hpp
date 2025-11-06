#ifndef _HISTORICO_HPP_
#define _HISTORICO_HPP_

#include<string>

#include"disciplina.hpp"

using namespace std;

class Historico
{
    private:
       Disciplina vet[100];
       int        qtd;
       int obterIndice(string matricula) const; 
    public:
       Historico();
       ~Historico();
       void inserir(const Disciplina &d);
       void remover(string matricula);
       void alterar(string matricula, const Disciplina &d);
       bool existe(string matricula) const;
       double cr() const;
       void listar() const;
};

#endif