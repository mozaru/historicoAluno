# Sistema de Histórico Acadêmico (C++)

## Visão Geral

Este projeto implementa um **Sistema de Histórico Acadêmico** em C++, pensado como ferramenta didática para ensinar:

- organização de código em camadas;
- separação de responsabilidades;
- aplicação de padrões de projeto na prática;
- tratamento consistente de erros;
- uso de CMake e configuração de build profissional.

O sistema gerencia disciplinas cursadas por um estudante, mantendo:

- cadastro de disciplinas;
- operações de inserção, edição, remoção e listagem;
- validação de regras de negócio;
- cálculo e exibição do coeficiente de rendimento (CR).

## Objetivos Técnicos

- Código organizado em camadas claras:
  - `entities/`
  - `services/`
  - `repositories/`
  - `uis/`
  - `logging/`
  - `core/` ou equivalente para infraestruturas comuns (registry, erros, utilitários).
- Suporte a múltiplas implementações de:
  - interface de usuário (UI);
  - persistência (repositórios).
- **Um único executável** por build, com exatamente:
  - 1 implementação de UI ativa;
  - 1 implementação de repositório ativa.
- Em **Release**:
  - foco em otimização;
  - bibliotecas do projeto linkadas estaticamente;
  - uso preferencial de bibliotecas externas estáticas (quando disponíveis).
- Em **Debug**:
  - foco em depuração;
  - linkagem flexível (dinâmica permitida).
- Tratamento unificado de erros com exceções específicas.
- Log configurável por nível.

Detalhes completos estão em:

- `visao_escopo.md`
- `arquitetura.md`
- `patterns.md`

## Arquitetura (resumo)

A arquitetura é baseada em camadas:

- **Entities**  
  Modelos de domínio (`Disciplina`, `Historico`), com dados e invariantes básicos.

- **Repositories**  
  Interface para acesso a dados (`IDisciplinaRepository`) + implementações específicas  
  (CSV, largura fixa, binário, SQLite, JSON, XML).

- **Services**  
  Camada de regras de negócio e orquestração:
  - validações (matrícula/ano/semestre, notas, créditos, nome);
  - uso dos repositórios;
  - cálculo do CR.

- **UIs (User Interfaces)**  
  Implementam a interação com o usuário a partir de uma interface comum:
  - `default` / simples
  - terminal padrão
  - `cpp-terminal`
  - `notcurses`
  - `ftxui`

- **Erros / Exceções**
  - Erro de negócio
  - Erro de conversão
  - Erro de infraestrutura

- **Logging**
  - Interface de logger com níveis: `INFO`, `DEBUG`, `ERROR`;
  - Implementação em arquivo;
  - Implementação em saída padrão.

- **Registry / Composition Root**
  - Responsável por fornecer instâncias de UI, repositório, serviços e logger;
  - Implementações concretas se registram nele;
  - A `main` fica estável, pedindo objetos ao registry.

Para detalhes e decisões, consulte `arquitetura.md`.

## Preparando o Ambiente de Desenvolvimento

### Requisitos

- CMake ≥ 3.15
- Compilador C++ com suporte a C++17 ou superior:
  - `g++`, `clang++` ou MSVC recente.
- (Opcional, dependendo das UIs/repos escolhidos):
  - `sqlite3` / `sqlite3` dev
  - bibliotecas para:
    - `cpp-terminal`
    - `notcurses`
    - `ftxui`
  - Essas dependências só são necessárias se a respectiva implementação estiver habilitada no build.

### Instalacao do SYS

Fazer o download do msys2 https://www.msys2.org/
sempre executar o atalho MSYS2 MINGW64, existem outros mas são para outros ambientes.

ao entrar no terminal, precisa atualizar tudo com o comando `pacman -Syu`

precisa instalar o gcc, o cmake e make do mingw, para isso execute o comando
`pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make`

agora precisa configurar o mingw como compilador do cmake, entao execute o comando
`cmake -B build -G "MinGW Makefiles"`

para criar as confuburacoes do da aplicacao faça:
`cmake -B build`    
esse cria o diretorio de build no diretorio build


para alterar, atualizar ou criar uma configuracao de build em release, ou seja, para producao:
`cmake -B build -DCMAKE_BUILD_TYPE=Release`

para compilar um configuracao use:
`cmake --build build`

### Estrutura Esperada do Projeto

Na raiz do projeto:

- `CMakeLists.txt`
- `visao_escopo.md`
- `arquitetura.md`
- `patterns.md`
- `README.md`
- diretórios de código (`src/`, `include/`, etc.)

## Como Configurar e Compilar

O projeto utiliza opções de CMake para escolher:

- qual UI será compilada;
- qual repositório será compilado;
- modo Debug/Release;
- comportamento do logger.

Exemplo genérico (ajuste aos nomes reais das opções quando definidos no `CMakeLists.txt`):

```bash
# Criar pasta de build
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DUI_IMPLEMENTATION=terminal \
  -DREPO_IMPLEMENTATION=csv

cmake --build build
```

Para build de Release (com link estático quando possível):

```bash
cmake -S . -B build_release \
  -DCMAKE_BUILD_TYPE=Release \
  -DUI_IMPLEMENTATION=ftxui \
  -DREPO_IMPLEMENTATION=sqlite \
  -DSTATIC_LINK=ON

cmake --build build_release
```

As opções exatas (`UI_IMPLEMENTATION`, `REPO_IMPLEMENTATION`, `STATIC_LINK`) são detalhadas em `arquitetura.md` conforme o `CMakeLists.txt` final.

## Como Executar

Após compilar:

```bash
./build/historico_academico        # ou nome definido no CMake
```

Opções esperadas (conceito):

* `-v` / `--verbose`
  Aumenta o nível de log (INFO/DEBUG), útil para depuração.

O comportamento da UI e do repositório em cada build depende da configuração feita no CMake (apenas uma combinação ativa por executável).

Para o fluxo funcional e exemplos de uso, consulte `arquitetura.md`.

