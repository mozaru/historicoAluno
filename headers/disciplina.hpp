#ifndef _DISCIPLINA_HPP_
#define _DISCIPLINA_HPP_

#include<string>

using namespace std;

class Disciplina
{
   private:
      string nome;
      string matricula;
      int    creditos;
      int    semestre;
      int    ano;
      double nota1;
      double nota2; 
   public:
      string getNome() const; 
      string getMatricula() const; 
      int    getCreditos() const; 
      int    getSemestre() const; 
      int    getAno() const; 
      double getNota1() const; 
      double getNota2() const; 

      void   setNome(string valor); 
      void   setMatricula(string valor); 
      void   setCreditos(int valor); 
      void   setSemestre(int valor); 
      void   setAno(int valor); 
      void   setNota1(double valor); 
      void   setNota2(double valor); 

      double media() const;

      void clear();
};

#endif