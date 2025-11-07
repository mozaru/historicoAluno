# Detecta plataforma (Windows vs Unix-like)
ifeq ($(OS),Windows_NT)
    EXE_EXT := .exe
else
    EXE_EXT :=
endif

# Compilador
CXX      := g++
CXXSTD   := -std=c++17

# Diretórios
INC_DIR  := headers
SRC_DIR  := src
OBJ_DIR  := obj

# Alvo final
TARGET   := historico$(EXE_EXT)

# ---------------------------------------------------------
# Funções utilitárias (somente GNU Make, mas portável entre SOs)
# ---------------------------------------------------------

# rwildcard(dir, padrão)
# Ex: $(call rwildcard,src/,*.cpp)
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# mkdir_p(path) - compatível com Windows (cmd) e Unix (sh)
define mkdir_p
  $(if $(filter Windows_NT,$(OS)),\
    if not exist "$(1)" mkdir "$(1)",\
    mkdir -p "$(1)")
endef

# rm_file(path)
define rm_file
  $(if $(filter Windows_NT,$(OS)),\
    if exist "$(1)" del /Q "$(1)",\
    rm -f "$(1)")
endef

# rm_dir(path)
define rm_dir
  $(if $(filter Windows_NT,$(OS)),\
    if exist "$(1)" rmdir /S /Q "$(1)",\
    rm -rf "$(1)")
endef

# ---------------------------------------------------------
# Includes: detecta todos os .h/.hpp recursivamente e gera -I únicos
# ---------------------------------------------------------

HDRS      := $(call rwildcard,$(INC_DIR)/,*.h) \
             $(call rwildcard,$(INC_DIR)/,*.hpp)

INC_FLAGS := $(addprefix -I,$(sort $(dir $(HDRS))))

CXXFLAGS  := -Wall -Wextra $(CXXSTD) $(INC_FLAGS)

# ---------------------------------------------------------
# Fontes / Objetos (recursivo em src/)
# ---------------------------------------------------------

# Todos os .cpp em src/ (recursivo)
SRC_FILES := $(call rwildcard,$(SRC_DIR)/,*.cpp)

# Adiciona principal.cpp na raiz do projeto
SRCS      := $(SRC_FILES) principal.cpp

# Converte:
#   src/algum/lugar/x.cpp  -> obj/algum/lugar/x.o
#   principal.cpp          -> obj/principal.o
OBJ_FROM_SRC := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_MAIN     := $(OBJ_DIR)/principal.o
OBJS         := $(OBJ_FROM_SRC) $(OBJ_MAIN)

# ---------------------------------------------------------
# Targets
# ---------------------------------------------------------

.PHONY: all clean list-src list-headers list-objs

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Regra genérica: qualquer .cpp dentro de src/ vira .o em obj/ espelhando estrutura
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(call mkdir_p,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

# principal.cpp na raiz
$(OBJ_DIR)/principal.o: principal.cpp
	@$(call mkdir_p,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza portável
clean:
	@$(call rm_dir,$(OBJ_DIR))
	@$(call rm_file,$(TARGET))

# Utilitários de debug

list-src:
	@echo SRC_FILES:
	@echo $(SRC_FILES)

list-headers:
	@echo INCLUDE DIRS:
	@echo $(sort $(dir $(HDRS)))

list-objs:
	@echo OBJS:
	@echo $(OBJS)
