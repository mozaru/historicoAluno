#ifndef _IUP_DISCIPLINA_FORM_DIALOG_HPP_
#define _IUP_DISCIPLINA_FORM_DIALOG_HPP_

extern "C" {
#include <iup.h>
}

#include <string>
#include "Disciplina.hpp"

// Dialog modal para Inserção/Edição de Disciplina.
//
// Campos:
//   Nome
//   Matrícula
//   Créditos
//   Ano
//   Semestre
//   Nota 1
//   Nota 2
//
// A classe não aplica regra de negócio, apenas:
// - coleta as strings dos campos;
// - converte para os tipos corretos via Conversion.hpp;
// - preenche um objeto Disciplina.
//
// Regras de negócio e validações de domínio continuam em IHistoricoService.

class IupDisciplinaFormDialog {
public:
    enum class Mode {
        Insert,
        Edit
    };

    // Se mode == Edit e initialDisciplina != nullptr,
    // os campos serão pré-preenchidos com os dados da disciplina.
    explicit IupDisciplinaFormDialog(Mode mode,
                                     const Disciplina* initialDisciplina = nullptr);

    // Exibe o formulário de forma modal.
    //
    // Retorno:
    //   true  -> usuário confirmou (OK) e outDisciplina foi preenchido.
    //   false -> usuário cancelou ou fechou o dialog.
    //
    // Em caso de erro de conversão:
    //   - é exibida mensagem de erro;
    //   - o formulário é reaberto com os valores preenchidos anteriormente;
    //   - o método só retorna true quando todas as conversões forem bem sucedidas.
    bool showModal(Disciplina& outDisciplina);

private:
    Mode mode;
    const Disciplina* initial;

    // Handles dos widgets principais:
    Ihandle* dialog = nullptr;
    Ihandle* nomeText = nullptr;
    Ihandle* matriculaText = nullptr;
    Ihandle* creditosText = nullptr;
    Ihandle* anoText = nullptr;
    Ihandle* semestreText = nullptr;
    Ihandle* nota1Text = nullptr;
    Ihandle* nota2Text = nullptr;

    // Cria toda a UI do dialog e configura atributos.
    void buildDialog();

    // Preenche os campos a partir de initial (modo edição).
    void fillFromInitial();

    // Callbacks estáticos para botões OK/Cancelar.
    // Signature exigida pelo IUP: int (*)(Ihandle*)
    static int onOk(Ihandle* ih);
    static int onCancel(Ihandle* ih);
};

#endif
