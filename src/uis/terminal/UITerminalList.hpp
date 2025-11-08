#ifndef _IUITERMINAL_LIST_
#define _IUITERMINAL_LIST_

#include <string>
#include <vector>
#include "IUITerminalForm.hpp"
#include "terminal.hpp"    // mesmo usado na TelaFormulario
#include "Disciplina.hpp"

class UITerminalList : public IUITerminalForm {
    private:
        const std::vector<Disciplina>& lst;
        double cr=0;
        std::string titulo;

        int foco = 0;              // índice global da disciplina em foco
        int offset = 0;            // primeira linha visível
        int maxVisiveis = 0;       // calculado a partir do tamanho do terminal

        int indiceSelecionado = -1;
        bool cancelado = false;

        void ajustarJanela();
        void desenharCabecalho(int w);
        void desenharRodape(int w, int h);
    public:
        explicit UITerminalList(const std::vector<Disciplina>& lst_,double cr_,
                        const std::string& titulo_ = "Historico de Disciplinas");

        // desenha a tela conforme estado atual (scroll, foco, etc)
        void desenhar();

        // processa teclas; retorna false quando deve sair da tela
        bool processarInput();

        // se o usuário confirmou uma escolha (Enter em alguma disciplina)
        int getIndiceSelecionado() const { return indiceSelecionado; }

        // se o usuário saiu com ESC (cancelou)
        bool foiCancelado() const { return cancelado; }
};

#endif