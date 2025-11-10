#ifndef _CPP_TERM_TYPES_HPP_
#define _CPP_TERM_TYPES_HPP_

#include <string>

// Tamanho simples de tela/janela em linhas/colunas.
struct CppTermSize
{
    int rows;
    int cols;
};

// Ações possíveis retornadas pela tela principal (Home).
enum class CppTermHomeAction
{
    None,
    Filter,   // usuário confirmou filtro (usar filtroRetornado)
    Insert,   // inserir nova disciplina
    Edit,     // editar disciplina selecionada (usar disciplinaId)
    Remove,   // remover disciplina selecionada (usar disciplinaId)
    Exit      // sair da aplicação
};

// Resultado padrão da Home.
struct CppTermHomeResult
{
    CppTermHomeAction action{CppTermHomeAction::None};
    std::string filtroRetornado;
    int disciplinaId{-1};
};

// Resultado genérico para dialogs de confirmação.
enum class CppTermConfirmResult
{
    Cancel, // Esc / fechar / não
    No,     // botão Não
    Yes     // botão Sim
};

// Resultado genérico para dialogs de mensagem.
enum class CppTermMessageResult
{
    Ok
};

#endif // _CPP_TERM_TYPES_HPP_
