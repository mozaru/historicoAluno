#ifndef _IUP_HELPERS_HPP_
#define _IUP_HELPERS_HPP_

extern "C" {
#include <iup.h>
}

#include <string>
#include "Disciplina.hpp"

// Funções utilitárias para facilitar uso do IUP na UI do Histórico.
// NÃO contêm regra de negócio, apenas ajudam na montagem de telas e widgets.

namespace IupHelpers {

    // Cria um botão padrão com texto e callback de ACTION.
    // name é opcional (útil para debug ou lookup).
    Ihandle* createButton(const char* title,
                          Icallback actionCallback = nullptr,
                          const char* name = nullptr);

    // Configura atributos padrão de um dialog (centrado, etc.).
    // Deve ser chamado ANTES de exibir o dialog (IupShowXY / IupPopup).
    void configureDialogPlacement(Ihandle* dialog);

    // Cria um "label + text" em linha (HBOX) e devolve o Ihandle do container.
    // outText recebe o Ihandle do campo de texto para posterior leitura.
    // textName/labelName são opcionais para facilitar debug.
    Ihandle* createLabeledText(const char* labelText,
                               Ihandle** outText,
                               const char* textName = nullptr,
                               const char* labelName = nullptr);

    // Configura um IupMatrix para exibir a lista de Disciplinas
    // com as colunas:
    // ID | Matrícula | Nome | Crd | Ano | Sem | Média
    //
    // Não preenche linhas, apenas configura cabeçalho e atributos básicos.
    void configureDisciplinaMatrix(Ihandle* matrix);

    // Limpa todas as linhas de dados do IupMatrix, preservando o cabeçalho.
    void clearMatrixData(Ihandle* matrix);

    // Preenche uma linha do IupMatrix com os dados de uma Disciplina.
    // row é 1-based (linha 0 é cabeçalho no IupMatrix).
    // A função garante que NUMLIN >= row.
    void fillDisciplinaRow(Ihandle* matrix, int row, const Disciplina& d);
}

#endif
