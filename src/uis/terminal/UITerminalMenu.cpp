#include "UITerminalMenu.hpp"
#include "terminal.hpp"
#include <iostream>

UITerminalMenu::UITerminalMenu(const std::string& titulo, const std::vector<std::string>& itens)
    : titulo(titulo), itens(itens) {}

void UITerminalMenu::desenhar() {
    Terminal::clear();
    Terminal::hideCursor();

    int w = Terminal::width();
    int h = Terminal::height();

    int tituloCol = (w - (int)titulo.size()) / 2;
    if (tituloCol < 1) tituloCol = 1;

    Terminal::moveCursor(2, tituloCol);
    std::cout << titulo;

    int startRow = 4;
    for (int i = 0; i < (int)itens.size(); ++i) {
        int row = startRow + i;
        Terminal::moveCursor(row, 4);
        if (i == indice) {
            std::cout << "> " << itens[i];
        } else {
            std::cout << "  " << itens[i];
        }
    }

    Terminal::moveCursor(h, 2);
    std::cout << "[Setas] Navega  [Enter] Seleciona  [Esc] Sair";
    std::cout.flush();
}

bool UITerminalMenu::processarInput() {
    auto ev = Terminal::readKey();
    switch (ev.key) {
        case Key::Up:
            if (indice > 0) indice--;
            break;
        case Key::Down:
            if (indice < (int)itens.size() - 1) indice++;
            break;
        case Key::Enter:
            acaoSelecionada = indice; // mapeie no caller
            return false; // fecha menu
        case Key::Esc:
            acaoSelecionada.reset();
            return false; // cancela
        default:
            break;
    }
    return true; // continua na tela
}
