#ifndef _IUI_CONSOLE_MAIS_OU_MENOS_HPP_
#define _IUI_CONSOLE_MAIS_OU_MENOS_HPP_

#include<string>
#include"disciplina.hpp"
#include"historico.hpp"
#include"IUI.hpp"

using namespace std;

class UIConsoleMaisOuMenos : public IUserInterface
{
    public:
        UIConsoleMaisOuMenos();
        int menu();
        bool editar(Disciplina &disc);
        string solicitarMatricula();
        void listar(const Historico &h);
        void mostrarInfo(string info);
        void mostrarAlerta(string info);
};     

#endif
