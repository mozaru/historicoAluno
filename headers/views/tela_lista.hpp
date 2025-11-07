#pragma once

#include <string>
#include "historico.hpp"   // precisa ter Historico / Disciplina
#include "terminal.hpp"    // mesmo usado na TelaFormulario

class TelaLista {
public:
    explicit TelaLista(const Historico& historico_,
                       const std::string& titulo_ = "Historico de Disciplinas");

    // desenha a tela conforme estado atual (scroll, foco, etc)
    void desenhar();

    // processa teclas; retorna false quando deve sair da tela
    bool processarInput();

    // se o usuário confirmou uma escolha (Enter em alguma disciplina)
    int getIndiceSelecionado() const { return indiceSelecionado; }

    // se o usuário saiu com ESC (cancelou)
    bool foiCancelado() const { return cancelado; }

private:
    const Historico& historico;
    std::string titulo;

    int foco = 0;              // índice global da disciplina em foco
    int offset = 0;            // primeira linha visível
    int maxVisiveis = 0;       // calculado a partir do tamanho do terminal

    int indiceSelecionado = -1;
    bool cancelado = false;

    void ajustarJanela();
    void desenharCabecalho(int w);
    void desenharRodape(int w, int h);
};
