#ifndef _IUP_CONFIRM_DIALOG_HPP_
#define _IUP_CONFIRM_DIALOG_HPP_

extern "C" {
#include <iup.h>
}

#include <string>
#include "Disciplina.hpp"

// Helpers para dialog de confirmação de remoção de disciplina.
//
// Segue o layout conceitual da especificação:
//   ID: ...
//   Nome: ...
//   Matrícula: ...
//   Ano/Sem: AAAA / S
//   Média: ...
//
//   Deseja realmente remover esta disciplina?
//
// Botões: [Sim] [Não]
//
// Retorna true se o usuário confirmar (Sim), false caso contrário.

namespace IupConfirmDialog {

    // Exibe um dialog modal perguntando se o usuário deseja remover a disciplina.
    // Retorna:
    //   true  -> usuário clicou "Sim"
    //   false -> usuário clicou "Não" ou fechou o dialog
    bool confirmDisciplinaRemoval(const Disciplina& disciplina);

} // namespace IupConfirmDialog

#endif
