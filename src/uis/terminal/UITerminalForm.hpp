#ifndef _UITERMINAL_FORM_HPP_
#define _UITERMINAL_FORM_HPP_

#include "IUITerminalForm.hpp"
#include <string>
#include <vector>

struct CampoFormulario {
    std::string label;
    std::string valor;
    std::size_t maxLen;

    CampoFormulario(const std::string& l, std::size_t maxLen_, const std::string& v = "")
        : label(l), valor(v), maxLen(maxLen_) {}
};

class UITerminalForm : public IUITerminalForm {
public:
    // campos: lista de labels + tamanhos + valores iniciais (para editar)
    UITerminalForm(const std::string& titulo,
                   const std::vector<CampoFormulario>& campos);

    void desenhar() override;
    bool processarInput() override;

    bool foiConfirmado() const { return confirmado; }
    bool foiCancelado() const { return cancelado; }

    const std::vector<CampoFormulario>& getCampos() const { return campos; }

private:
    std::string titulo;
    std::vector<CampoFormulario> campos;

    // foco:
    // 0 .. campos.size()-1 : campos
    // campos.size()        : botao OK
    // campos.size()+1      : botao Cancelar
    int foco = 0;

    bool confirmado = false;
    bool cancelado = false;

    void moverFocoCima();
    void moverFocoBaixo();
    void desenharCampo(int idx, int row, int colLabel, int colValor, bool ativo);
    void desenharBotoes(int row, int colCentro);
};

#endif // TELA_FORMULARIO_HPP
