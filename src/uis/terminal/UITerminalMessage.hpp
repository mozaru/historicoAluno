#ifndef _UITERMINAL_MESSAGE_HPP_
#define _UITERMINAL_MESSAGE_HPP_

#include "IUITerminalForm.hpp"
#include <string>
#include <vector>

class UITerminalMessage : public IUITerminalForm {
public:
    // campos: lista de labels + tamanhos + valores iniciais (para editar)
    UITerminalMessage(const std::string& titulo,
                const std::string& message);

    void desenhar() override;
    bool processarInput() override;

private:
    std::string titulo;
    std::vector<std::string> campos;

    bool confirmado = false;
    bool cancelado = false;
};

#endif // TELA_FORMULARIO_HPP
