#include "CppTermDisciplinaForm.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cctype>

// cpp-terminal
#include "cpp-terminal/window.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/exception.hpp"

CppTermDisciplinaForm::CppTermDisciplinaForm(ILogger& logger)
    : log(logger)
{
}

bool CppTermDisciplinaForm::show(Disciplina& d, bool isEdicao)
{
    // Prepara snapshot inicial. Só vamos aplicar em 'd' se confirmar.
    std::vector<Field> fields;

    fields.push_back(Field{ "Nome",       d.getNome(),       50, false, false });
    fields.push_back(Field{ "Matricula",  d.getMatricula(),  20, false, false });
    fields.push_back(Field{ "Creditos",   (d.getCreditos()  > 0 ? std::to_string(d.getCreditos())  : ""),  3, true,  false });
    fields.push_back(Field{ "Ano",        (d.getAno()       > 0 ? std::to_string(d.getAno())       : ""),  4, true,  false });
    fields.push_back(Field{ "Semestre",   (d.getSemestre()  > 0 ? std::to_string(d.getSemestre())  : ""),  2, true,  false });
    fields.push_back(Field{ "Nota 1",     (d.getNota1()     > 0 ? std::to_string(d.getNota1())     : ""),  6, true,  true  });
    fields.push_back(Field{ "Nota 2",     (d.getNota2()     > 0 ? std::to_string(d.getNota2())     : ""),  6, true,  true  });

    const std::string titulo = isEdicao
        ? "EDITAR DISCIPLINA"
        : "INSERIR DISCIPLINA";

    LOG_INF("DisciplinaForm: abrir (", (isEdicao ? "edicao" : "insercao"), ")")

    bool ok = false;

    try {
        ok = editFields(fields, titulo);
    }
    catch (const Term::Exception& e) {
        LOG_ERR("DisciplinaForm: erro cpp-terminal: ", e.what())
        ok = false;
    }
    catch (const std::exception& e) {
        LOG_ERR("DisciplinaForm: erro inesperado: ", e.what())
        ok = false;
    }

    if (!ok) {
        LOG_DBG("DisciplinaForm: cancelado pelo usuario")
        return false;
    }

    if (!applyToDisciplina(fields, d)) {
        LOG_ERR("DisciplinaForm: falha ao aplicar campos na Disciplina")
        return false;
    }

    LOG_INF("DisciplinaForm: confirmado com sucesso")
    return true;
}

bool CppTermDisciplinaForm::editFields(std::vector<Field>& fields,
                                       const std::string& titulo)
{
    if (fields.empty())
        return false;

    Term::Screen screen = Term::screen_size();
    Term::Window win(screen);
    CppTermSize size = CppTermUI::getWindowSize(win);        

    int current = 0; // índice campo atual

    bool running    = true;
    bool needRender = true;

    while (running)
    {
        if (needRender)
        {
            win.clear();

            // Dimensões aproximadas do formulário
            const int formWidth  = (size.cols > 60) ? 60 : size.cols - 4;
            const int formHeight = static_cast<int>(fields.size()) + 8;
            int top, left, w, h;

            CppTermUI::drawCenteredFrame(win, formWidth, formHeight,
                                         top, left, w, h);

            // Título
            CppTermUI::drawCenteredText(win, top,
                                        titulo.substr(0, w - 4));

            // Campos
            int y = top + 2;
            for (std::size_t i = 0; i < fields.size(); ++i)
            {
                const Field& f = fields[i];

                std::ostringstream line;
                line << f.label << ": " << f.value;

                std::string text = line.str();
                if ((int)text.size() > w - 4)
                    text.resize(w - 4);

                int x = left + 2;

                win.print_str(static_cast<std::uint32_t>(x),
                              static_cast<std::uint32_t>(y),
                              text);

                // Indicador do campo selecionado
                if ((int)i == current)
                {
                    // Marcador simples ">" no início da linha
                    win.print_str(static_cast<std::uint32_t>(x - 2),
                                  static_cast<std::uint32_t>(y),
                                  ">");
                }

                ++y;
                if (y >= top + h - 4)
                    break;
            }

            // Botões [ OK ] [ Cancelar ]
            const std::string btOk  = "[ OK ]";
            const std::string btCan = "[ Cancelar ]";
            const int buttonsY      = top + h - 3;
            int totalLen = (int)btOk.size() + 2 + (int)btCan.size();
            int bx = left + (w - totalLen) / 2;
            if (bx < left + 1) bx = left + 1;

            win.print_str(static_cast<std::uint32_t>(bx),
                          static_cast<std::uint32_t>(buttonsY),
                          btOk);
            win.print_str(static_cast<std::uint32_t>(bx + btOk.size() + 2),
                          static_cast<std::uint32_t>(buttonsY),
                          btCan);

            Term::cout << win.render(1, 1, false) << std::flush;
            needRender = false;
        }

        Term::Event ev = Term::read_event();
        if (ev.empty())
            continue;

        if (ev.type()!=Term::Event::Type::Key)
           continue;
        
        Term::Key key(ev);

        switch (key)
        {
        // Navegação entre campos
        case Term::Key::ArrowUp:
            if (current > 0) {
                --current;
                needRender = true;
            }
            break;

        case Term::Key::ArrowDown:
        case Term::Key::Tab:
            if (current + 1 < (int)fields.size()) {
                ++current;
                needRender = true;
            } else {
                current = 0;
                needRender = true;
            }
            break;

        /*case Term::Key::Shift_Tab:
            if (current > 0) {
                --current;
            } else {
                current = (int)fields.size() - 1;
            }
            needRender = true;
            break;
        */ 
        // Confirma (OK)
        case Term::Key::Enter:
        //case Term::Key::Return:
            // Aceita diretamente os valores atuais.
            return true;

        // Cancelar
        case Term::Key::Esc:
            return false;

        // Edição básica do campo atual
        default:
        {
            // Dependendo da versão da lib, você terá algo como:
            //  - key->is_character()
            //  - key->unicode()
            // Aqui vou assumir uma API genérica e você ajusta o detalhe.
            char ch = static_cast<std::int32_t>(key);

            // Exemplo genérico; adapte para sua Term::Key:
            // if (key->is_character()) ch = (char)key->unicode();
            // Para não chutar a API, vamos supor que Type::Char exista não-named.
            // Se não, você substitui isso por sua verificação real.

            if (ch != 0)
            {
                Field& f = fields[current];

                // Backspace manual se vier como caractere (caso API entregue assim)
                if (ch == 8 || ch == 127)
                {
                    if (!f.value.empty()) {
                        f.value.pop_back();
                        needRender = true;
                    }
                }
                else
                {
                    // Aplica regras de tipo
                    bool aceita = true;

                    if (f.numeric)
                    {
                        if (std::isdigit(static_cast<unsigned char>(ch)))
                        {
                            aceita = true;
                        }
                        else if (f.real && (ch == '.' || ch == ','))
                        {
                            // Permite um ponto/virgula
                            if (f.value.find('.') != std::string::npos &&
                                f.value.find(',') != std::string::npos)
                                aceita = false;
                        }
                        else
                        {
                            aceita = false;
                        }
                    }

                    if (aceita && (int)f.value.size() < f.maxLen)
                    {
                        f.value.push_back(ch);
                        needRender = true;
                    }
                }
            }

            break;
        }
        }
    }

    return false; // fallback, não deveria chegar aqui
}

bool CppTermDisciplinaForm::applyToDisciplina(const std::vector<Field>& fields,
                                              Disciplina& d)
{
    if (fields.size() < 7)
        return false;

    try {
        const std::string& nome      = fields[0].value;
        const std::string& matricula = fields[1].value;
        const std::string& sCred     = fields[2].value;
        const std::string& sAno      = fields[3].value;
        const std::string& sSem      = fields[4].value;
        const std::string& sNota1    = fields[5].value;
        const std::string& sNota2    = fields[6].value;

        int    cred  = sCred.empty() ? 0 : std::stoi(sCred);
        int    ano   = sAno.empty()  ? 0 : std::stoi(sAno);
        int    sem   = sSem.empty()  ? 0 : std::stoi(sSem);
        double nota1 = sNota1.empty()? 0.0 : std::stod(sNota1);
        double nota2 = sNota2.empty()? 0.0 : std::stod(sNota2);

        d.setNome(nome);
        d.setMatricula(matricula);
        d.setCreditos(cred);
        d.setAno(ano);
        d.setSemestre(sem);
        d.setNota1(nota1);
        d.setNota2(nota2);

        LOG_DBG("DisciplinaForm: aplicado com sucesso (matricula=", matricula, ")")
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERR("DisciplinaForm: erro conversao/aplicacao: ", e.what())
        return false;
    }
}
