#ifndef _CPP_TERM_HOME_SCREEN_HPP_
#define _CPP_TERM_HOME_SCREEN_HPP_

#include <vector>
#include <string>

#include "Disciplina.hpp"
#include "ILogger.hpp"
#include "CppTermTypes.hpp"
#include "CppTermWindowUtils.hpp"

// cpp-terminal
#include "cpp-terminal/window.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/key.hpp"

class CppTermHomeScreen
{
public:
    explicit CppTermHomeScreen(ILogger& logger);

    // Desenha a tela principal, processa input e retorna a ação do usuário.
    // - disciplinas: lista atual (já eventualmente filtrada)
    // - cr: coeficiente atual calculado pelo service
    // - currentFilter: string de filtro a exibir no campo
    CppTermHomeResult show(const std::vector<Disciplina>& disciplinas,
                           double cr,
                           const std::string& currentFilter);

private:
    enum class Focus {
        FilterInput,
        FilterButton,
        InsertButton,
        Table
    };

    ILogger&    log;
    std::size_t scrollOffset;
    std::size_t selectedIndex; // índice na lista (não é o id)
    std::string filterText;
    Focus       focus;

    void render(Term::Window& win,
                const std::vector<Disciplina>& disciplinas,
                double cr,
                std::size_t rows,
                std::size_t cols);

    void renderHeader(Term::Window& win,
                      std::size_t cols) const;

    void renderFilterBar(Term::Window& win,
                         std::size_t cols) const;

    void renderTable(Term::Window& win,
                     const std::vector<Disciplina>& disciplinas,
                     double cr,
                     std::size_t rows,
                     std::size_t cols);

    void renderFooter(Term::Window& win,
                      std::size_t rows,
                      std::size_t cols) const;

    void adjustSelection(std::size_t totalRows, std::size_t visibleRows);

    // Processa tecla; preenche outResult e retorna true se deve sair da tela.
    bool handleKey(const Term::Key& key,
                   const std::vector<Disciplina>& disciplinas,
                   std::size_t visibleRows,
                   CppTermHomeResult& outResult);
};

#endif // _CPP_TERM_HOME_SCREEN_HPP_
