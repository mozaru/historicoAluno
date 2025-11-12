#include "UIFtxui.hpp"

#include <utility>
#include <string>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include "ILogger.hpp"
#include "Errors.hpp"
#include "Conversion.hpp"
#include "Disciplina.hpp"
#include "IHistoricoService.hpp"

#include "FtxuiAppState.hpp"
#include "FtxuiHomeScreen.hpp"
#include "FtxuiDisciplinaForm.hpp"
#include "FtxuiConfirmDialog.hpp"
#include "FtxuiMessageDialog.hpp"

using ftxui::Component;
using ftxui::Element;
using ftxui::Event;

namespace ui {
namespace ftxuiui {

// -----------------------------------------------------------------------------
// Componente raiz que combina HomeScreen + modal opcional.
// -----------------------------------------------------------------------------
class RootComponent : public ftxui::ComponentBase {
public:
    RootComponent(Component home, Component* modal_ptr)
        : home_(std::move(home))
        , modal_ptr_(modal_ptr)
    {
        Add(home_);
    }

    Element OnRender() override {
        Element base = home_->OnRender();

        if (modal_ptr_ && *modal_ptr_) {
            Element overlay = (*modal_ptr_)->Render();
            // dbox: desenha base e por cima o overlay (modal).
            return ftxui::dbox({
                base,
                overlay,
            });
        }

        return base;
    }

    bool OnEvent(Event event) override {
        // Se há modal, ele tem prioridade exclusiva nos eventos.
        if (modal_ptr_ && *modal_ptr_) {
            if ((*modal_ptr_)->OnEvent(event)) {
                return true;
            }
            return false;
        }

        // Caso contrário, delega para a HomeScreen.
        if (home_->OnEvent(event)) {
            return true;
        }

        return false;
    }

private:
    Component home_;
    Component* modal_ptr_; // não-owning, gerenciado em UIFtxui::run
};

// -----------------------------------------------------------------------------
// UIFtxui
// -----------------------------------------------------------------------------

UIFtxui::UIFtxui(IHistoricoService& service, ILogger& logger, const Configuracao& conf)
    : historicoService(service)
    , log(logger)
{
}

void UIFtxui::run() {
    LOG_INF("UIFtxui iniciada")

    FtxuiAppState state;

    // Carrega lista inicial uma vez (pode falhar).
    try {
        auto list = historicoService.list();
        state.SetAll(list);
        LOG_INF("UIFtxui: lista inicial carregada qtd=", list.size())
    }
    catch (const InfraError& e) {
        LOG_ERR("UIFtxui: erro infra ao carregar lista inicial: ", e.what())
        state.SetError("Erro de infraestrutura ao carregar historico.");
    }
    catch (const std::exception& e) {
        LOG_ERR("UIFtxui: erro inesperado ao carregar lista inicial: ", e.what())
        state.SetError("Erro inesperado ao carregar historico.");
    }

    // Tela interativa FTXUI.
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    Component modal = nullptr; // modal atual (form, confirm, mensagem)

    auto close_modal = [&]() {
        modal = nullptr;
        //screen.RequestDraw();
    };

    // -------------------------------------------------------------------------
    // Callbacks da HomeScreen -> orquestram chamadas de serviço + estado.
    // -------------------------------------------------------------------------
    FtxuiHomeScreenCallbacks cb;

    // NOVA DISCIPLINA
    cb.on_new_disciplina = [&]() {
        LOG_INF("UIFtxui: novo solicitado")

        FtxuiDisciplinaFormOptions opts;
        opts.title   = "Nova disciplina";
        opts.editing = false;

        opts.on_submit = [&](const Disciplina& d_input) {
            try {
                Disciplina d = d_input;

                int novoId = historicoService.insert(d);
                d.setId(novoId);

                auto list = historicoService.list();
                state.SetAll(list);
                state.SetStatus("Disciplina inserida com sucesso (id=" + toString(novoId) + ").");

                LOG_INF("UIFtxui: insercao concluida id=", novoId)

                close_modal();
            }
            catch (const BusinessError& e) {
                LOG_ERR("UIFtxui: erro negocio insert: ", e.what())
                state.SetError(e.what());
                close_modal();
            }
            catch (const InfraError& e) {
                LOG_ERR("UIFtxui: erro infra insert: ", e.what())
                state.SetError("Erro de infraestrutura ao inserir disciplina.");
                close_modal();
            }
            catch (const std::exception& e) {
                LOG_ERR("UIFtxui: erro inesperado insert: ", e.what())
                state.SetError("Erro inesperado ao inserir disciplina.");
                close_modal();
            }
        };

        opts.on_cancel = [&]() {
            LOG_DBG("UIFtxui: novo cancelado pelo usuario")
            close_modal();
        };

        modal = MakeDisciplinaForm(opts);
        //screen.RequestDraw();
    };

    // EDITAR DISCIPLINA
    cb.on_edit_disciplina = [&](int id) {
        LOG_INF("UIFtxui: editar solicitado id=", id)

        // Busca dados atuais (não chama list de novo).
        Disciplina d;
        try {
            d = historicoService.get(id);
        }
        catch (const BusinessError& e) {
            LOG_ERR("UIFtxui: erro negocio get para editar: ", e.what())
            state.SetError(e.what());
            return;
        }
        catch (const InfraError& e) {
            LOG_ERR("UIFtxui: erro infra get para editar: ", e.what())
            state.SetError("Erro de infraestrutura ao consultar disciplina.");
            return;
        }
        catch (const std::exception& e) {
            LOG_ERR("UIFtxui: erro inesperado get para editar: ", e.what())
            state.SetError("Erro inesperado ao consultar disciplina.");
            return;
        }

        FtxuiDisciplinaFormOptions opts;
        opts.title   = "Editar disciplina (id=" + toString(id) + ")";
        opts.editing = true;
        opts.initial = d;

        opts.on_submit = [&, id](const Disciplina& d_editado) {
            try {
                historicoService.update(id, d_editado);

                Disciplina atualizado = d_editado;
                atualizado.setId(id);

                //state.AddOrUpdateDisciplina(atualizado);
                auto list = historicoService.list();
                state.SetAll(list);
                state.SetStatus("Disciplina atualizada com sucesso (id=" + toString(id) + ").");

                LOG_INF("UIFtxui: atualizacao concluida id=", id)

                close_modal();
            }
            catch (const BusinessError& e) {
                LOG_ERR("UIFtxui: erro negocio update: ", e.what())
                state.SetError(e.what());
                close_modal();
            }
            catch (const InfraError& e) {
                LOG_ERR("UIFtxui: erro infra update: ", e.what())
                state.SetError("Erro de infraestrutura ao atualizar disciplina.");
                close_modal();
            }
            catch (const std::exception& e) {
                LOG_ERR("UIFtxui: erro inesperado update: ", e.what())
                state.SetError("Erro inesperado ao atualizar disciplina.");
                close_modal();
            }
        };

        opts.on_cancel = [&]() {
            LOG_DBG("UIFtxui: edicao cancelada pelo usuario id=", id)
            close_modal();
        };

        modal = MakeDisciplinaForm(opts);
        //screen.RequestDraw();
    };

    // REMOVER DISCIPLINA
    cb.on_delete_disciplina = [&](int id) {
        LOG_INF("UIFtxui: remocao solicitada id=", id)

        Disciplina d;
        try {
            d = historicoService.get(id); // ou pegar do FtxuiAppState
        }             
        catch (const BusinessError& e) {
            LOG_ERR("UIFtxui: erro negocio remove: ", e.what())
            state.SetError(e.what());
            return;
        }
        catch (const InfraError& e) {
            LOG_ERR("UIFtxui: erro infra remove: ", e.what())
            state.SetError("Erro de infraestrutura ao remover disciplina.");
            return;
        }
        catch (const std::exception& e) {
            LOG_ERR("UIFtxui: erro inesperado remove: ", e.what())
            state.SetError("Erro inesperado ao remover disciplina.");
            return;
        }

        FtxuiConfirmDialogOptions opts;
        opts.title         = "Confirmar remocao";
        opts.message       = "Deseja realmente remover a disciplina selecionada (id=" + toString(id) + ")?";
        opts.confirm_label = "Sim";
        opts.cancel_label  = "Nao";
        opts.danger        = true;
        opts.disciplina    = d;

        opts.on_confirm = [&,id]() {
            try {
                historicoService.remove(id);
                auto list = historicoService.list();
                state.SetAll(list);
                //state.RemoveDisciplinaById(id);
                state.SetStatus("Disciplina removida com sucesso (id=" + toString(id) + ").");

                LOG_INF("UIFtxui: remocao concluida id=", id)
            }
            catch (const BusinessError& e) {
                LOG_ERR("UIFtxui: erro negocio remove: ", e.what())
                state.SetError(e.what());
            }
            catch (const InfraError& e) {
                LOG_ERR("UIFtxui: erro infra remove: ", e.what())
                state.SetError("Erro de infraestrutura ao remover disciplina.");
            }
            catch (const std::exception& e) {
                LOG_ERR("UIFtxui: erro inesperado remove: ", e.what())
                state.SetError("Erro inesperado ao remover disciplina.");
            }

            close_modal();
        };

        opts.on_cancel = [&, id]() {
            LOG_DBG("UIFtxui: remocao cancelada pelo usuario id=", id)
            state.SetStatus("Remocao cancelada id= (id=" + toString(id) + ").");
            close_modal();
        };

        modal = MakeConfirmDialog(opts);
        //screen.RequestDraw();
    };

    // MOSTRAR CR
    cb.on_show_cr = [&]() {
        LOG_INF("UIFtxui: calculo CR solicitado")

        try {
            double cr = historicoService.calculateCR();
            std::string msg = "CR do aluno: " + toString(cr);
            state.SetStatus(msg);

            FtxuiMessageDialogOptions m;
            m.title   = "Coeficiente de Rendimento";
            m.message = msg;
            m.is_error = false;
            m.on_close = [&]() { close_modal(); };

            modal = MakeMessageDialog(m);
            //screen.RequestDraw();

            LOG_INF("UIFtxui: CR calculado: ", cr)
        }
        catch (const InfraError& e) {
            LOG_ERR("UIFtxui: erro infra calcular CR: ", e.what())
            state.SetError("Erro de infraestrutura ao calcular CR.");

            FtxuiMessageDialogOptions m;
            m.title    = "Erro ao calcular CR";
            m.message  = "Erro de infraestrutura ao calcular CR.";
            m.is_error = true;
            m.on_close = [&]() { close_modal(); };

            modal = MakeMessageDialog(m);
            //screen.RequestDraw();
        }
        catch (const std::exception& e) {
            LOG_ERR("UIFtxui: erro inesperado calcular CR: ", e.what())
            state.SetError("Erro inesperado ao calcular CR.");

            FtxuiMessageDialogOptions m;
            m.title    = "Erro ao calcular CR";
            m.message  = "Erro inesperado ao calcular CR.";
            m.is_error = true;
            m.on_close = [&]() { close_modal(); };

            modal = MakeMessageDialog(m);
            //screen.RequestDraw();
        }
    };

    // SAIR
    cb.on_quit = [&]() {
        LOG_INF("UIFtxui: encerramento solicitado pelo usuario")
        state.RequestQuit();
        screen.Exit();
    };

    // -------------------------------------------------------------------------
    // Monta HomeScreen + Root e entra no loop da FTXUI.
    // -------------------------------------------------------------------------
    Component home = MakeHomeScreen(state, cb);
    Component root = ftxui::Make<RootComponent>(home, &modal);

    try {
        screen.Loop(root);
    }
    catch (const std::exception& e) {
        LOG_ERR("UIFtxui: erro inesperado no loop principal: ", e.what())
        // Aqui não temos UI segura para exibir nada; o log já registra.
    }

    LOG_INF("UIFtxui encerrada")
}

} // namespace ftxuiui
} // namespace ui

