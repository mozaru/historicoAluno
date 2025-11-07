#ifndef _TELA_MENU_HPP_
#define _TELA_MENU_HPP_

#include "tela.hpp"
#include <vector>
#include <string>

class TelaMenu : public Tela {
public:
    using Action = int; // você pode mapear depois

    TelaMenu(const std::string& titulo, const std::vector<std::string>& itens);

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
