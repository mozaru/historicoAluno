#ifndef _UI_CONSOLE_HPP_
#define _UI_CONSOLE_HPP_

#include <string>
#include "ILogger.hpp"
#include "Disciplina.hpp"
#include "IUserInterface.hpp"
#include "IHistoricoService.hpp"
#include "Configuracao.hpp"

class UIConsole : public IUserInterface
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
        UIConsole(IHistoricoService& aHistoricoService, ILogger& aLog, const Configuracao& conf);
        void run();
};     

#endif
