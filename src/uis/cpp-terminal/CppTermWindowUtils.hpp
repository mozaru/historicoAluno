#ifndef _CPP_TERM_WINDOW_UTILS_HPP_
#define _CPP_TERM_WINDOW_UTILS_HPP_

#include <string>
#include <vector>

#include "CppTermTypes.hpp"

// Forward declaration para evitar espalhar includes pesados nos headers.
namespace Term {
    class Window;
}

// Funções utilitárias comuns às telas cpp-terminal.
// Não inicializam nem finalizam o terminal: isso é responsabilidade do UICppTerminal.

namespace CppTermUI
{
    // Retorna o tamanho atual do "canvas" disponível a partir de um Window.
    // Se não for possível obter, retorna algo razoável (ex: 24x80).
    CppTermSize getWindowSize(const Term::Window& w);

    // Desenha texto centralizado horizontalmente em uma linha específica.
    void drawCenteredText(Term::Window& w, int row, const std::string& text);

    // Desenha uma moldura retangular simples (semelhante a uma "janela").
    // top/left: posição inicial
    // width/height: dimensões internas aproximadas (ajustadas se ultrapassar limites).
    void drawFrame(Term::Window& w,
                   int top,
                   int left,
                   int width,
                   int height);

    // Calcula posição e desenha uma "janela" centralizada com borda.
    // Retorna via parâmetros de saída a posição/topo/esquerda e dimensões usadas.
    void drawCenteredFrame(Term::Window& w,
                           int desiredWidth,
                           int desiredHeight,
                           int& outTop,
                           int& outLeft,
                           int& outWidth,
                           int& outHeight);

    // Linha horizontal simples (usada na Home).
    void draw_horizontal_line(Term::Window& w,
                              int row,
                              int colStart,
                              int colEnd);

    // Caixa de input simples (texto + foco).
    void draw_input_box(Term::Window& w,
                        int x,
                        int y,
                        int width,
                        const std::string& text,
                        bool focused);

    // Botão simples [ Label ] com indicação de foco.
    void draw_button(Term::Window& w,
                     int x,
                     int y,
                     const std::string& label,
                     bool focused);

    // Trunca string para tamanho máximo.
    std::string truncate(const std::string& text, std::size_t maxLen);

    // Destaque de linha (usado para seleção na tabela).
    void highlight_line(Term::Window& w,
                        int row,
                        int colStart,
                        int colEnd);

    // Texto centralizado com truncamento para caber em cols.
    void draw_truncated_centered(Term::Window& w,
                                 int row,
                                 int cols,
                                 const std::string& text);


    // Desenha um dialog genérico centralizado:
    //  - título na parte superior da moldura,
    //  - linhas de conteúdo no meio,
    //  - botões na parte inferior.
    // Não lê input; apenas desenha.
    void drawDialog(Term::Window& w,
                    const std::string& titulo,
                    const std::vector<std::string>& linhas,
                    const std::vector<std::string>& botoes,
                    int botaoFocado);
}

#endif // _CPP_TERM_WINDOW_UTILS_HPP_
