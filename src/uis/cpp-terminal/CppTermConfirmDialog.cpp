#include "CppTermConfirmDialog.hpp"

#include <iostream>

// cpp-terminal
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/window.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/exception.hpp"

#include "CppTermWindowUtils.hpp"

CppTermConfirmDialog::CppTermConfirmDialog(ILogger& logger)
    : log(logger)
{
}

CppTermConfirmResult
CppTermConfirmDialog::show(const std::string& titulo,
                           const std::vector<std::string>& linhas)
{
    LOG_INF("ConfirmDialog: titulo='", titulo, "'")

    try
    {
        // Tamanho atual do terminal / tela
        Term::Screen screen = Term::screen_size();
        Term::Window win(screen);

        // Começa focado em [Nao] para evitar remoção acidental
        int botaoFocado = 1; // 0 = Sim, 1 = Nao

        CppTermUI::drawDialog(win, titulo, linhas, {"Sim", "Nao"}, botaoFocado);
        Term::cout << win.render(1, 1, false) << std::flush;

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
                // Alterna foco entre [Sim] e [Nao]
                case Term::Key::ArrowLeft:
                case Term::Key::ArrowRight:
                case Term::Key::Tab:
                //case Term::Key::Shift_Tab:
                {
                    botaoFocado = (botaoFocado == 0) ? 1 : 0;

                    Term::Window w2(screen);
                    CppTermUI::drawDialog(w2, titulo, linhas,
                                          {"Sim", "Nao"}, botaoFocado);
                    Term::cout << w2.render(1, 1, false) << std::flush;

                    LOG_DBG("ConfirmDialog: foco botao=",
                            (botaoFocado == 0 ? "Sim" : "Nao"))
                    break;
                }

                // Enter confirma o botão focado
                case Term::Key::Enter:
                //case Term::Key::Return:
                {
                    if (botaoFocado == 0)
                    {
                        LOG_INF("ConfirmDialog: resposta=Yes (Enter)")
                        return CppTermConfirmResult::Yes;
                    }
                    else
                    {
                        LOG_INF("ConfirmDialog: resposta=No (Enter)")
                        return CppTermConfirmResult::No;
                    }
                }

                // Esc cancela
                case Term::Key::Esc:
                {
                    LOG_INF("ConfirmDialog: resposta=Cancel (Esc)")
                    return CppTermConfirmResult::Cancel;
                }

                // Atalhos de confirmação rápida
                case Term::Key::y:
                case Term::Key::Y:
                case Term::Key::s:
                case Term::Key::S:
                    LOG_INF("ConfirmDialog: resposta=Yes (atalho)")
                    return CppTermConfirmResult::Yes;

                case Term::Key::n:
                case Term::Key::N:
                    LOG_INF("ConfirmDialog: resposta=No (atalho)")
                    return CppTermConfirmResult::No;

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
                CppTermUI::drawDialog(w2, titulo, linhas,
                                      {"Sim", "Nao"}, botaoFocado);
                Term::cout << w2.render(1, 1, false) << std::flush;
                break;
            }

            default:
                // Mouse/outros eventos ignorados por enquanto.
                break;
            }
        }
    }
    catch (const Term::Exception& e)
    {
        LOG_ERR("ConfirmDialog: erro cpp-terminal: ", e.what())

        // Fallback texto puro (não mata a app)
        std::cerr << "[" << titulo << "]" << std::endl;
        for (const auto& l : linhas)
            std::cerr << l << std::endl;
        std::cerr << "(y = Sim, n = Nao, outro = Cancela)" << std::endl;

        char c;
        if (std::cin >> c)
        {
            if (c == 'y' || c == 'Y' || c == 's' || c == 'S')
                return CppTermConfirmResult::Yes;
            if (c == 'n' || c == 'N')
                return CppTermConfirmResult::No;
        }
        return CppTermConfirmResult::Cancel;
    }
    catch (const std::exception& e)
    {
        LOG_ERR("ConfirmDialog: erro inesperado: ", e.what())

        std::cerr << "[" << titulo << "]" << std::endl;
        for (const auto& l : linhas)
            std::cerr << l << std::endl;

        return CppTermConfirmResult::Cancel;
    }
}
