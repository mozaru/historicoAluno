#ifndef _UIFTXUI_HPP_
#define _UIFTXUI_HPP_

#include <memory>

#include "IUserInterface.hpp"
#include "IHistoricoService.hpp"
#include "ILogger.hpp"
#include "Configuracao.hpp"

namespace ui {
namespace ftxuiui {

class UIFtxui : public IUserInterface {
public:
    UIFtxui(IHistoricoService& service, ILogger& logger, const Configuracao& conf);
    ~UIFtxui() override = default;

    void run() override;

private:
    IHistoricoService& historicoService;
    ILogger& log;
};

} // namespace ftxuiui
} // namespace ui

#endif
