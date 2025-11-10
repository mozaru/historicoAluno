# Especificação de Interface de Usuário

**Projeto:** Histórico Aluno
**Contexto:** TUI / “console com janelas” (cpp-terminal, FTXUI, etc.)
**Responsável inicial:** Mozar Baptista da Silva

## 1. Objetivo

Esta especificação define **como a interface de usuário deve se comportar**, independente da biblioteca gráfica ou TUI utilizada, garantindo que qualquer implementação (console avançado, TUI, GUI desktop, web) ofereça a mesma experiência funcional, o mesmo fluxo e respeite os mesmos contratos arquiteturais.

A camada de UI é apenas uma **fronteira de apresentação**. Ela:

1. **Depende exclusivamente de contratos estáveis:**

   * `IUserInterface` – define o ponto de entrada da UI (`run()`), permitindo trocar a implementação por injeção de dependência.
   * `IHistoricoService` – único ponto de acesso às regras de negócio e ao histórico de disciplinas.
   * `ILogger` – mecanismo padrão de logging para auditoria, depuração e rastreabilidade.

2. **Não conhece detalhes de infraestrutura:**

   * Não acessa diretamente repositórios, arquivos, banco de dados ou formato de persistência.
   * Não instancia diretamente implementações concretas de serviço ou repositório (isso é responsabilidade do composition root / `main`).
   * Não faz lógica de negócio relevante (validações, regras de CR, etc. ficam no `IHistoricoService`).

3. **Simula janelas em ambiente texto:**

   * Deve apresentar telas, painéis e dialogs de forma organizada, com:

     * título,
     * área de conteúdo,
     * rodapé,
     * botões e campos interativos.
   * O usuário deve perceber uma **experiência de “aplicação gráfica”**, ainda que em terminal.

4. **Requisitos não funcionais chave:**

   * **Consistência arquitetural**

     * Todas as UIs (cpp-terminal, FTXUI, futura GUI, web) devem seguir este documento.
     * Alterar a tecnologia de UI não pode exigir mudança em `IHistoricoService`, entidades ou repositórios.

   * **Robustez e tratamento de erros**

     * Qualquer falha de validação ou infraestrutura deve:

       * ser interceptada,
       * gerar log adequado,
       * ser apresentada ao usuário em dialog de erro claro,
       * nunca derrubar o processo silenciosamente.
     * Exceções:

       * `BusinessError`: mensagem exibida diretamente ao usuário (erro esperado de regra).
       * `InfraError`: mensagem amigável + detalhe só no log.
       * Outras exceções: mensagem genérica de falha inesperada + log detalhado.

   * **Logging obrigatório**

     * Toda implementação de UI deve registrar eventos relevantes:

       * entrada/saída da aplicação,
       * operações de inserir, editar, remover, filtrar,
       * erros de negócio,
       * erros de infra,
       * entradas inválidas significativas.
     * Uso consistente das macros/níveis (`INF`, `DBG`, `ERR`), no mesmo espírito da `UIConsole`.

   * **Usabilidade mínima**

     * Operável 100% por teclado.
     * Fluxo previsível: sempre retornar à tela principal após operações.
     * Feedback visual claro após sucesso ou erro.
     * Evitar “sumir com informação”: manter lista como contexto.

   * **Portabilidade**

     * Não depender de recursos específicos de um terminal/biblioteca que inviabilizem portar a mesma lógica para outra implementação.

   * **Testabilidade**

     * Organização da lógica de UI de forma que seja possível isolar interação com `IHistoricoService` e `ILogger` (ex.: camadas/objetos intermediários, handlers de eventos, etc.).

Em resumo: a UI é um adaptador. Ela traduz eventos de teclado/mouse em chamadas ao `IHistoricoService`, mostra resultados de forma amigável, registra tudo no `ILogger`, e respeita rigorosamente a separação de responsabilidades.

---

## 2. Estrutura Geral da Interface

A estrutura da UI é baseada em uma **tela principal fixa de listagem** (“Home”) e um conjunto de **dialogs modais**. Isso deve ser mantido em qualquer implementação:

### 2.1 Componentes principais

1. **Tela Principal – Listagem do Histórico (Home)**

   * É o estado base da aplicação.
   * Sempre visível após a conclusão de qualquer operação.
   * Exibe:

     * título,
     * campo de filtro + ações principais,
     * tabela com disciplinas (com scroll),
     * rodapé com créditos.
   * Todas as ações partem daqui: inserir, editar, remover, filtrar.

2. **Dialogs Modais**

   * `Dialog de Inserção/Edição de Disciplina`
   * `Dialog de Confirmação de Remoção`
   * `Dialog de Mensagem de Sucesso`
   * `Dialog de Mensagem de Erro`
   * Enquanto um dialog está aberto:

     * a Home fica “congelada” (sem interação),
     * o foco de navegação é exclusivo do dialog,
     * a saída do dialog sempre retorna para um estado consistente (normalmente a Home).

### 2.2 Papel do `IUserInterface::run()`

A implementação concreta (ex.: `UICppTerminal`) **deve**:

* Herdar de `IUserInterface` e implementar `void run() override`.
* Receber via construtor:

  * uma referência para `IHistoricoService`,
  * uma referência para `ILogger`.
* Implementar internamente o **loop principal da aplicação**, que:

  1. Renderiza a Tela Principal.
  2. Captura entrada (teclado/mouse).
  3. Decide se abre um dialog (inserção, edição, remoção, mensagens).
  4. Invoca `IHistoricoService` conforme ação.
  5. Trata exceções, gera logs e exibe dialogs apropriados.
  6. Retorna à Tela Principal.
  7. Sai do loop apenas quando o usuário escolher encerrar.

Esse loop nunca deve:

* acessar diretamente dados persistidos,
* encapsular regras de negócio que já pertençam ao serviço,
* ignorar erros silenciosamente.

### 2.3 Visão macro em termos de fluxo

Em pseudo-fluxo:

```text
start run()
  -> log "UI iniciada"
  -> while (!sair):
        desenhar Tela Principal (home)
        ler ação do usuário
        se FILTRAR:
            aplicar filtro via service (list ou list filtrado) e redesenhar
        se INSERIR:
            abrir Dialog Inserção -> chamar insert() -> sucesso/erro -> mensagens -> voltar home
        se EDITAR:
            abrir Dialog Edição -> get() -> update() -> sucesso/erro -> mensagens -> voltar home
        se REMOVER:
            abrir Dialog Confirmação -> remove() -> sucesso/erro -> mensagens -> voltar home
        se SAIR:
            sair loop
  -> log "UI encerrada"
end run()
```

## 3. Tela Principal (Home)

### Layout (conceitual)

```text
+--------------------------------------------------------------------------------------+
|                                    HISTORICO ALUNO                                   |
+--------------------------------------------------------------------------------------+
| Filtro: [.................................................] [Filtrar]      [Inserir] |
+--------------------------------------------------------------------------------------+
| ID  Matricula   Nome                           Crd  Ano  Sem  Média   Ações          |
|----+-----------+------------------------------+----+----+----+--------+--------------|
|  1  20210001   Algoritmos                     4    2021 1    8.50    [Editar][Remov] |
|  2  20210001   Estruturas de Dados            4    2021 2    9.00    [Editar][Remov] |
|  ..                                                                  [Editar][Remov] |
+--------------------------------------------------------------------------------------+
|  ^ scroll com ↑/↓ / PgUp/PgDn / (mouse se disponível)                                |
+--------------------------------------------------------------------------------------+
| Autor: Mozar Baptista da Silva | 11/2025 | mozar.silva@gmail.com | www.11tech.com.br |
+--------------------------------------------------------------------------------------+
```

### Comportamento

* **[Filtro]**: texto livre; aplicado em nome, matrícula, ano/semestre conforme implementação.
* `[Filtrar]`: aplica filtro.
* `[Inserir]`: abre dialog de Inserção.
* `[Editar]`: abre dialog de Edição da linha selecionada.
* `[Remov]`: abre dialog de Confirmação.
* Scroll na tabela quando não couber na tela.
* Sempre retornar para esta tela após qualquer operação.

---

## 4. Dialogs

Padrão geral:

```text
+----------------------------------------------------+
|                      TITULO                        |
+----------------------------------------------------+
|                                                    |
|           texto/mensagem/conteúdo aqui             |
|                                                    |
|                 [ Botao1 ] [ Botao2 ]              |
+----------------------------------------------------+
```

Centralizar visualmente dentro da área disponível. Modal: bloqueia a home enquanto aberto.

---

### 4.1 Inserir / Editar Disciplina

```text
+-----------------------------------------------+
|                  DISCIPLINA                   |
+-----------------------------------------------+
| Nome:       [..............................]  |
| Matrícula:  [..............................]  |
| Créditos:   [............]                    |
| Ano:        [............]                    |
| Semestre:   [............]                    |
| Nota 1:     [............]                    |
| Nota 2:     [............]                    |
|                                               |
|              [ OK ]   [ Cancelar ]            |
+-----------------------------------------------+
```

* Inserir → campos vazios.
* Editar → campos preenchidos.
* `OK`:

  * chama `insert` ou `update` em `IHistoricoService`.
  * em sucesso → Dialog Sucesso.
  * em erro → Dialog Erro.
* `Cancelar` ou `Esc` → volta para Home sem alterar.

---

### 4.2 Confirmar Remoção

```text
+------------------------------------------------+
|               REMOVER DISCIPLINA               |
+------------------------------------------------+
| ID: 1                                          |
| Nome: Algoritmos                               |
| Matrícula: 20210001                            |
| Ano/Sem: 2021 / 1                              |
| Média: 8.50                                    |
|                                                |
|   Deseja realmente remover esta disciplina?    |
|                                                |
|               [ Sim ]   [ Não ]                |
+------------------------------------------------+
```

* `[Sim]`:

  * `remove(id)` via `IHistoricoService`.
  * sucesso → Dialog Sucesso.
* `[Não]` ou `Esc` → volta para Home.

---

### 4.3 Mensagem de Sucesso

```text
+-----------------------------------------+
|                 SUCESSO                 |
+-----------------------------------------+
|                                         |
|     Disciplina inserida com sucesso.    |
|                                         |
|                 [ OK ]                  |
+-----------------------------------------+
```

* Após `OK`, volta para Home (lista atualizada).

---

### 4.4 Mensagem de Erro

```text
+-----------------------------------------+
|                  ERRO                   |
+-----------------------------------------+
|                                         |
|   <mensagem amigável do problema>       |
|                                         |
|                 [ OK ]                  |
+-----------------------------------------+
```

* Usar:

  * `BusinessError` → mensagem do erro.
  * `InfraError` / `std::exception` → mensagem genérica + log detalhado.
* Após `OK`, volta para Home.

---

## 5. Navegação

A navegação deve ser consistente em todas as implementações, garantindo uso confortável apenas com teclado, com suporte opcional a mouse quando a biblioteca permitir.

### 5.1 Teclado (obrigatório)

- `Tab` / `Shift+Tab`: alternar entre campos e botões focáveis em dialogs e na barra de ações.
- `↑/↓`:
  - na tabela: navegar entre linhas;
  - em listas ou grupos de opções: mudar seleção.
- `Enter` ou `Space`:
  - ativar o elemento atualmente focado (botão, ação, linha selecionada quando aplicável).
- `Esc`:
  - em dialogs de Inserção/Edição: equivalente a `[Cancelar]`;
  - em dialog de Confirmação: equivalente a `[Não]`;
  - em dialogs de Sucesso ou Erro: equivalente a `[OK]`.
- A partir da Home, a combinação para “Sair” (ex.: tecla dedicada ou opção de menu) pode ser definida por implementação, mas deve ser clara na UI.

### 5.2 Mouse (opcional)

Se a biblioteca suportar:

- Permitir clique em botões.
- Permitir seleção de linhas da tabela.
- Permitir scroll via roda do mouse.

A ausência de suporte a mouse **não** pode prejudicar o uso completo da aplicação: tudo deve ser acessível por teclado.

Faz sentido sim – **desde que seja enxuto, canônico e claramente “exemplo”, não regra**.

O trecho do `UITerminal` que você colou é bom como referência real, mas é grande demais para ficar no meio da especificação. Mistura layout, navegação, widgets específicos etc. O documento de UI deve:

* definir contratos e comportamento;
* mostrar **um padrão recomendado** de uso de `IUserInterface`, `IHistoricoService`, `ILogger`;
* não acoplar à implementação concreta `UITerminal*` ou à API específica de uma lib.

O que eu recomendo:

* manter o documento como está (com os ajustes que fizemos),
* adicionar **uma seção final curta** com um exemplo didático de implementação,
* deixando explícito: “exemplo ilustrativo, não normativo”.

Abaixo vai exatamente o bloco que você pode colar no final do documento como `## 6. Exemplo de Implementação (ilustrativo)`.

---

## 6. Exemplo de Implementação (ilustrativo)

Esta seção não é normativa. O objetivo é ilustrar como uma implementação concreta
de UI deve **herdar de `IUserInterface`**, **consumir `IHistoricoService`**,
**usar `ILogger`** e **aplicar o padrão de tratamento de erros e logs** descrito
nas seções anteriores.

### 6.1 Exemplo de classe UICppTerminal (esqueleto)

```cpp
// UICppTerminal.hpp
#pragma once

#include "IUserInterface.hpp"
#include "IHistoricoService.hpp"
#include "ILogger.hpp"

class UICppTerminal : public IUserInterface {
public:
    UICppTerminal(IHistoricoService& service, ILogger& logger);

    void run() override;

private:
    IHistoricoService& historicoService;
    ILogger& log;

    // Métodos auxiliares (conceituais, nomes podem variar)
    void desenharHome();
    void abrirDialogInserir();
    void abrirDialogEditar(int id);
    void abrirDialogRemover(int id);
    void mostrarMensagemSucesso(const std::string& msg);
    void mostrarMensagemErro(const std::string& msg);
};
```

```cpp
// UICppTerminal.cpp
#include "UICppTerminal.hpp"
#include "Errors.hpp"      // BusinessError, InfraError
// #include <cpp-terminal/...> // inclui a lib concreta aqui

UICppTerminal::UICppTerminal(IHistoricoService& service, ILogger& logger)
    : historicoService(service), log(logger)
{
    // inicialização específica da biblioteca de terminal (se necessário)
    LOG_INF("UICppTerminal criada");
}

void UICppTerminal::run()
{
    LOG_INF("UI iniciada");

    bool sair = false;
    while (!sair) {
        try {
            desenharHome();           // desenha a tela principal (lista + filtro + ações)

            // lê ação do usuário (teclado/mouse) e decide o próximo passo:
            // (pseudo-código, implementação concreta depende da lib usada)
            //
            // acao = lerAcaoUsuario();
            //
            // switch (acao.tipo) {
            //   case Acao::Filtrar:         ... chama historicoService.list(...) ...
            //   case Acao::Inserir:         abrirDialogInserir();      break;
            //   case Acao::Editar:          abrirDialogEditar(acao.id); break;
            //   case Acao::Remover:         abrirDialogRemover(acao.id); break;
            //   case Acao::Sair:            sair = true;              break;
            // }

        }
        catch (const BusinessError& e) {
            LOG_ERR("erro de negocio na UI: ", e.what());
            mostrarMensagemErro(e.what());
        }
        catch (const InfraError& e) {
            LOG_ERR("erro de infra na UI: ", e.what());
            mostrarMensagemErro("Erro de infraestrutura. Consulte o log ou tente novamente.");
        }
        catch (const std::exception& e) {
            LOG_ERR("erro inesperado na UI: ", e.what());
            mostrarMensagemErro("Erro inesperado. Consulte o log.");
        }
    }

    LOG_INF("UI encerrada");
}
```
