#include "XmlDisciplinaRepository.hpp"

#include <fstream>
#include <stdexcept>

#include "Errors.hpp"

using namespace std;

using XmlDoc  = pugi::xml_document;
using XmlNode = pugi::xml_node;

// --------------------------------------------------------
// Construtor / destrutor
// --------------------------------------------------------

XmlDisciplinaRepository::XmlDisciplinaRepository(ILogger& aLog,
                                                 const Configuracao& conf)
    : log(aLog)
    , filename(conf.getFileName("xml"))
{
    LOG_INF("XmlDisciplinaRepository arquivo=", filename);
}

XmlDisciplinaRepository::~XmlDisciplinaRepository() = default;

// --------------------------------------------------------
// Helpers: load/save + root
// --------------------------------------------------------

void XmlDisciplinaRepository::loadDocument(XmlDoc& doc) const
{
    // Se não existir, criamos raiz vazia.
    std::ifstream in(filename);
    if (!in.good())
    {
        doc.reset();
        doc.append_child("disciplinas");
        return;
    }

    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    if (!result)
    {
        throw ConversionError(
            std::string("Falha ao ler XML de disciplinas: ") + result.description()
        );
    }

    XmlNode root = doc.child("disciplinas");
    if (!root)
    {
        throw ConversionError("XML de disciplinas invalido: raiz <disciplinas> ausente.");
    }
}

void XmlDisciplinaRepository::saveDocument(const XmlDoc& doc) const
{
    if (!doc.child("disciplinas"))
        throw InfraError("XML interno invalido: raiz <disciplinas> ausente ao salvar.");

    if (!doc.save_file(filename.c_str(), "  "))
        throw InfraError("Falha ao gravar XML de disciplinas no arquivo.");
}

XmlNode XmlDisciplinaRepository::getRoot(XmlDoc& doc)
{
    XmlNode root = doc.child("disciplinas");
    if (!root)
        root = doc.append_child("disciplinas");
    return root;
}

// --------------------------------------------------------
// Helpers de acesso por id (1-based, posicional)
// --------------------------------------------------------

int XmlDisciplinaRepository::getRecordCount(XmlNode root)
{
    int count = 0;
    for (XmlNode n : root.children("disciplina"))
        ++count;
    return count;
}

XmlNode XmlDisciplinaRepository::getDisciplinaNodeById(XmlNode root, int id)
{
    if (id <= 0)
        return XmlNode();

    int idx = 0;
    for (XmlNode n : root.children("disciplina"))
    {
        ++idx;
        if (idx == id)
            return n;
    }
    return XmlNode();
}

// --------------------------------------------------------
// Map Disciplina <-> XML node
// --------------------------------------------------------

void XmlDisciplinaRepository::fillNodeFromDisciplina(XmlNode node,
                                                     const Disciplina& d)
{
    // Limpa conteúdo anterior
    node.remove_children();

    auto set_child = [&](const char* name, const std::string& value) {
        XmlNode c = node.append_child(name);
        c.append_child(pugi::node_pcdata).set_value(value.c_str());
    };

    auto set_child_int = [&](const char* name, int value) {
        XmlNode c = node.append_child(name);
        c.text().set(value);
    };

    auto set_child_double = [&](const char* name, double value) {
        XmlNode c = node.append_child(name);
        c.text().set(value);
    };

    set_child("matricula", d.getMatricula());
    set_child("nome",      d.getNome());
    set_child_int("semestre",  d.getSemestre());
    set_child_int("ano",       d.getAno());
    set_child_int("creditos",  d.getCreditos());
    set_child_double("nota1",  d.getNota1());
    set_child_double("nota2",  d.getNota2());
}

Disciplina XmlDisciplinaRepository::makeDisciplinaFromNode(XmlNode node, int id)
{
    if (!node)
        throw ConversionError("Nodo <disciplina> invalido no XML.");

    auto get_str = [&](const char* name) -> std::string {
        XmlNode c = node.child(name);
        if (!c || !c.child_value())
            return "";
        return std::string(c.child_value());
    };

    auto get_int = [&](const char* name) -> int {
        XmlNode c = node.child(name);
        if (!c)
            throw ConversionError(std::string("Campo inteiro ausente no XML: ") + name);
        return c.text().as_int();
    };

    auto get_double = [&](const char* name) -> double {
        XmlNode c = node.child(name);
        if (!c)
            throw ConversionError(std::string("Campo double ausente no XML: ") + name);
        return c.text().as_double();
    };

    Disciplina d;
    d.setId(id);
    d.setMatricula(get_str("matricula"));
    d.setNome(get_str("nome"));
    d.setSemestre(get_int("semestre"));
    d.setAno(get_int("ano"));
    d.setCreditos(get_int("creditos"));
    d.setNota1(get_double("nota1"));
    d.setNota2(get_double("nota2"));

    return d;
}

// --------------------------------------------------------
// Métodos do repositório
// --------------------------------------------------------

Disciplina XmlDisciplinaRepository::get(int id) const
{
    LOG_DBG("xml.get id=", id);

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    XmlNode node = getDisciplinaNodeById(root, id);
    if (!node)
        throw InfraError("Disciplina nao encontrada (id=" + std::to_string(id) + ")");

    Disciplina d = makeDisciplinaFromNode(node, id);

    LOG_DBG("xml.get ok id=", id, " nome=", d.getNome());
    return d;
}

int XmlDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("xml.insert nome=", disciplina.getNome());

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    XmlNode node = root.append_child("disciplina");
    fillNodeFromDisciplina(node, disciplina);

    int newId = getRecordCount(root); // posicao do ultimo

    saveDocument(doc);

    LOG_DBG("xml.insert ok id=", newId);
    return newId;
}

void XmlDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("xml.update id=", id, " novo_nome=", disciplina.getNome());

    if (id <= 0)
        throw InfraError("Id invalido para update (id=" + std::to_string(id) + ")");

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    XmlNode node = getDisciplinaNodeById(root, id);
    if (!node)
        throw InfraError("Disciplina nao encontrada para update (id=" + std::to_string(id) + ")");

    fillNodeFromDisciplina(node, disciplina);
    saveDocument(doc);

    LOG_DBG("xml.update ok id=", id);
}

void XmlDisciplinaRepository::remove(int id)
{
    LOG_DBG("xml.remove id=", id);

    if (id <= 0)
        throw InfraError("Id invalido para remocao (id=" + std::to_string(id) + ")");

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    int total = getRecordCount(root);
    if (total == 0 || id > total)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");

    // Se só tem um, apaga direto
    if (total == 1)
    {
        XmlNode only = root.child("disciplina");
        if (only)
            root.remove_child(only);
        saveDocument(doc);
        LOG_DBG("xml.remove ok id=", id, " total_novo=0");
        return;
    }

    // Node a apagar
    XmlNode target = getDisciplinaNodeById(root, id);
    if (!target)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");

    // Node do ultimo
    XmlNode last = getDisciplinaNodeById(root, total);
    if (!last)
        throw InfraError("Falha ao localizar ultimo registro em remocao XML.");

    if (target != last)
    {
        // Copia conteúdo do último para o slot do meio
        XmlDoc tempDoc;
        XmlNode tempWrapper = tempDoc.append_child("x");
        XmlNode tempCopy = tempWrapper.append_copy(last);

        target.remove_children();
        for (XmlNode c : tempCopy.children())
        {
            target.append_copy(c);
        }
    }

    // Remove o ultimo
    root.remove_child(last);

    saveDocument(doc);

    LOG_DBG("xml.remove ok id=", id, " total_antigo=", total, " total_novo=", total - 1);
}

std::vector<Disciplina> XmlDisciplinaRepository::list() const
{
    LOG_DBG("xml.list");

    std::vector<Disciplina> out;

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    int id = 0;
    for (XmlNode node : root.children("disciplina"))
    {
        ++id;
        out.push_back(makeDisciplinaFromNode(node, id));
    }

    LOG_DBG("xml.list retornou=", out.size());
    return out;
}

bool XmlDisciplinaRepository::exist(const std::string& matricula,
                                    int ano,
                                    int semestre) const
{
    LOG_DBG("xml.exist m=", matricula,
            " ano=", ano, " sem=", semestre);

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    int id = 0;
    for (XmlNode node : root.children("disciplina"))
    {
        ++id;
        Disciplina d = makeDisciplinaFromNode(node, id);
        if (d.getMatricula() == matricula &&
            d.getAno()       == ano &&
            d.getSemestre()  == semestre)
        {
            LOG_DBG("xml.exist true id=", id);
            return true;
        }
    }

    LOG_DBG("xml.exist false");
    return false;
}

bool XmlDisciplinaRepository::exist(int id) const
{
    LOG_DBG("xml.exist(id) id=", id);

    XmlDoc doc;
    loadDocument(doc);
    XmlNode root = getRoot(doc);

    int total = getRecordCount(root);
    bool ok = (id > 0 && id <= total);

    LOG_DBG(ok ? "true" : "false");
    return ok;
}
