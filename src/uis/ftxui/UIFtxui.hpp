#ifndef _UIFTXUI_HPP_
#define _UIFTXUI_HPP_

#include <memory>

#include "IUserInterface.hpp"

class IHistoricoService;
class ILogger;

namespace ui {
namespace ftxuiui {

class UIFtxui : public IUserInterface {
public:
    UIFtxui(IHistoricoService& service, ILogger& logger);
    ~UIFtxui() override = default;

    void run() override;

private:
    IHistoricoService& historicoService;
    ILogger& log;
};

} // namespace ftxuiui
} // namespace ui

#endif
