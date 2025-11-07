#include "tela_formulario.hpp"
#include "terminal.hpp"
#include <iostream>

TelaFormulario::TelaFormulario(const std::string& titulo_,
                               const std::vector<CampoFormulario>& campos_)
    : titulo(titulo_), campos(campos_) {}

void TelaFormulario::desenhar() {
    Terminal::clear();
    Terminal::hideCursor();

    const int w = Terminal::width();
    const int h = Terminal::height();

    // Título centralizado
    int colTitulo = (w - static_cast<int>(titulo.size())) / 2;
    if (colTitulo < 1) colTitulo = 1;
    Terminal::moveCursor(2, colTitulo);
    std::cout << titulo;

    // Layout dos campos
    int startRow = 4;
    int colLabel = w / 2 - 25;
    if (colLabel < 2) colLabel = 2;
    int colValor = colLabel + 20;

    for (int i = 0; i < static_cast<int>(campos.size()); ++i) {
        int row = startRow + i * 2; // espaçamento
        bool ativo = (foco == i);
        desenharCampo(i, row, colLabel, colValor, ativo);
    }

    // Linha dos botões
    int rowBotoes = startRow + static_cast<int>(campos.size()) * 2 + 2;
    if (rowBotoes > h - 3) rowBotoes = h - 3;
    int centro = w / 2;
    desenharBotoes(rowBotoes, centro);

    // Rodapé de ajuda
    Terminal::moveCursor(h, 2);
    std::cout << "[Setas] Navega  [Tab] Proximo  [Enter] Seleciona/Confirma  [Esc] Cancelar";
    std::cout.flush();
}

void TelaFormulario::desenharCampo(int idx, int row,
                                   int colLabel, int colValor,
                                   bool ativo) {
    const auto& c = campos[idx];

    // Label
    Terminal::moveCursor(row, colLabel);
    std::cout << c.label << ":";

    // Caixa de texto simples
    std::string exibido = c.valor;
    // padding visual até maxLen (limitado pra não poluir demais)
    if (exibido.size() < c.maxLen && c.maxLen <= 60) {
        exibido.append(c.maxLen - exibido.size(), ' ');
    }

    Terminal::moveCursor(row, colValor);

    if (ativo) {
        // indicação simples de foco: colchetes
        std::cout << "[" << exibido << "]";
    } else {
        std::cout << " " << exibido << " ";
    }
}

void TelaFormulario::desenharBotoes(int row, int colCentro) {
    // [ OK ] [ Cancelar ]
    std::string okStr = "[ OK ]";
    std::string cancelStr = "[ Cancelar ]";

    int totalLen = static_cast<int>(okStr.size() + 1 + cancelStr.size());
    int startCol = colCentro - totalLen / 2;
    if (startCol < 2) startCol = 2;

    int focoOk = static_cast<int>(campos.size());
    int focoCancel = focoOk + 1;

    // OK
    Terminal::moveCursor(row, startCol);
    if (foco == focoOk) {
        std::cout << ">" << okStr << "<";
    } else {
        std::cout << " " << okStr << " ";
    }

    // espaço
    std::cout << " ";

    // Cancelar
    if (foco == focoCancel) {
        std::cout << ">" << cancelStr << "<";
    } else {
        std::cout << " " << cancelStr << " ";
    }
}

void TelaFormulario::moverFocoCima() {
    int maxIndex = static_cast<int>(campos.size()) + 1;
    if (foco > 0) {
        foco--;
    } else {
        foco = maxIndex; // sobe do topo para o último (cíclico)
    }
}

void TelaFormulario::moverFocoBaixo() {
    int maxIndex = static_cast<int>(campos.size()) + 1;
    if (foco < maxIndex) {
        foco++;
    } else {
        foco = 0; // volta para o primeiro
    }
}

bool TelaFormulario::processarInput() {
    auto ev = Terminal::readKey();

    int focoOk = static_cast<int>(campos.size());
    int focoCancel = focoOk + 1;

    switch (ev.key) {
        case Key::Up:
            moverFocoCima();
            break;

        case Key::Down:
        case Key::Tab:
            moverFocoBaixo();
            break;

        case Key::Left:
            if (foco == focoCancel) foco = focoOk;
            break;

        case Key::Right:
            if (foco == focoOk) foco = focoCancel;
            break;

        case Key::Backspace:
            if (foco >= 0 && foco < static_cast<int>(campos.size())) {
                auto& v = campos[foco].valor;
                if (!v.empty()) v.pop_back();
            }
            break;

        case Key::Character:
            if (foco >= 0 && foco < static_cast<int>(campos.size())) {
                auto& campo = campos[foco];
                if (campo.valor.size() < campo.maxLen) {
                    campo.valor.push_back(ev.ch);
                }
            }
            break;

        case Key::Enter:
            if (foco == focoOk) {
                confirmado = true;
                cancelado = false;
                return false; // fecha tela
            } else if (foco == focoCancel) {
                confirmado = false;
                cancelado = true;
                return false; // fecha tela
            } else {
                // se estiver em um campo, Enter desce para o próximo / OK
                moverFocoBaixo();
            }
            break;

        case Key::Esc:
            confirmado = false;
            cancelado = true;
            return false; // fecha tela

        default:
            break;
    }

    return true; // continua na tela
}
