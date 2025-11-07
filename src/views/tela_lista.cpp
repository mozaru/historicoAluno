#include "tela_lista.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

TelaLista::TelaLista(const Historico& historico_,
                     const std::string& titulo_)
    : historico(historico_), titulo(titulo_) {}

void TelaLista::ajustarJanela() {
    const int h = Terminal::height();

    // Layout:
    // 1: título
    // 2: linha em branco
    // 3: cabeçalho
    // 4: separador
    // ...
    // última-2: linha separador
    // última-1: CR / info
    // última: ajuda
    //
    // Então vamos reservar:
    // 4 linhas topo + 4 linhas rodapé = 8
    // resto é área de dados
    int areaDados = h - 8;
    if (areaDados < 1) areaDados = 1;
    maxVisiveis = areaDados;

    const int qtd = historico.getQtd();
    if (foco < 0) foco = 0;
    if (foco >= qtd && qtd > 0) foco = qtd - 1;

    // garante que foco esteja dentro da janela [offset, offset + maxVisiveis)
    if (foco < offset) {
        offset = foco;
    } else if (foco >= offset + maxVisiveis) {
        offset = foco - maxVisiveis + 1;
    }

    if (offset < 0) offset = 0;
    if (qtd > maxVisiveis && offset > qtd - maxVisiveis) {
        offset = qtd - maxVisiveis;
    }
    if (qtd <= maxVisiveis) {
        offset = 0;
    }
}

void TelaLista::desenharCabecalho(int w) {
    // título centralizado
    int colTitulo = (w - static_cast<int>(titulo.size())) / 2;
    if (colTitulo < 1) colTitulo = 1;

    Terminal::moveCursor(1, colTitulo);
    cout << titulo;

    // cabeçalho tabela (linha 3)
    Terminal::moveCursor(3, 2);
    cout << left
         << setw(12) << "Matricula"
         << setw(25) << "Nome"
         << setw(10) << "Creditos"
         << setw(10) << "Ano"
         << setw(10) << "Semestre"
         << setw(10) << "Media";

    // linha separadora (linha 4)
    Terminal::moveCursor(4, 2);
    cout << string(77, '-');
}

void TelaLista::desenharRodape(int w, int h) {
    const int qtd = historico.getQtd();

    // linha separadora acima do rodape
    int sepRow = h - 3;
    if (sepRow < 5) sepRow = 5;
    Terminal::moveCursor(sepRow, 2);
    cout << string(77, '-');

    // CR e info de paginação
    double cr = (qtd > 0) ? historico.cr() : 0.0;

    Terminal::moveCursor(sepRow + 1, 2);
    cout << "CR do aluno: " << std::fixed << std::setprecision(2) << cr;

    // info página (offset / maxVisiveis)
    if (qtd > maxVisiveis) {
        int paginaAtual = (offset / maxVisiveis) + 1;
        int totalPaginas = (qtd + maxVisiveis - 1) / maxVisiveis;
        std::string pagStr = "Pagina " + std::to_string(paginaAtual)
                           + "/" + std::to_string(totalPaginas);
        int colPag = w - static_cast<int>(pagStr.size()) - 2;
        if (colPag < 2) colPag = 2;
        Terminal::moveCursor(sepRow + 1, colPag);
        cout << pagStr;
    }

    // linha de ajuda
    Terminal::moveCursor(h, 2);
    cout << "[Setas] Navega  [Enter] Seleciona  [Esc] Voltar";
}

void TelaLista::desenhar() {
    Terminal::clear();
    Terminal::hideCursor();

    const int w = Terminal::width();
    const int h = Terminal::height();

    ajustarJanela();
    desenharCabecalho(w);

    const int qtd = historico.getQtd();
    int row = 5; // primeira linha de dados

    int fim = offset + maxVisiveis;
    if (fim > qtd) fim = qtd;

    for (int i = offset; i < fim; ++i) {
        const Disciplina& d = historico.getDisciplina(i);

        Terminal::moveCursor(row, 2);

        bool ativo = (i == foco);

        if (ativo) {
            // marcador simples de seleção; se tiver suporte, aqui pode usar
            // inversão/cor via Terminal, mas sem depender disso.
            cout << ">";
        } else {
            cout << " ";
        }

        cout << left
             << setw(11) << d.getMatricula()    // 11 porque já gastamos 1 com '>' ou ' '
             << setw(25) << d.getNome()
             << setw(10) << d.getCreditos()
             << setw(10) << d.getAno()
             << setw(10) << d.getSemestre()
             << setw(10) << std::fixed << std::setprecision(2) << d.media();

        ++row;
        if (row >= h - 3) break; // segurança extra
    }

    desenharRodape(w, h);
    cout.flush();
}

bool TelaLista::processarInput() {
    auto ev = Terminal::readKey();

    const int qtd = historico.getQtd();
    if (qtd == 0) {
        // nada para navegar; qualquer coisa sai
        cancelado = true;
        return false;
    }

    switch (ev.key) {
        case Key::Up:
            if (foco > 0) {
                foco--;
            } else {
                // opcional: loop para o fim
                foco = 0; // ou: foco = qtd - 1;
            }
            break;

        case Key::Down:
            if (foco < qtd - 1) {
                foco++;
            } else {
                // opcional: loop
                foco = qtd - 1; // ou: foco = 0;
            }
            break;

        // Se quiser page up/down (se existirem no seu Terminal):
        case Key::PageUp:
            foco -= maxVisiveis;
            if (foco < 0) foco = 0;
            break;

        case Key::PageDown:
            foco += maxVisiveis;
            if (foco > qtd - 1) foco = qtd - 1;
            break;

        case Key::Enter:
            indiceSelecionado = foco;
            cancelado = false;
            return false; // sai da tela

        case Key::Esc:
            cancelado = true;
            indiceSelecionado = -1;
            return false;

        default:
            break;
    }

    // após qualquer mudança, ajusta janela e redesenha
    ajustarJanela();
    return true;
}
