#include "tela_menu.hpp"

int menu()
{
  TelaMenu menu("Historico do Aluno", {
            "Inserir aluno",
            "Remover aluno",
            "Alterar aluno",
            "Listar historico",
            "Mostrar CR",
            "Sair"
        });

  bool naTela = true;
  while (naTela) {
      menu.desenhar();
      naTela = menu.processarInput();
  }

  auto acao = menu.getAcaoSelecionada();
  return  !acao.has_value()?6:*acao+1;
}
