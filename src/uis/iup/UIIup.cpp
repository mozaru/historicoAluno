#include "UIIup.hpp"

#include "Errors.hpp"
#include "IupMessageDialog.hpp"

UIIup::UIIup(IHistoricoService& service, ILogger& logger, const Configuracao& conf)
    : historicoService(service),
      log(logger)
{
    // Nada aqui além de guardar referências.
    // Inicialização da IUP é feita em run().
}

void UIIup::run()
{
    LOG_INF("UI IUP iniciada");

    bool iupOpened = false;

    try {
        // Inicializa a biblioteca IUP.
        int argc = 0;
        char** argv = nullptr;
        int ret = IupOpen(&argc, &argv);
        if (ret == IUP_ERROR) {
            LOG_ERR("Falha ao inicializar IUP (IupOpen retornou IUP_ERROR)");
            // Não temos UI pra mostrar mensagem; só logamos e encerramos.
            return;
        }
        iupOpened = true;
        
        IupSetGlobal("UTF8MODE", "YES");
        IupSetGlobal("UTF8MODE_FILE", "YES");

        // (Opcional) inicializar controles adicionais, como matrix,
        // se a sua build de IUP exigir:
        IupControlsOpen();

        {
            // Cria a janela principal (Home) e exibe.
            IupHomeWindow home(historicoService, log);

            Ihandle* dlg = home.getDialog();
            if (!dlg) {
                LOG_ERR("Dialog principal (Home) nulo");
                if (iupOpened) {
                    IupClose();
                }
                return;
            }

            IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

            // Loop principal de eventos da IUP.
            IupMainLoop();
        } // home destruído aqui, dialog destruído no destrutor

        LOG_INF("Loop principal da UI IUP encerrado");
    }
    catch (const BusinessError& e) {
        LOG_ERR("BusinessError nao tratado na UI IUP: ", e.what());
        // Se ainda tivermos IUP ativo, mostramos um dialog de erro.
        if (iupOpened) {
            IupMessageDialog::showError(e.what());
        }
    }
    catch (const InfraError& e) {
        LOG_ERR("InfraError nao tratado na UI IUP: ", e.what());
        if (iupOpened) {
            IupMessageDialog::showError("Erro de infraestrutura na interface. Consulte o log.");
        }
    }
    catch (const std::exception& e) {
        LOG_ERR("Excecao inesperada na UI IUP: ", e.what());
        if (iupOpened) {
            IupMessageDialog::showError("Erro inesperado na interface. Consulte o log.");
        }
    }
    catch (...) {
        LOG_ERR("Excecao desconhecida na UI IUP.");
        if (iupOpened) {
            IupMessageDialog::showError("Erro desconhecido na interface. Consulte o log.");
        }
    }

    if (iupOpened) {
        IupClose();
        LOG_INF("IUP finalizada com sucesso");
    }

    LOG_INF("UI IUP encerrada");
}
