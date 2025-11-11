#include "FtxuiDisciplinaForm.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>

#include "Conversion.hpp"
#include "Errors.hpp"

namespace ui {
namespace ftxuiui {

using namespace ftxui;

namespace {

class DisciplinaForm : public ComponentBase {
public:
    explicit DisciplinaForm(const FtxuiDisciplinaFormOptions& options)
        : options_(options)
    {
        if (options_.editing) {
            id_        = options_.initial.getId();
            matricula_ = options_.initial.getMatricula();
            nome_      = options_.initial.getNome();
            creditos_  = toString(options_.initial.getCreditos());
            ano_       = toString(options_.initial.getAno());
            semestre_  = toString(options_.initial.getSemestre());
            nota1_     = toString(options_.initial.getNota1());
            nota2_     = toString(options_.initial.getNota2());
        }

        input_matricula_ = Input(&matricula_, "Matricula");
        input_nome_      = Input(&nome_, "Nome da disciplina");
        input_creditos_  = Input(&creditos_, "Creditos (int)");
        input_ano_       = Input(&ano_, "Ano (int)");
        input_semestre_  = Input(&semestre_, "Semestre (int)");
        input_nota1_     = Input(&nota1_, "Nota 1 (double)");
        input_nota2_     = Input(&nota2_, "Nota 2 (double)");

        button_salvar_ = Button("Salvar", [this] { OnSubmit(); });
        button_cancel_ = Button("Cancelar", [this] { OnCancel(); });

        Add(Container::Vertical({
            input_matricula_,
            input_nome_,
            input_creditos_,
            input_ano_,
            input_semestre_,
            input_nota1_,
            input_nota2_,
            Container::Horizontal({
                button_salvar_,
                button_cancel_,
            }),
        }));
    }

    Element OnRender() override {
        const std::string title =
            !options_.title.empty()
                ? options_.title
                : (options_.editing ? "Editar disciplina" : "Nova disciplina");

        std::vector<Element> rows;

        rows.push_back(text(title) | bold | center);
        rows.push_back(separator());

        rows.push_back(labeled("Matricula", input_matricula_->Render()));
        rows.push_back(labeled("Nome",      input_nome_->Render()));
        rows.push_back(labeled("Creditos",  input_creditos_->Render()));
        rows.push_back(labeled("Ano",       input_ano_->Render()));
        rows.push_back(labeled("Semestre",  input_semestre_->Render()));
        rows.push_back(labeled("Nota 1",    input_nota1_->Render()));
        rows.push_back(labeled("Nota 2",    input_nota2_->Render()));

        rows.push_back(separator());
        rows.push_back(
            hbox({
                button_salvar_->Render(),
                text(" "),
                button_cancel_->Render(),
            }) | center
        );

        if (!error_message_.empty()) {
            rows.push_back(separator());
            rows.push_back(text(error_message_) | color(Color::RedLight));
        }

        Element form =
            vbox(std::move(rows))
            | border
            | size(WIDTH, GREATER_THAN, 60)     // largura mínima
            | size(HEIGHT, GREATER_THAN, 15);   // altura mínima

        // clear_under garante que nada da tela base apareça "por baixo"
        // dentro da área ocupada pelo form.
        return form | clear_under | center;
    }



    bool OnEvent(Event event) override {
        if (event == Event::Escape) {
            OnCancel();
            return true;
        }
        return ComponentBase::OnEvent(event);
    }

private:
    FtxuiDisciplinaFormOptions options_;
    int id_ = -1;

    std::string matricula_;
    std::string nome_;
    std::string creditos_;
    std::string ano_;
    std::string semestre_;
    std::string nota1_;
    std::string nota2_;

    std::string error_message_;

    Component input_matricula_;
    Component input_nome_;
    Component input_creditos_;
    Component input_ano_;
    Component input_semestre_;
    Component input_nota1_;
    Component input_nota2_;

    Component button_salvar_;
    Component button_cancel_;

    static Element labeled(const std::string& label, Element field) {
        return hbox({
            text(label + ": ") | size(WIDTH, EQUAL, 14),
            field | flex,
        });
    }

    void OnSubmit() {
        error_message_.clear();
        try {
            int creditos = toInt(creditos_);
            int ano      = toInt(ano_);
            int semestre = toInt(semestre_);
            double n1    = toDouble(nota1_);
            double n2    = toDouble(nota2_);

            Disciplina d;
            if (options_.editing && id_ > 0)
                d.setId(id_);

            d.setMatricula(matricula_);
            d.setNome(nome_);
            d.setCreditos(creditos);
            d.setAno(ano);
            d.setSemestre(semestre);
            d.setNota1(n1);
            d.setNota2(n2);

            if (options_.on_submit)
                options_.on_submit(d);
        }
        catch (const ConversionError& e) {
            error_message_ = e.what();
        }
        catch (...) {
            error_message_ = "Erro ao validar dados da disciplina.";
        }
    }

    void OnCancel() {
        error_message_.clear();
        if (options_.on_cancel)
            options_.on_cancel();
    }
};

} // namespace

ftxui::Component MakeDisciplinaForm(const FtxuiDisciplinaFormOptions& options) {
    return ftxui::Make<DisciplinaForm>(options);
}

} // namespace ftxuiui
} // namespace ui
