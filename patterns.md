# Padrões de Projeto Utilizados

Este documento lista os padrões aplicados no projeto e indica onde aparecem.

## 1. Repository

**O que é:** abstrai o acesso a dados, expondo operações de alto nível ao invés de detalhes de armazenamento.

**Onde é usado:**

- Interface `IDisciplinaRepository`.
- Implementações:
  - `csv`, `fixed`, `bin`, `sqlite`, `json`, `xml` (cada uma em seu diretório).
- Services usam apenas a interface, não conhecem detalhes de arquivo/banco.

---

## 2. Strategy

**O que é:** permite selecionar algoritmos/comportamentos em tempo de execução (ou build), via uma interface comum e múltiplas implementações.

**Onde é usado:**

- Interfaces com múltiplas implementações:
  - `IDisciplinaRepository` → várias estratégias de persistência.
  - `IUI` → várias estratégias de interface (default, terminal, cpp-terminal, notcurses, ftxui).
  - `ILogger` → log em arquivo ou em console.
- A implementação ativa é determinada pela configuração do build e pelo registro no `Registry`.

---

## 3. Application Service (Service Layer)

**O que é:** camada de serviços que orquestra regras de negócio e coordena repositórios, isolando a lógica de aplicação da infraestrutura.

**Onde é usado:**

- `HistoricoService` (e serviços relacionados):
  - valida dados;
  - aplica regras de negócio (unicidade, faixas válidas, etc.);
  - chama o repositório;
  - expõe operações usadas pelas UIs.

---

## 4. Dependency Injection (manual)

**O que é:** fornecer dependências de fora para dentro (por construtor, setter ou fábrica), ao invés de criá-las internamente.

**Onde é usado:**

- Services recebem `IDisciplinaRepository` por injeção (em vez de criar diretamente).
- UIs recebem services (direta ou indiretamente) prontos para uso.
- `Logger` pode ser injetado nas camadas que precisarem registrar eventos.

Não há framework; a DI é feita manualmente via `Registry`/composition root, apropriado para fins didáticos.

---

## 5. Factory / Abstract Factory

**O que é:** encapsular a criação de objetos, isolando o código cliente dos detalhes de construção.

**Onde é usado:**

- `Registry`:
  - expõe métodos para criar:
    - `IDisciplinaRepository`
    - `IUI`
    - `ILogger`
  - atua como uma fábrica central configurável.
- Implementações concretas registram lambdas/funções de criação no `Registry`.

---

## 6. Service Locator / Registry

**O que é:** ponto central de acesso a serviços compartilhados. Considerado controverso em sistemas grandes, mas útil em cenários didáticos e controlados.

**Onde é usado:**

- `Registry` (singleton):
  - armazena fábricas/instâncias;
  - provê acesso centralizado no `main` (e, se necessário, em outros módulos).
- Uso é documentado explicitamente:
  - simplifica a demonstração de plugabilidade;
  - deixa claro o trade-off entre praticidade e acoplamento.

---

## 7. Exception Handling como Política de Erro

Não é um GoF formal, mas é uma decisão arquitetural importante.

**Onde é usado:**

- `BusinessError`:
  - para violações de regra de negócio.
- `ConversionError`:
  - para falhas de parsing/conversão.
- `InfraError`:
  - para erros de infraestrutura.

A UI captura essas exceções, decide a mensagem ao usuário e registra log adequado.

---

## 8. Logging como Cross-Cutting Concern

Novamente, não é GoF isolado, mas aplica conceitos de separação de interesses + Strategy.

**Onde é usado:**

- `ILogger` (Strategy)
- `FileLogger`, `ConsoleLogger`
- Integração com tratamento de exceções e fluxo da UI.

---

## 9. Domain Model (tocado de forma leve)

**O que é:** modelar conceitos do domínio como objetos ricos.

**Onde é usado:**

- `Disciplina`, `Historico`
- Representam os dados acadêmicos de forma clara, usados como base para os services.

---

## Observação

Todos os padrões foram escolhidos com critério:

- aparecem em problemas reais;
- têm correspondência direta no código;
- evitam inflar o projeto apenas “para dizer que usou GoF”.

O objetivo é que o aluno consiga:
- localizar cada padrão no código;
- entender o porquê da existência dele;
- perceber como a combinação deles produz um sistema flexível e organizado.
