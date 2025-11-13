# UI IUP – Histórico Acadêmico

Esta pasta contém a implementação da interface de usuário usando **IUP** (Tecgraf/PUC-Rio),
seguindo a especificação da UI do projeto:

- Tela principal fixa (Home) com:
  - filtro;
  - listagem de disciplinas (tabela);
  - ações: Inserir, Editar, Remover, Sair;
  - exibição do CR.
- Dialogs modais:
  - formulário de disciplina (Inserir/Editar);
  - confirmação de remoção;
  - mensagens de sucesso;
  - mensagens de erro.

A UI IUP é mais uma implementação de `IUserInterface`, paralela a `console`, `terminal`,
`cpp-terminal`, etc. A seleção é feita **em tempo de build** via CMake.

---

## Arquivos

- `UIIup.hpp / UIIup.cpp`  
  Implementação de `IUserInterface` usando IUP:
  - inicializa/finaliza IUP (`IupOpen`, `IupClose`);
  - cria a janela principal (`IupHomeWindow`);
  - roda o loop de eventos (`IupMainLoop`);
  - faz logging de entrada/saída da UI e captura exceções de topo.

- `IupHomeWindow.hpp / IupHomeWindow.cpp`  
  Janela principal (“Home”):
  - monta filtro, tabela (`IupMatrix`), botões e barra de status;
  - chama `IHistoricoService` para `list()` e `calculateCR()`;
  - abre os dialogs de inserção, edição, remoção;
  - exibe mensagens de sucesso/erro;
  - registra logs via `ILogger` (macros `LOG_INF`, `LOG_DBG`, `LOG_ERR`).

- `IupDisciplinaFormDialog.hpp / IupDisciplinaFormDialog.cpp`  
  Formulário modal de disciplina (Inserir/Editar):
  - campos: Nome, Matrícula, Créditos, Ano, Semestre, Nota 1, Nota 2;
  - converte strings → tipos (`toInt`, `toDouble`), lança `ConversionError` em erro;
  - preenche um `Disciplina` de saída;
  - **não** aplica regra de negócio (isso é do `IHistoricoService`).

- `IupConfirmDialog.hpp / IupConfirmDialog.cpp`  
  Dialog de confirmação de remoção:
  - mostra ID, Nome, Matrícula, Ano/Sem, Média;
  - pergunta “Deseja realmente remover esta disciplina?”;
  - usa `IupAlarm` com botões `[Sim]` / `[Não]`.

- `IupMessageDialog.hpp / IupMessageDialog.cpp`  
  Dialogs simples de mensagem:
  - `showInfo(title, message)`;
  - `showSuccess(message)` (título fixo `SUCESSO`);
  - `showError(message)` (título fixo `ERRO`).

- `IupHelpers.hpp / IupHelpers.cpp`  
  Helpers de UI:
  - criação de botões padrão;
  - criação de “label + text” em linha;
  - configuração de dialogs (centralização, etc.);
  - configuração de `IupMatrix` para exibir `Disciplina`;
  - preenchimento de linhas da tabela a partir de `Disciplina`.

---

## Dependências

Você precisa ter a biblioteca **IUP** instalada (headers + libs).

Os nomes exatos das libs dependem da sua instalação/plataforma. Exemplos **(apenas ilustrativos)**:

- Em Windows + MinGW (MSYS2), algo como:
  - `iup`, `iupcontrols`, `iupimglib`, etc.
- Em outras plataformas, os nomes podem variar.

Este módulo **não tenta** descobrir automaticamente IUP. Em vez disso, ele espera que você
informe:

- `external\include`  → diretório onde estão os headers (`iup.h`, etc.);
- `external\iup\lib`    → lista de libs a linkar (`iup;iupcontrols;...`).

Essas variáveis são lidas no `CMakeLists.txt` desta pasta.

---

## Habilitando a UI IUP no build

1. Certifique-se de que IUP está instalado e que você sabe:
   - caminho dos headers;
   - nomes e caminho das libs.

2. Configure o CMake com as variáveis necessárias. Exemplo genérico:

`cmake -S . -B build_iup -DCMAKE_BUILD_TYPE=Release -DUI_IMPLEMENTATION=iup -DREPOSITORY_IMPLEMENTATION=memory`

  
