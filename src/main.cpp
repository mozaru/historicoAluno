#include "IDisciplinaRepository.hpp"
#include "IHistoricoService.hpp"
#include "IUserInterface.hpp"
#include "ILogger.hpp"

// Implementações concretas (exemplo de um build com terminal + csv)
#include "MemoryDisciplinaRepository.hpp"
#include "HistoricoService.hpp"
#include "FileLogger.hpp"
#include "ConsoleLogger.hpp"

#if defined(UI_IMPLEMENTATION_CONSOLE)
    #include "UIConsole.hpp"
    using UIType = UIConsole;

#elif defined(UI_IMPLEMENTATION_TERMINAL)
    #include "UITerminal.hpp"
    using UIType = UITerminal;

#elif defined(UI_IMPLEMENTATION_CPP_TERMINAL)
    #include "UICppTerminal.hpp"
    using UIType = UICppTerminal;

#elif defined(UI_IMPLEMENTATION_FTXUI)
    #include "UIFtxui.hpp"
    using UIType = ui::ftxuiui::UIFtxui;          // ajustar quando implementar

#elif defined(UI_IMPLEMENTATION_NOT_CURSORS)
    #include "UINotCursors.hpp"
    using UIType = UINotCursors;     // ajustar quando implementar

#elif defined(UI_IMPLEMENTATION_WEB)
    #include "UIWeb.hpp"
    using UIType = UIWeb;            // ajustar quando implementar
#else
    #error "Nenhuma UI_IMPLEMENTATION_* definida. Defina, por exemplo: -DUI_IMPLEMENTATION_CPP_TERMINAL"
#endif

int main(int argc, char** argv)
{
    // 1. Configurar logger (ex: baseado em -v)
    bool verbose = (argc > 1 && std::string(argv[1]) == "-v");
    FileLogger logger(verbose);
    //ConsoleLogger logger(verbose);

    // 2. Escolher e criar o repositório ativo
    MemoryDisciplinaRepository repo(logger);

    // 3. Criar o service com o repositório
    HistoricoService historicoService(repo, logger);

    // 4. Criar a UI concreta com as dependências
    UIType ui(historicoService, logger);

    // 5. Deixar a UI assumir o controle
    ui.run();

    return 0;
}


/*
int main() {
    auto& registry = Registry::instance();
    auto& logger = registry.getLogger();
    auto& historicoService = registry.getHistoricoService();
    auto ui = registry.createUI();  // std::unique_ptr<IUI>
    ui->run();
}
*/
