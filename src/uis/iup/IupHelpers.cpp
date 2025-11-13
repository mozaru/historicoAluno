// src/uis/iup/IupHelpers.cpp
#include "IupHelpers.hpp"
#include "Conversion.hpp"

// ---------- helpers genéricos ----------
namespace IupHelpers {
    Ihandle* createButton(const char* title, Icallback actionCallback, const char* name)
    {
        Ihandle* b = IupButton(title, nullptr);
        if (name)
            IupSetAttribute(b, "NAME", name);
        if (actionCallback)
            IupSetCallback(b, "ACTION", actionCallback);
        IupSetAttribute(b, "PADDING", "10x4");
        return b;
    }

    Ihandle* createLabeledText(const char* labelText,
                            Ihandle** outText,
                            const char* textName,
                            const char* labelName)
    {
        Ihandle* lbl = IupLabel(labelText);
        if (labelName)
            IupSetAttribute(lbl, "NAME", labelName);

        Ihandle* txt = IupText(nullptr);
        if (textName)
            IupSetAttribute(txt, "NAME", textName);
        IupSetAttribute(txt, "SIZE", "150x");
        IupSetAttribute(txt, "EXPAND", "HORIZONTAL");

        if (outText)
            *outText = txt;

        Ihandle* hbox = IupHbox(lbl, txt, nullptr);
        IupSetAttribute(hbox, "GAP", "5");
        return hbox;
    }

    void configureDialogPlacement(Ihandle* dialog)
    {
        IupSetAttribute(dialog, "DIALOGFRAME", "YES");
        IupSetAttribute(dialog, "SIZE", "HALFxHALF");
        //IupSetAttribute(dialog, "PLACEMENT", "CENTER");

        // Permitir redimensionar
        //IupSetAttribute(dialog, "RESIZE", "YES");
        //IupSetAttribute(dialog, "MINBOX", "YES");
        //IupSetAttribute(dialog, "MAXBOX", "YES");

    }

    // ---------- Matrix de disciplinas ----------

    static void setCell(Ihandle* m, int lin, int col, const std::string& value)
    {
        // "" indica célula da matrix (não é atributo genérico)
        IupSetStrAttributeId2(m, "", lin, col, value.c_str());
    }

    void configureDisciplinaMatrix(Ihandle* m)
    {
        // 7 colunas de dados + 2 colunas de "botão"
        IupSetAttribute(m, "NUMCOL", "9");
        IupSetAttribute(m, "NUMCOL_VISIBLE", "9");
        IupSetAttribute(m, "NUMLIN", "0");          // sem dados ainda
        IupSetAttribute(m, "NUMLIN_VISIBLE", "15"); // quantas linhas aparecem

        IupSetAttribute(m, "RESIZEMATRIX", "YES");
        IupSetAttribute(m, "SCROLLBAR", "YES");
        IupSetAttribute(m, "READONLY", "YES");

        // Cabeçalho (linha 0, colunas 1..9)
        setCell(m, 0, 1, "ID");
        setCell(m, 0, 2, "Matrícula");
        setCell(m, 0, 3, "Nome");
        setCell(m, 0, 4, "Crd");
        setCell(m, 0, 5, "Ano");
        setCell(m, 0, 6, "Sem");
        setCell(m, 0, 7, "Média");
        setCell(m, 0, 8, "Editar");
        setCell(m, 0, 9, "Remover");

        // larguras das colunas (ajusta como quiser)
        IupSetAttribute(m, "WIDTH1", "30");   // ID
        IupSetAttribute(m, "WIDTH2", "80");   // Matrícula
        IupSetAttribute(m, "WIDTH3", "200");  // Nome
        IupSetAttribute(m, "WIDTH4", "40");   // Crd
        IupSetAttribute(m, "WIDTH5", "60");   // Ano
        IupSetAttribute(m, "WIDTH6", "40");   // Sem
        IupSetAttribute(m, "WIDTH7", "60");   // Média
        IupSetAttribute(m, "WIDTH8", "70");   // Editar
        IupSetAttribute(m, "WIDTH9", "80");   // Remover

        // alinhamento (opcional)
        IupSetAttribute(m, "ALIGNMENT1", "ACENTER");
        IupSetAttribute(m, "ALIGNMENT4", "ACENTER");
        IupSetAttribute(m, "ALIGNMENT5", "ACENTER");
        IupSetAttribute(m, "ALIGNMENT6", "ACENTER");
        IupSetAttribute(m, "ALIGNMENT7", "ACENTER");
        IupSetAttribute(m, "ALIGNMENT8", "ACENTER");
        IupSetAttribute(m, "ALIGNMENT9", "ACENTER");
    }

    void clearMatrixData(Ihandle* m)
    {
        // zera todas as linhas de dados (cabeçalho fica)
        IupSetAttribute(m, "NUMLIN", "0");
    }

    void fillDisciplinaRow(Ihandle* m, int row, const Disciplina& d)
    {
        // Garante que a linha existe
        int currentLines = IupGetInt(m, "NUMLIN");
        if (row > currentLines) {
            IupSetInt(m, "NUMLIN", row);
        }

        // colunas 1..7: dados
        setCell(m, row, 1, toString(d.getId()));
        setCell(m, row, 2, d.getMatricula());
        setCell(m, row, 3, d.getNome());
        setCell(m, row, 4, toString(d.getCreditos()));
        setCell(m, row, 5, toString(d.getAno()));
        setCell(m, row, 6, toString(d.getSemestre()));
        setCell(m, row, 7, toString(d.getMedia()));

        // colunas 8 e 9: “botões” de ação
        setCell(m, row, 8, "[Editar]");
        setCell(m, row, 9, "[Remover]");
    }
}