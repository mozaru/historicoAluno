

# arquitetura.md

```markdown
# Arquitetura do Projeto

## 1. Visão Geral

A arquitetura foi projetada para:

- manter o **domínio limpo e independente** de tecnologia;
- permitir **múltiplas implementações** de interface e persistência;
- manter **um único executável por build**, com:
  - exatamente uma UI ativa;
  - exatamente um repositório ativo;
- servir como exemplo concreto de boas práticas em C++ moderno para ensino de POO e padrões de projeto.

## 2. Camadas e Módulos

### 2.1. Entities

Responsabilidade: representar o modelo de domínio.

Principais classes:

- `Disciplina`
  - campos: id técnico, matrícula, nome, ano, semestre, créditos, nota1, nota2.
  - não acessa I/O, nem conhece detalhes de persistência ou UI.
- `Historico`
  - coleção de `Disciplina` e suporte ao cálculo de CR (ou exposto via service).

Regras de negócio NÃO ficam escondidas aqui; validações centrais são tratadas pelos services.

### 2.2. Repositories

Responsabilidade: prover acesso aos dados.

Interface principal:

- `IDisciplinaRepository`
  - `int insert(const Disciplina&)`
  - `Disciplina get(int id)`
  - `void update(int id, const Disciplina&)`
  - `void remove(int id)`
  - `std::vector<Disciplina> list()`
  - `bool exist(const std::string& matricula, int ano, int semestre)`

Implementações previstas (cada uma em seu subdiretório):

- `csv` – texto separado por delimitador.
- `fixed` – largura fixa de campos.
- `bin` – binário.
- `sqlite` – banco SQLite.
- `json` – arquivo JSON.
- `xml` – arquivo XML.

Decisões:

- O **id é técnico**, usado internamente pelo sistema:
  - em SQLite: PK do banco;
  - em arquivos: índice/ordem carregada em memória.
- A **regra de unicidade** é sempre baseada em `(matricula, ano, semestre)` e aplicada na camada de serviço.
- Erros na persistência:
  - problemas de parse → **Erro de Conversão**;
  - problemas de IO/banco → **Erro de Infra**.

### 2.3. Services

Responsabilidade: encapsular regras de negócio e orquestrar o uso dos repositórios.

Exemplos:

- `HistoricoService`
  - cadastro, alteração, remoção de disciplinas;
  - uso de `IDisciplinaRepository`;
  - validações:
    - matrícula+ano+semestre únicos;
    - ano entre 2000 e ano corrente;
    - semestre em {1, 2};
    - créditos entre 1 e 20;
    - nome entre 3 e 30 caracteres;
    - notas entre 0 e 10;
  - cálculo do coeficiente de rendimento (detalhes documentados no código).

Services lançam **Erro de Negócio** quando regras são quebradas.

### 2.4. UIs (Interfaces de Usuário)

Responsabilidade: interação com o usuário.

Interface principal (conceito):

- `IUI` / `IHistoricoUI`
  - exibe a tela principal do histórico;
  - mostra listagem de disciplinas e CR;
  - oferece opções de inserir, editar, remover, filtrar;
  - usa serviços (não fala direto com repositórios).

Implementações previstas:

- `default` / simples (sem dependências externas).
- terminal puro (std::cin / std::cout).
- `cpp-terminal`.
- `notcurses`.
- `ftxui`.

Regras:

- Em cada build deve existir **apenas uma implementação ativa**.
- A UI é responsável por capturar exceções e:
  - exibir mensagens amigáveis (via telas de alerta/informação);
  - acionar o logger.

### 2.5. Erros / Exceções

Tipos principais:

- **BusinessError** (erro de negócio)
  - violações de regra (dados inválidos etc.).
- **ConversionError**
  - erros de conversão/parse (string ↔ número, parsing de arquivo).
- **InfraError**
  - falhas de infraestrutura (arquivo, banco, libs externas).

Fluxo:

- Services → `BusinessError`.
- Utilitários de conversão → `ConversionError`.
- Repositórios:
  - conversão → `ConversionError`;
  - IO/DB → `InfraError`.
- UI captura, mostra alerta e registra log.

### 2.6. Utilitários de Conversão

Módulo dedicado para:

- `string → int`
- `string → double`
- `int → string`
- `double → string` (sempre com 1 casa decimal)

Erros de conversão lançam `ConversionError`.

### 2.7. Logging

Interface:

- `ILogger`
  - `logInfo(msg)`
  - `logDebug(msg)`
  - `logError(msg)`

Implementações:

- `ConsoleLogger` – escreve na saída padrão.
- `FileLogger` – grava em arquivo:
  - um arquivo por execução (nomeado com data/hora).

Configuração (conceito):

- Em Release:
  - padrão: logger em arquivo;
  - `-v` (verbose): inclui `INFO` e `DEBUG`;
  - sem `-v`: log mínimo (`ERROR`).
- Em Debug:
  - logs mais verbosos liberados.

### 2.8. Registry / Composition Root

`Registry` (singleton simples) concentra:

- registro de fábrica de:
  - `IDisciplinaRepository`
  - `IUI`
  - `ILogger`
- criação das instâncias concretas.

Implementações concretas:

- em seus `.cpp`, registram-se no `Registry` ao serem linkadas.
- se mais de uma UI ou repo for registrada:
  - a última sobrescreve (comportamento definido, mas considerado erro de configuração).

A `main`:

- consulta o `Registry`;
- monta serviços;
- inicia a UI.
- não contém `#ifdef` de seleção tecnológica.

## 3. Requisitos Técnicos

- C++17 ou superior.
- CMake como sistema de build.
- Um único executável por configuração:
  - 1 UI;
  - 1 repositório.
- Compilação:
  - `Debug` com foco em depuração.
  - `Release` com:
    - otimizações;
    - link estático do projeto;
    - uso preferencial de libs estáticas externas quando disponíveis.
- Estrutura modular por diretórios, alinhada às camadas descritas.

## 4. Decisões Técnicas-Chave (resumo)

- Uso de **id técnico** separado da chave de negócio.
- Aplicação de regras de negócio exclusivamente em services.
- Tratamento de erro baseado em exceções tipadas.
- Uso de logger como dependência configurável.
- Uso de um `Registry` para centralizar criação de componentes e manter `main` estável.
- Seleção de UI/repositório feita em nível de build (CMake), não em tempo de execução.
