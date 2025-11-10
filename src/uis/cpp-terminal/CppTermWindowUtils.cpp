#include "CppTermWindowUtils.hpp"

#include <algorithm>

#include "cpp-terminal/window.hpp"
#include "cpp-terminal/color.hpp"

namespace CppTermUI
{

CppTermSize getWindowSize(const Term::Window& w)
{
    CppTermSize size;
    size.rows = static_cast<int>(w.rows());
    size.cols = static_cast<int>(w.columns());

    if (size.rows <= 0) size.rows = 24;
    if (size.cols <= 0) size.cols = 80;

    return size;
}

void drawCenteredText(Term::Window& w, int row, const std::string& text)
{
    CppTermSize size = getWindowSize(w);
    if (row < 0 || row >= size.rows)
        return;

    int len = static_cast<int>(text.size());
    int col = (len < size.cols) ? (size.cols - len) / 2 : 0;

    if (col < 0) col = 0;

    // Se o texto for maior que a largura, corta.
    std::string out = text;
    if (static_cast<int>(out.size()) > size.cols)
        out.resize(size.cols);

    w.print_str(static_cast<std::uint32_t>(col),
                static_cast<std::uint32_t>(row),
                out);
}

void drawFrame(Term::Window& w,
               int top,
               int left,
               int width,
               int height)
{
    CppTermSize size = getWindowSize(w);

    if (width < 2 || height < 2)
        return;

    int right  = std::min(left + width  - 1, size.cols - 1);
    int bottom = std::min(top  + height - 1, size.rows - 1);
    if (right <= left || bottom <= top)
        return;

    // linhas horizontais
    for (int x = left; x <= right; ++x) {
        w.print_str(static_cast<std::uint32_t>(x),
                    static_cast<std::uint32_t>(top), "-");
        w.print_str(static_cast<std::uint32_t>(x),
                    static_cast<std::uint32_t>(bottom), "-");
    }

    // linhas verticais
    for (int y = top; y <= bottom; ++y) {
        w.print_str(static_cast<std::uint32_t>(left),
                    static_cast<std::uint32_t>(y), "|");
        w.print_str(static_cast<std::uint32_t>(right),
                    static_cast<std::uint32_t>(y), "|");
    }

    // cantos
    w.print_str(static_cast<std::uint32_t>(left),
                static_cast<std::uint32_t>(top), "+");
    w.print_str(static_cast<std::uint32_t>(right),
                static_cast<std::uint32_t>(top), "+");
    w.print_str(static_cast<std::uint32_t>(left),
                static_cast<std::uint32_t>(bottom), "+");
    w.print_str(static_cast<std::uint32_t>(right),
                static_cast<std::uint32_t>(bottom), "+");
}

void drawCenteredFrame(Term::Window& w,
                       int desiredWidth,
                       int desiredHeight,
                       int& outTop,
                       int& outLeft,
                       int& outWidth,
                       int& outHeight)
{
    CppTermSize size = getWindowSize(w);

    if (desiredWidth > size.cols)
        desiredWidth = size.cols;
    if (desiredHeight > size.rows)
        desiredHeight = size.rows;

    if (desiredWidth < 10)
        desiredWidth = std::min(10, size.cols);
    if (desiredHeight < 5)
        desiredHeight = std::min(5, size.rows);

    outWidth  = desiredWidth;
    outHeight = desiredHeight;
    outTop    = (size.rows - desiredHeight) / 2;
    outLeft   = (size.cols - desiredWidth) / 2;

    if (outTop < 0) outTop = 0;
    if (outLeft < 0) outLeft = 0;

    drawFrame(w, outTop, outLeft, outWidth, outHeight);
}

void drawDialog(Term::Window& w,
                const std::string& titulo,
                const std::vector<std::string>& linhas,
                const std::vector<std::string>& botoes,
                int botaoFocado)
{
    CppTermSize size = getWindowSize(w);

    // Largura baseada no maior entre título, linhas e botões.
    int width = static_cast<int>(titulo.size()) + 4;
    for (const auto& l : linhas)
        width = std::max(width, static_cast<int>(l.size()) + 4);

    int botoesWidth = 0;
    for (const auto& b : botoes)
        botoesWidth += static_cast<int>(b.size()) + 4; // "[  ] " etc
    if (!botoes.empty())
        width = std::max(width, botoesWidth + 4);

    if (width > size.cols)
        width = size.cols - 2;

    // Altura: linhas + título + botões + margens.
    int height = 4 + static_cast<int>(linhas.size()) + 2;
    if (height > size.rows)
        height = size.rows - 2;

    int top, left, realW, realH;
    drawCenteredFrame(w, width, height, top, left, realW, realH);

    // Título
    std::string tituloAjust = titulo;
    if (static_cast<int>(tituloAjust.size()) > realW - 4)
        tituloAjust.resize(realW - 4);

    int titleRow = top;
    int titleCol = left + (realW - static_cast<int>(tituloAjust.size())) / 2;
    if (titleCol < left + 1) titleCol = left + 1;

    w.print_str(static_cast<std::uint32_t>(titleCol),
                static_cast<std::uint32_t>(titleRow),
                tituloAjust);

    // Conteúdo
    int contentStart = top + 2;
    for (std::size_t i = 0; i < linhas.size(); ++i) {
        int row = contentStart + static_cast<int>(i);
        if (row >= top + realH - 3)
            break;

        std::string line = linhas[i];
        if (static_cast<int>(line.size()) > realW - 4)
            line.resize(realW - 4);

        int col = left + 2;
        w.print_str(static_cast<std::uint32_t>(col),
                    static_cast<std::uint32_t>(row),
                    line);
    }

    // Botões
    if (!botoes.empty()) {
        int buttonsRow = top + realH - 2;

        int totalBtnsWidth = 0;
        for (const auto& b : botoes)
            totalBtnsWidth += static_cast<int>(b.size()) + 4;
        totalBtnsWidth += static_cast<int>(botoes.size()) - 1;

        int startCol = left + (realW - totalBtnsWidth) / 2;
        if (startCol < left + 1) startCol = left + 1;

        int col = startCol;
        for (std::size_t i = 0; i < botoes.size(); ++i) {
            std::string label = "[ " + botoes[i] + " ]";
            if (static_cast<int>(label.size()) > realW - 4)
                label.resize(realW - 4);

            // Destaque simples para o botão focado:
            if (static_cast<int>(i) == botaoFocado) {
                // Aqui usamos apenas o próprio label; quem quiser pode usar cores.
                // Como cpp-terminal usa drawing abstrato, fazemos "inversão" manual
                // se necessário em outra evolução.
            }

            w.print_str(static_cast<std::uint32_t>(col),
                        static_cast<std::uint32_t>(buttonsRow),
                        label);

            col += static_cast<int>(label.size()) + 2;
            if (col >= left + realW - 2)
                break;
        }
    }
}

void draw_horizontal_line(Term::Window& w,
                          int row,
                          int colStart,
                          int colEnd)
{
    if (colEnd < colStart) std::swap(colStart, colEnd);
    for (int x = colStart; x <= colEnd; ++x) {
        w.print_str(static_cast<std::uint32_t>(x),
                    static_cast<std::uint32_t>(row), "-");
    }
}

void draw_input_box(Term::Window& w,
                    int x,
                    int y,
                    int width,
                    const std::string& text,
                    bool focused)
{
    if (width < 3) return;

    std::string content = text;
    if ((int)content.size() > width - 2)
        content = content.substr(content.size() - (width - 2));

    std::string box(width, ' ');
    box.front() = '[';
    box.back()  = ']';
    for (int i = 0; i < (int)content.size() && i + 1 < width - 1; ++i) {
        box[1 + i] = content[i];
    }

    if (focused) {
        // Marca foco de forma simples: prefixo ">"
        w.print_str(static_cast<std::uint32_t>(x - 2),
                    static_cast<std::uint32_t>(y), ">");
    }

    w.print_str(static_cast<std::uint32_t>(x),
                static_cast<std::uint32_t>(y),
                box);
}

void draw_button(Term::Window& w,
                 int x,
                 int y,
                 const std::string& label,
                 bool focused)
{
    std::string text = "[";
    text += label;
    text += "]";

    if (focused) {
        // Simples: adiciona ">" antes do botão
        w.print_str(static_cast<std::uint32_t>(x - 2),
                    static_cast<std::uint32_t>(y), ">");
    }

    w.print_str(static_cast<std::uint32_t>(x),
                static_cast<std::uint32_t>(y),
                text);
}

std::string truncate(const std::string& text, std::size_t maxLen)
{
    if (text.size() <= maxLen) return text;
    return text.substr(0, maxLen);
}

void highlight_line(Term::Window& w,
                    int row,
                    int colStart,
                    int colEnd)
{
    if (colEnd < colStart) std::swap(colStart, colEnd);
    // Destaque simples: desenha ">" no início da linha.
    w.print_str(static_cast<std::uint32_t>(colStart),
                static_cast<std::uint32_t>(row),
                ">");
}

void draw_truncated_centered(Term::Window& w,
                             int row,
                             int cols,
                             const std::string& text)
{
    std::string s = text;
    if ((int)s.size() > cols)
        s.resize(cols);
    int col = (cols - static_cast<int>(s.size())) / 2;
    if (col < 0) col = 0;
    w.print_str(static_cast<std::uint32_t>(col),
                static_cast<std::uint32_t>(row), s);
}


} // namespace CppTermUI
