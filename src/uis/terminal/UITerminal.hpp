#ifndef _UITERMINAL_HPP_
#define _UITERMINAL_HPP_

#include<string>
#include "ILogger.hpp"
#include"Disciplina.hpp"
#include"IUserInterface.hpp"
#include"IHistoricoService.hpp"

class UITerminal : public IUserInterface 
{
    private:
        IHistoricoService& historicoService;
        ILogger& log;
        int menu() const;
        bool editar(Disciplina &disc) const;
        int solicitarId() const;
        void listar() const;
        void mostrarInfo(std::string info) const;
        void mostrarAlerta(std::string info) const;
    public:
        UITerminal(IHistoricoService& aHistoricoService, ILogger& aLog);
        void run();
};

#endif
