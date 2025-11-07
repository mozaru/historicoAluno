#include "tela_message.hpp"
#include "terminal.hpp"
#include <iostream>
#include <string>
#include <vector>


std::vector<std::string> split_newlines(const std::string& s) {
    std::vector<std::string> lines;
    std::string current;

    for (char c : s) {
        if (c == '\n') {
            lines.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    // última linha (mesmo se string não terminar com \n)
    lines.push_back(current);

    return lines;
}

TelaMessage::TelaMessage(const std::string& titulo_,
                         const std::string& campos_)
    : titulo(titulo_), campos(split_newlines(campos_)) {
    }

void TelaMessage::desenhar() {
    Terminal::clear();
    Terminal::hideCursor();

    const int w = Terminal::width();
    const int h = Terminal::height();

    // Título centralizado
    int colTitulo = (w - static_cast<int>(titulo.size())) / 2;
    if (colTitulo < 1) colTitulo = 1;
    Terminal::moveCursor(2, colTitulo);
    std::cout << titulo;

    int maxLen = 0;
    for(const auto v : campos)
    {
       int len = static_cast<int>(v.size());
       if (len>maxLen) maxLen = len;
    }

    // Layout dos campos
    int startRow = 4;
    int col = w / 2 - maxLen / 2;
    if (col < 2) col = 2;

    for (int i = 0; i < static_cast<int>(campos.size()); ++i) {
        int row = startRow + i * 2; // espaçamento
        Terminal::moveCursor(row, col);
        std::string exibido = campos[i];
        std::cout << " " << exibido << " ";
    }

    // Linha dos botões
    std::string okStr = "[ OK ]";

    int totalLen = static_cast<int>(okStr.size());
    int startCol =  w / 2 - totalLen / 2;
    if (startCol < 2) startCol = 2;

    // OK
    Terminal::moveCursor(h-3, startCol);
    std::cout << ">" << okStr << "<";

    // Rodapé de ajuda
    Terminal::moveCursor(h, 2);
    std::cout << "[Enter] Confirma  [Esc] Cancelar";
    std::cout.flush();
}

bool TelaMessage::processarInput() {
    auto ev = Terminal::readKey();

    switch (ev.key) {
        case Key::Enter:
            confirmado = true;
            return false; // fecha tela
        case Key::Esc:
            confirmado = false;
            cancelado = true;
            return false; // fecha tela
        default:
            break;
    }

    return true; // continua na tela
}
