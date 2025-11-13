#include "IupMessageDialog.hpp"

namespace IupMessageDialog {

    void showInfo(const std::string& title, const std::string& message)
    {
        // IupMessage cria um dialog modal simples com título + texto + OK.
        // Ele espera C-strings, então usamos c_str().
        IupMessage(title.c_str(), message.c_str());
    }

    void showSuccess(const std::string& message)
    {
        // Especificação: título "SUCESSO".
        IupMessage("SUCESSO", message.c_str());
    }

    void showError(const std::string& message)
    {
        // Especificação: título "ERRO".
        // A mensagem já deve vir "amigável" da camada que tratou a exceção
        // (UI ou service). Detalhes técnicos ficam no log, não aqui.
        IupMessage("ERRO", message.c_str());
    }

} // namespace IupMessageDialog
