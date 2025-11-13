#include "IupDisciplinaFormDialog.hpp"

#include "IupHelpers.hpp"
#include "IupMessageDialog.hpp"
#include "Conversion.hpp"
#include "Errors.hpp" // para ConversionError

#include <stdexcept>
#include <string>

IupDisciplinaFormDialog::IupDisciplinaFormDialog(Mode mode,
                                                 const Disciplina* initialDisciplina)
    : mode(mode),
      initial(initialDisciplina)
{
    buildDialog();

    if (mode == Mode::Edit && initial != nullptr) {
        fillFromInitial();
    }
}

void IupDisciplinaFormDialog::buildDialog()
{
    // Cria cada linha "Label + Text" usando helpers.
    Ihandle* nomeLine      = IupHelpers::createLabeledText("Nome:",      &nomeText,      "txt_nome",      "lbl_nome");
    Ihandle* matrLine      = IupHelpers::createLabeledText("Matrícula:", &matriculaText, "txt_matricula", "lbl_matr");
    Ihandle* credLine      = IupHelpers::createLabeledText("Créditos:",  &creditosText,  "txt_creditos",  "lbl_cred");
    Ihandle* anoLine       = IupHelpers::createLabeledText("Ano:",       &anoText,       "txt_ano",       "lbl_ano");
    Ihandle* semLine       = IupHelpers::createLabeledText("Semestre:",  &semestreText,  "txt_semestre",  "lbl_sem");
    Ihandle* nota1Line     = IupHelpers::createLabeledText("Nota 1:",    &nota1Text,     "txt_nota1",     "lbl_nota1");
    Ihandle* nota2Line     = IupHelpers::createLabeledText("Nota 2:",    &nota2Text,     "txt_nota2",     "lbl_nota2");

    // Botões OK / Cancelar
    Ihandle* okButton = IupHelpers::createButton("OK",       &IupDisciplinaFormDialog::onOk,     "btn_ok");
    Ihandle* cancelButton = IupHelpers::createButton("Cancelar", &IupDisciplinaFormDialog::onCancel, "btn_cancel");

    Ihandle* buttonsHbox = IupHbox(
        IupFill(),
        okButton,
        IupFill(),
        cancelButton,
        IupFill(),
        nullptr
    );
    IupSetAttribute(buttonsHbox, "ALIGNMENT", "ACENTER");
    IupSetAttribute(buttonsHbox, "MARGIN", "0x10");

    // Agrupa tudo em um VBox
    Ihandle* vbox = IupVbox(
        nomeLine,
        matrLine,
        credLine,
        anoLine,
        semLine,
        nota1Line,
        nota2Line,
        buttonsHbox,
        nullptr
    );

    IupSetAttribute(vbox, "MARGIN", "10x10");
    IupSetAttribute(vbox, "GAP", "4");

    dialog = IupDialog(vbox);

    // Título por modo
    if (mode == Mode::Insert) {
        IupSetAttribute(dialog, "TITLE", "DISCIPLINA - Inserir");
    }
    else {
        IupSetAttribute(dialog, "TITLE", "DISCIPLINA - Editar");
    }

    // Config padrão de dialog (centralizado, etc.)
    IupHelpers::configureDialogPlacement(dialog);
    IupSetAttribute(dialog, "SIZE", "400x220");
}

// Preenche campos a partir de 'initial' (modo edição).
void IupDisciplinaFormDialog::fillFromInitial()
{
    if (!initial) {
        return;
    }

    IupSetAttribute(nomeText,      "VALUE", initial->getNome().c_str());
    IupSetAttribute(matriculaText, "VALUE", initial->getMatricula().c_str());

    IupSetAttribute(creditosText,  "VALUE", toString(initial->getCreditos()).c_str());
    IupSetAttribute(anoText,       "VALUE", toString(initial->getAno()).c_str());
    IupSetAttribute(semestreText,  "VALUE", toString(initial->getSemestre()).c_str());

    IupSetAttribute(nota1Text,     "VALUE", toString(initial->getNota1()).c_str());
    IupSetAttribute(nota2Text,     "VALUE", toString(initial->getNota2()).c_str());
}

bool IupDisciplinaFormDialog::showModal(Disciplina& outDisciplina)
{
    if (!dialog) {
        // Defesa: se por algum motivo buildDialog não foi chamado.
        buildDialog();
        if (mode == Mode::Edit && initial != nullptr) {
            fillFromInitial();
        }
    }

    // Se estivermos em modo edição e houver disciplina inicial, preservamos o ID.
    if (mode == Mode::Edit && initial != nullptr) {
        outDisciplina.setId(initial->getId());
    }
    else {
        outDisciplina.setId(0); // ou deixa para o service atribuir um novo ID técnico.
    }

    // Loop: reabre o formulário se houver erro de conversão.
    while (true) {
        // STATUS padrão -> CANCEL
        IupSetAttribute(dialog, "STATUS", "CANCEL");

        // Exibe o dialog de forma modal.
        IupPopup(dialog, IUP_CENTER, IUP_CENTER);

        const char* status = IupGetAttribute(dialog, "STATUS");
        std::string statusStr = status ? status : "CANCEL";

        if (statusStr != "OK") {
            // Usuário cancelou ou fechou o dialog.
            IupDestroy(dialog);
            dialog = nullptr;
            return false;
        }

        try {
            // Lê valores dos campos.
            const char* nomeVal      = IupGetAttribute(nomeText,      "VALUE");
            const char* matrVal      = IupGetAttribute(matriculaText, "VALUE");
            const char* credVal      = IupGetAttribute(creditosText,  "VALUE");
            const char* anoVal       = IupGetAttribute(anoText,       "VALUE");
            const char* semVal       = IupGetAttribute(semestreText,  "VALUE");
            const char* nota1Val     = IupGetAttribute(nota1Text,     "VALUE");
            const char* nota2Val     = IupGetAttribute(nota2Text,     "VALUE");

            std::string nomeStr  = nomeVal  ? nomeVal  : "";
            std::string matrStr  = matrVal  ? matrVal  : "";
            std::string credStr  = credVal  ? credVal  : "";
            std::string anoStr   = anoVal   ? anoVal   : "";
            std::string semStr   = semVal   ? semVal   : "";
            std::string nota1Str = nota1Val ? nota1Val : "";
            std::string nota2Str = nota2Val ? nota2Val : "";

            // Converte numéricos (pode lançar ConversionError).
            int creditos = toInt(credStr);
            int ano      = toInt(anoStr);
            int semestre = toInt(semStr);
            double n1    = toDouble(nota1Str);
            double n2    = toDouble(nota2Str);

            // Preenche o objeto de saída.
            outDisciplina.setNome(nomeStr);
            outDisciplina.setMatricula(matrStr);
            outDisciplina.setCreditos(creditos);
            outDisciplina.setAno(ano);
            outDisciplina.setSemestre(semestre);
            outDisciplina.setNota1(n1);
            outDisciplina.setNota2(n2);

            // A média é responsabilidade do service ou do repositório?
            // Pela arquitetura, cálculo de CR está no service, mas a média
            // da disciplina pode ser derivada aqui ou no service.
            // Para manter o form neutro, deixamos a média igual à atual
            // (modo edição) ou 0 no modo inserção, e o service decide.
            if (mode == Mode::Edit && initial != nullptr) {
                outDisciplina.setMedia(initial->getMedia());
            }
            else {
                outDisciplina.setMedia(0.0);
            }

            // Se chegou aqui, todas as conversões funcionaram.
            IupDestroy(dialog);
            dialog = nullptr;
            return true;
        }
        catch (const ConversionError& e) {
            // Erro de conversão de entrada do usuário.
            // Mostramos a mensagem e reabrimos o formulário.
            IupMessageDialog::showError(e.what());
            // loop continua, reabrindo o popup com os dados ainda nos campos.
            continue;
        }
        catch (const std::exception& e) {
            // Algum erro inesperado na montagem dos dados do formulário.
            IupMessageDialog::showError(std::string("Erro inesperado ao ler campos: ") + e.what());
            // Por segurança, cancelamos.
            IupDestroy(dialog);
            dialog = nullptr;
            return false;
        }
    }
}

// Callback de clique em OK.
// Seta STATUS = "OK" no dialog e fecha.
int IupDisciplinaFormDialog::onOk(Ihandle* ih)
{
    Ihandle* dlg = IupGetDialog(ih);
    if (dlg) {
        IupSetAttribute(dlg, "STATUS", "OK");
    }
    return IUP_CLOSE;
}

// Callback de clique em Cancelar.
// Seta STATUS = "CANCEL" e fecha.
int IupDisciplinaFormDialog::onCancel(Ihandle* ih)
{
    Ihandle* dlg = IupGetDialog(ih);
    if (dlg) {
        IupSetAttribute(dlg, "STATUS", "CANCEL");
    }
    return IUP_CLOSE;
}
