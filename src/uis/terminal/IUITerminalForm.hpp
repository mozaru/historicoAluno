#ifndef _IUITERMINAL_FORM_HPP_
#define _IUITERMINAL_FORM_HPP_

#include "terminal.hpp"

class IUITerminalForm {
    public:
        virtual ~IUITerminalForm() = default;

        virtual void desenhar() = 0;       // desenha a tela inteira
        virtual bool processarInput() = 0; // retorna false se quiser fechar/voltar
};

#endif