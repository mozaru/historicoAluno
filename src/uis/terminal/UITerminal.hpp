#ifndef _UITERMINAL_HPP_
#define _UITERMINAL_HPP_

#include<string>
#include "ILogger.hpp"
#include"Disciplina.hpp"
#include"IUserInterface.hpp"
#include"IHistoricoService.hpp"

using namespace std;

class UITerminal : public IUserInterface 
{
    private:
        IHistoricoService& historicoService;
        ILogger& log;
        int menu() const;
        bool editar(Disciplina &disc) const;
        int solicitarId() const;
        void listar() const;
        void mostrarInfo(string info) const;
        void mostrarAlerta(string info) const;
    public:
        UITerminal(IHistoricoService& aHistoricoService, ILogger& aLog);
        void run();
};

#endif
