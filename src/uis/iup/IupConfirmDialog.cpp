#include "IupConfirmDialog.hpp"
#include "Conversion.hpp"

#include <sstream>

namespace IupConfirmDialog {

    bool confirmDisciplinaRemoval(const Disciplina& disciplina)
    {
        std::ostringstream msg;

        msg << "ID: "        << disciplina.getId()         << "\n"
            << "Nome: "      << disciplina.getNome()       << "\n"
            << "Matrícula: " << disciplina.getMatricula()  << "\n"
            << "Ano/Sem: "   << disciplina.getAno()
            << " / "         << disciplina.getSemestre()   << "\n"
            << "Média: "     << toString(disciplina.getMedia()) << "\n"
            << "\n"
            << "Deseja realmente remover esta disciplina?";

        // IupAlarm cria um dialog modal com até 3 botões.
        // Retornos típicos:
        //   1 -> primeiro botão
        //   2 -> segundo botão
        //   3 -> terceiro botão (se houver)
        //
        // Aqui usamos "Sim" (1) e "Não" (2).
        int ret = IupAlarm(
            "REMOVER DISCIPLINA",   // título
            msg.str().c_str(),     // mensagem
            "Sim",                 // botão 1
            "Não",                 // botão 2
            nullptr                // sem terceiro botão
        );

        return (ret == 1);
    }

} // namespace IupConfirmDialog
