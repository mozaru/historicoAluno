#include "FtxuiAppState.hpp"

#include "Conversion.hpp"
#include "Errors.hpp"

#include <algorithm>
#include <cctype>

namespace ui {
namespace ftxuiui {

namespace {

std::string toLower(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        out.push_back(static_cast<char>(std::tolower(c)));
    }
    return out;
}

bool containsCaseInsensitive(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) {
        return true;
    }
    const std::string t = toLower(text);
    const std::string p = toLower(pattern);
    return t.find(p) != std::string::npos;
}

} // namespace

void FtxuiAppState::SetAll(const std::vector<Disciplina>& list) {
    all_disciplines_ = list;
    ApplyFilter();
}

const std::vector<Disciplina>& FtxuiAppState::All() const noexcept {
    return all_disciplines_;
}

const std::vector<Disciplina>& FtxuiAppState::Filtered() const noexcept {
    return filtered_disciplines_;
}

void FtxuiAppState::ApplyFilter() noexcept {
    // Mantém robustez: qualquer erro de conversão vira mensagem amigável
    // e não explode exceção pra fora.
    error_message_.clear();

    int previous_selected_id = GetSelectedId();

    int ano_value = 0;
    bool use_ano = false;

    int semestre_value = 0;
    bool use_semestre = false;

    double min_media_value = 0.0;
    bool use_min_media = false;

    double max_media_value = 0.0;
    bool use_max_media = false;

    try {
        if (!filter_ano.empty()) {
            ano_value = toInt(filter_ano);
            use_ano = true;
        }
    } catch (const ConversionError&) {
        error_message_ = "Filtro de ano invalido.";
    }

    try {
        if (!filter_semestre.empty()) {
            semestre_value = toInt(filter_semestre);
            use_semestre = true;
        }
    } catch (const ConversionError&) {
        if (error_message_.empty()) {
            error_message_ = "Filtro de semestre invalido.";
        }
    }

    try {
        if (!filter_min_media.empty()) {
            min_media_value = toDouble(filter_min_media);
            use_min_media = true;
        }
    } catch (const ConversionError&) {
        if (error_message_.empty()) {
            error_message_ = "Filtro de media minima invalido.";
        }
    }

    try {
        if (!filter_max_media.empty()) {
            max_media_value = toDouble(filter_max_media);
            use_max_media = true;
        }
    } catch (const ConversionError&) {
        if (error_message_.empty()) {
            error_message_ = "Filtro de media maxima invalido.";
        }
    }

    filtered_disciplines_.clear();
    filtered_disciplines_.reserve(all_disciplines_.size());

    for (const auto& d : all_disciplines_) {
        bool ok = true;

        if (!filter_matricula.empty()) {
            if (!containsCaseInsensitive(d.getMatricula(), filter_matricula)) {
                ok = false;
            }
        }

        if (ok && !filter_nome.empty()) {
            if (!containsCaseInsensitive(d.getNome(), filter_nome)) {
                ok = false;
            }
        }

        if (ok && use_ano) {
            if (d.getAno() != ano_value) {
                ok = false;
            }
        }

        if (ok && use_semestre) {
            if (d.getSemestre() != semestre_value) {
                ok = false;
            }
        }

        if (ok && use_min_media) {
            if (d.getMedia() < min_media_value) {
                ok = false;
            }
        }

        if (ok && use_max_media) {
            if (d.getMedia() > max_media_value) {
                ok = false;
            }
        }

        if (ok) {
            filtered_disciplines_.push_back(d);
        }
    }

    // Se houve erro de conversão em algum filtro, a mensagem já foi registrada.
    // Mesmo assim, mantemos o resultado do filtro que conseguiu ser aplicado.

    RebuildSelectionAfterFilter(previous_selected_id);
}

void FtxuiAppState::SelectIndex(int index) noexcept {
    if (filtered_disciplines_.empty()) {
        selected_index_ = -1;
        return;
    }

    if (index < 0) {
        selected_index_ = 0;
    } else if (index >= static_cast<int>(filtered_disciplines_.size())) {
        selected_index_ = static_cast<int>(filtered_disciplines_.size()) - 1;
    } else {
        selected_index_ = index;
    }
}

int FtxuiAppState::SelectedIndex() const noexcept {
    return selected_index_;
}

const Disciplina* FtxuiAppState::GetSelected() const noexcept {
    if (selected_index_ < 0 ||
        selected_index_ >= static_cast<int>(filtered_disciplines_.size())) {
        return nullptr;
    }
    return &filtered_disciplines_[static_cast<std::size_t>(selected_index_)];
}

Disciplina* FtxuiAppState::GetSelected() noexcept {
    if (selected_index_ < 0 ||
        selected_index_ >= static_cast<int>(filtered_disciplines_.size())) {
        return nullptr;
    }
    return &filtered_disciplines_[static_cast<std::size_t>(selected_index_)];
}

int FtxuiAppState::GetSelectedId() const noexcept {
    const Disciplina* d = GetSelected();
    return d ? d->getId() : -1;
}

void FtxuiAppState::ClearMessages() noexcept {
    status_message_.clear();
    error_message_.clear();
}

void FtxuiAppState::SetStatus(const std::string& msg) noexcept {
    status_message_ = msg;
    // Mantém erro anterior apenas se ainda relevante; aqui, por simplicidade,
    // limpamos o erro ao definir um novo status positivo.
    error_message_.clear();
}

void FtxuiAppState::SetError(const std::string& msg) noexcept {
    error_message_ = msg;
}

const std::string& FtxuiAppState::StatusMessage() const noexcept {
    return status_message_;
}

const std::string& FtxuiAppState::ErrorMessage() const noexcept {
    return error_message_;
}

void FtxuiAppState::RequestQuit() noexcept {
    request_quit_ = true;
}

bool FtxuiAppState::ShouldQuit() const noexcept {
    return request_quit_;
}

void FtxuiAppState::AddOrUpdateDisciplina(const Disciplina& d) noexcept {
    const int id = d.getId();
    bool updated = false;

    for (auto& it : all_disciplines_) {
        if (it.getId() == id) {
            it = d;
            updated = true;
            break;
        }
    }

    if (!updated) {
        all_disciplines_.push_back(d);
    }

    ApplyFilter();
}

void FtxuiAppState::RemoveDisciplinaById(int id) noexcept {
    auto it = std::remove_if(
        all_disciplines_.begin(),
        all_disciplines_.end(),
        [id](const Disciplina& d) { return d.getId() == id; }
    );

    if (it != all_disciplines_.end()) {
        all_disciplines_.erase(it, all_disciplines_.end());
    }

    ApplyFilter();
}

void FtxuiAppState::RebuildSelectionAfterFilter(int previous_selected_id) noexcept {
    selected_index_ = -1;

    if (filtered_disciplines_.empty()) {
        return;
    }

    if (previous_selected_id > 0) {
        for (std::size_t i = 0; i < filtered_disciplines_.size(); ++i) {
            if (filtered_disciplines_[i].getId() == previous_selected_id) {
                selected_index_ = static_cast<int>(i);
                return;
            }
        }
    }

    // Se o item anterior não existe mais, seleciona o primeiro.
    selected_index_ = 0;
}

} // namespace ftxuiui
} // namespace ui
