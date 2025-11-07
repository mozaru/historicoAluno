#ifndef _TELA_MESSAGE_HPP_
#define _TELA_MESSAGE_HPP_

#include "tela.hpp"
#include <string>
#include <vector>

class TelaMessage : public Tela {
public:
    // campos: lista de labels + tamanhos + valores iniciais (para editar)
    TelaMessage(const std::string& titulo,
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
