#ifndef _IUI_HPP_
#define _IUI_HPP_

#include"disciplina.hpp"
#include"historico.hpp"

class IUserInterface
{
    public:
        virtual int menu() = 0;
        virtual bool editar(Disciplina &disc) = 0;
        virtual string solicitarMatricula() = 0;
        virtual void listar(const Historico &h) = 0;
        virtual void mostrarInfo(string info) = 0;
        virtual void mostrarAlerta(string info) = 0;
};     

#endif
