#ifndef _IUI_CONSOLE_HPP_
#define _IUI_CONSOLE_HPP_

#include<string>
#include"disciplina.hpp"
#include"historico.hpp"
#include"IUI.hpp"

using namespace std;

class UIConsole : public IUserInterface
{
    public:
        int menu();
        bool editar(Disciplina &disc);
        string solicitarMatricula();
        void listar(const Historico &h);
        void mostrarInfo(string info);
        void mostrarAlerta(string info);
};     

#endif
