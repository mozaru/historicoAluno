#ifndef _DISCIPLINA_HPP_
#define _DISCIPLINA_HPP_

#include<string>

class Disciplina
{
   private:
      int    id;
      std::string nome;
      std::string matricula;
      int    creditos;
      int    semestre;
      int    ano;
      double nota1;
      double nota2;
      double media;
   public:
      int    getId() const;
      std::string getNome() const; 
      std::string getMatricula() const; 
      int    getCreditos() const; 
      int    getSemestre() const; 
      int    getAno() const; 
      double getNota1() const; 
      double getNota2() const; 
      double getMedia() const;

      void   setId(int valor);
      void   setNome(std::string valor); 
      void   setMatricula(std::string valor); 
      void   setCreditos(int valor); 
      void   setSemestre(int valor); 
      void   setAno(int valor); 
      void   setNota1(double valor); 
      void   setNota2(double valor); 
      void   setMedia(double valor); 

      void clear();
};

#endif