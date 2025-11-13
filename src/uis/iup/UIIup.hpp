#ifndef _UI_IUP_HPP_
#define _UI_IUP_HPP_

extern "C" {
#include <iup.h>
}

#include "IUserInterface.hpp"
#include "IHistoricoService.hpp"
#include "ILogger.hpp"
#include "Configuracao.hpp"
#include "IupHomeWindow.hpp"

// Implementação de IUserInterface usando IUP.
//
// Responsabilidades:
// - Inicializar e finalizar a lib IUP.
// - Criar a janela principal (IupHomeWindow).
// - Rodar o loop principal de eventos (IupMainLoop).
// - Garantir logging de entrada/saída e tratamento genérico de erros.

class UIIup : public IUserInterface {
public:
    UIIup(IHistoricoService& service, ILogger& logger, const Configuracao& conf);
    ~UIIup() override = default;

    void run() override;

private:
    IHistoricoService& historicoService;
    ILogger& log;
};

#endif
