#include "IDisciplinaRepository.hpp"
#include "IHistoricoService.hpp"
#include "IUserInterface.hpp"
#include "ILogger.hpp"
#include "Configuracao.hpp"

// Implementações concretas (exemplo de um build com terminal + csv)
#include "MemoryDisciplinaRepository.hpp"
#include "HistoricoService.hpp"
#include "FileLogger.hpp"
#include "ConsoleLogger.hpp"
#include "Errors.hpp"
#include <iostream>
#include <memory>

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
    #error "Nenhuma UI_IMPLEMENTATION_* definida. Defina, por exemplo: -DUI_IMPLEMENTATION=console|terminal|cpp-terminal|ftxui|notcurser|web"
#endif

#if defined(REPOSITORY_IMPLEMENTATION_MEMORY)
    #include "MemoryDisciplinaRepository.hpp"
    using RepType = MemoryDisciplinaRepository;

#elif defined(REPOSITORY_IMPLEMENTATION_BINARY)
    #include "BinaryDisciplinaRepository.hpp"
    using RepType = BinaryDisciplinaRepository;

#elif defined(REPOSITORY_IMPLEMENTATION_CSV)
    #include "CsvDisciplinaRepository.hpp"
    using RepType = CsvDisciplinaRepository;      // ajustar quando implementar

#elif defined(REPOSITORY_IMPLEMENTATION_FIXED)
    #include "FixedDisciplinaRepository.hpp"
    using RepType = FixedDisciplinaRepository; // ajustar quando implementar

#elif defined(REPOSITORY_IMPLEMENTATION_JSON)
    #include "JsonDisciplinaRepository.hpp"
    using RepType =JsonDisciplinaRepository;  // ajustar quando implementar

#elif defined(REPOSITORY_IMPLEMENTATION_SQLITE)
    #include "SqliteDiciplinaRepository.hpp"
    using RepType = SqliteDisciplinaRepository;            // ajustar quando implementar

#elif defined(REPOSITORY_IMPLEMENTATION_XML)
    #include "XmlDisciplinaRepository.hpp"
    using RepType = XmlDisciplinaRepository;            // ajustar quando implementar
#else
    #error "Nenhuma REPOSITORY_IMPLEMENTATION_* definida. Defina, por exemplo: -DREPOSITORY_IMPLEMENTATION=memory|bin|csv|fixed|json|sqlite|xml"
#endif

int main(int argc, char** argv)
{
    try {
        Configuracao config(argc, argv, "app.config");
        // 1. Configurar logger (ex: baseado em -v)
        std::unique_ptr<ILogger> ptr_log;
        if (config.getLogType()=="file")
            ptr_log = std::make_unique<FileLogger>(config);
        else
            ptr_log = std::make_unique<ConsoleLogger>(config);
        ILogger &log = *ptr_log;

        try {
            // 2. Escolher e criar o repositório ativo
            RepType repo(log, config);

            // 3. Criar o service com o repositório
            HistoricoService historicoService(repo, log);

            // 4. Criar a UI concreta com as dependências
            UIType ui(historicoService, log);

            // 5. Deixar a UI assumir o controle
            ui.run();
        }
        catch (const BusinessError& e) {
            LOG_ERR("erro infra nao tratado ", e.what())
        }
        catch (const InfraError& e) {
            LOG_ERR("erro infra nao tratado ", e.what())
        }
        catch (const std::exception& e) {
            LOG_ERR("erro inesperado nao tratado ", e.what())
        }
    } catch (const std::exception& e) {
        std::cerr << "erro nao tratado" << e.what();
    }
    catch (...) {
        std::cerr << "erro nao tratado";
    }
    
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

