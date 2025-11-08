#ifndef _UITERMINAL_MENU_HPP_
#define _UITERMINAL_MENU_HPP_

#include "IUITerminalForm.hpp"
#include <vector>
#include <string>

class UITerminalMenu : public IUITerminalForm {
public:
    using Action = int; // você pode mapear depois

    UITerminalMenu(const std::string& titulo, const std::vector<std::string>& itens);

    void desenhar() override;
    bool processarInput() override;

    std::optional<Action> getAcaoSelecionada() const { return acaoSelecionada; }

private:
    std::string titulo;
    std::vector<std::string> itens;
    int indice = 0;
    std::optional<Action> acaoSelecionada;
};

#endif
