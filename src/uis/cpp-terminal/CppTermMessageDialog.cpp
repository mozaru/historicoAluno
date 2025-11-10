#include "CppTermMessageDialog.hpp"

#include <vector>
#include <iostream>

// cpp-terminal
#include "cpp-terminal/window.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/exception.hpp"

CppTermMessageDialog::CppTermMessageDialog(ILogger& logger)
    : log(logger)
{
}

CppTermMessageResult CppTermMessageDialog::show(const std::string& titulo,
                                                const std::string& mensagem)
{
    LOG_INF("MessageDialog: titulo='", titulo, "' msg='", mensagem, "'")

    try
    {
        Term::Screen screen = Term::screen_size();
        Term::Window win(screen);

        // Quebra simples em linhas se houver quebras explícitas.
        std::vector<std::string> linhas;
        {
            std::string atual;
            for (char c : mensagem)
            {
                if (c == '\n')
                {
                    linhas.push_back(atual);
                    atual.clear();
                }
                else
                {
                    atual.push_back(c);
                }
            }
            if (!atual.empty())
                linhas.push_back(atual);
            if (linhas.empty())
                linhas.push_back(std::string());
        }

        // Desenha o dialog com um único botão [OK], focado.
        CppTermUI::drawDialog(win, titulo, linhas, {"OK"}, 0);

        // Renderiza na tela.
        Term::cout << win.render(1, 1, false) << std::flush;

        // Loop de leitura até o usuário confirmar.
        while (true)
        {
            Term::Event ev = Term::read_event();

            switch (ev.type())
            {
                case Term::Event::Type::Key:
                {
                    Term::Key key(ev);
                    switch (key)
                    {
                        // Enter confirma o botão focado
                        case Term::Key::Enter:
                        //case Term::Key::Return:
                        case Term::Key::Space:
                        {
                            LOG_DBG("MessageDialog: fechado pelo usuario (Enter, Return, Space)")
                            return CppTermMessageResult::Ok;                        
                        }
                        // Esc cancela
                        case Term::Key::Esc:
                        {
                            LOG_INF("MessageDialog: fechado pelo usuario (Esc)")
                            return CppTermMessageResult::Ok;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case Term::Event::Type::Screen:
                {
                    // Redimensionou o terminal → redesenha centralizado
                    screen = Term::Screen(ev);
                    Term::Window w2(screen);
                    CppTermUI::drawDialog(win, titulo, linhas, {"OK"}, 0);
                    Term::cout << w2.render(1, 1, false) << std::flush;
                    break;
                }
                default:
                    // Mouse/outros eventos ignorados por enquanto.
                    break;
                // Ignora mouse/outros eventos: dialog continua até "OK".
            }            
        }
    }
    catch (const Term::Exception& e)
    {
        LOG_ERR("MessageDialog: erro cpp-terminal: ", e.what())
        // Em caso de falha na UI, faz o mínimo possível.
        std::cerr << "[" << titulo << "] " << mensagem << std::endl;
        return CppTermMessageResult::Ok;
    }
    catch (const std::exception& e)
    {
        LOG_ERR("MessageDialog: erro inesperado: ", e.what())
        std::cerr << "[" << titulo << "] " << mensagem << std::endl;
        return CppTermMessageResult::Ok;
    }
}
