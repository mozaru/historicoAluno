#ifndef _TELA_HPP_
#define _TELA_HPP_

#include "terminal.hpp"

class Tela {
    public:
        virtual ~Tela() = default;

        virtual void desenhar() = 0;       // desenha a tela inteira
        virtual bool processarInput() = 0; // retorna false se quiser fechar/voltar
};

#endif