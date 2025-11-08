#include "IDisciplinaRepository.hpp"
#include "IHistoricoService.hpp"
#include "IUserInterface.hpp"
#include "ILogger.hpp"

// Implementações concretas (exemplo de um build com terminal + csv)
#include "MemoryDisciplinaRepository.hpp"
#include "HistoricoService.hpp"
#include "FileLogger.hpp"
#include "UITerminal.hpp"

int main(int argc, char** argv)
{
    // 1. Configurar logger (ex: baseado em -v)
    bool verbose = (argc > 1 && std::string(argv[1]) == "-v");
    FileLogger logger(verbose);

    // 2. Escolher e criar o repositório ativo
    MemoryDisciplinaRepository repo(logger);

    // 3. Criar o service com o repositório
    HistoricoService historicoService(repo, logger);

    // 4. Criar a UI concreta com as dependências
    UITerminal ui(historicoService, logger);

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
