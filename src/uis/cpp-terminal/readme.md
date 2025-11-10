# UI cpp-terminal – Histórico Aluno

Esta pasta contém a implementação da interface de usuário baseada em
[`cpp-terminal`](https://github.com/jupyter-xeus/cpp-terminal) para o projeto **Histórico Aluno**.

A ideia é demonstrar uma TUI no terminal com “janelas”, diálogos e formulários,
respeitando a arquitetura do projeto:

- UI depende apenas de:
  - `IUserInterface`
  - `IHistoricoService`
  - `ILogger`
- Nada de acesso direto a repositório ou infraestrutura.
- Toda navegação/tela é apenas adaptação visual em cima do domínio.

Este guia cobre:

1. Como obter o `cpp-terminal`
2. Como configurar o CMake do projeto
3. Como compilar usando a UI `UICppTerminal`
4. Como executar

---

## 1. Dependências

### 1.1. Ferramentas

Em geral:

- CMake >= 3.16
- Compilador C++ com suporte a C++17

### 1.2. MSYS2 / MinGW-w64 (Windows)

No terminal **MSYS2 MinGW 64-bit**:

```bash
pacman -Syu
pacman -S --needed \
  mingw-w64-x86_64-toolchain \
  mingw-w64-x86_64-cmake \
  git
```

Use sempre o shell `mingw64.exe` para configurar/compilar o projeto.

---

## 2. Obtendo o `cpp-terminal`

O projeto assume o `cpp-terminal` como subdiretório em `external/cpp-terminal`.

Na raiz do `historicoAluno`:

```bash
git submodule add https://github.com/jupyter-xeus/cpp-terminal external/cpp-terminal
# ou, se não quiser submodule:
# git clone https://github.com/jupyter-xeus/cpp-terminal external/cpp-terminal
```

Estrutura esperada:

```text
historicoAluno/
  external/
    cpp-terminal/
      CMakeLists.txt
      cpp-terminal/
        ...
  src/
    uis/
      cpp-terminal/
        UICppTerminal.hpp/.cpp
        CppTermHomeScreen.*
        CppTermDisciplinaForm.*
        CppTermConfirmDialog.*
        CppTermMessageDialog.*
        CppTermWindowUtils.*
        CppTermTypes.hpp
```

---

## 3. Integração com CMake

No `CMakeLists.txt` principal já deve existir (ou você adiciona):

```cmake
# Coleta fontes
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(historico ${SRC_FILES})

# Importa cpp-terminal
add_subdirectory(external/cpp-terminal)

# Linka com cpp-terminal
target_link_libraries(historico PRIVATE cpp-terminal::cpp-terminal)
```

### 3.1. Seleção da implementação de UI

A seleção da UI é feita via a opção `-DUI_IMPLEMENTATION=` no CMake.

Exemplo de mapeamento (no próprio `CMakeLists.txt`):

```cmake
if (NOT UI_IMPLEMENTATION)
    set(UI_IMPLEMENTATION "console")
endif()

message(STATUS "UI_IMPLEMENTATION = ${UI_IMPLEMENTATION}")

if (UI_IMPLEMENTATION STREQUAL "console")
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_CONSOLE)

elseif (UI_IMPLEMENTATION STREQUAL "terminal")
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_TERMINAL)

elseif (UI_IMPLEMENTATION STREQUAL "cpp-terminal")
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_CPP_TERMINAL)

# futuros:
elseif (UI_IMPLEMENTATION STREQUAL "ftxui")
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_FTXUI)
elseif (UI_IMPLEMENTATION STREQUAL "not-cursors")
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_NOT_CURSORS)
elseif (UI_IMPLEMENTATION STREQUAL "web")
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_WEB)

else()
    message(FATAL_ERROR "Valor invalido para UI_IMPLEMENTATION: ${UI_IMPLEMENTATION}")
endif()
```

No `main.cpp`, a escolha da UI é feita assim:

```cpp
#if defined(UI_IMPLEMENTATION_CPP_TERMINAL)
  #include "UICppTerminal.hpp"
  using UIType = UICppTerminal;
// ...demais opções...
#endif
```

---

## 4. Como compilar com a UI cpp-terminal

### 4.1. Windows (MSYS2 MinGW64)

No diretório raiz do projeto:

```bash
mkdir -p build
cd build

cmake -G "MinGW Makefiles" \
  -DUI_IMPLEMENTATION=cpp-terminal \
  -DCMAKE_BUILD_TYPE=Release \
  ..

cmake --build .
```

Se tudo estiver correto, será gerado o executável:

```text
build/historico.exe
```

---

## 5. Executando

No terminal (no mesmo ambiente em que compilou):

```bash
./historico
```

Opcionalmente com log verboso:

```bash
./historico -v
```

A UI `UICppTerminal` irá:

* inicializar o terminal em modo “raw”/TUI;
* exibir a tela principal de listagem do histórico;
* abrir diálogos de inserir/editar/remover usando as classes desta pasta;
* chamar apenas `IHistoricoService` para regras de negócio;
* registrar logs via `ILogger`/`FileLogger`.

---

## 6. Notas

* Se a UI cpp-terminal não inicializar por algum motivo (terminal sem suporte, execução fora de TTY, etc.), as exceções são registradas via `ILogger` e o programa pode cair em fallback textual (dependendo de como você tratar).
* Esta pasta não deve conter regras de negócio nem acesso direto a repositórios.
* Outras UIs (console simples, FTXUI, web, etc.) devem seguir o mesmo contrato e podem coexistir, mudando apenas a flag de compilação.

