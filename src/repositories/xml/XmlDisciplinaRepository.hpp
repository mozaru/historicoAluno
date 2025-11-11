#ifndef XML_DISCIPLINA_REPOSITORY_HPP
#define XML_DISCIPLINA_REPOSITORY_HPP

#include <string>
#include <vector>

#include "ILogger.hpp"
#include "IDisciplinaRepository.hpp"
#include "Disciplina.hpp"
#include "Configuracao.hpp"

// pugixml (MIT) - header-only + cpp
#include "pugixml.hpp"

class XmlDisciplinaRepository : public IDisciplinaRepository
{
public:
    XmlDisciplinaRepository(ILogger& aLog, const Configuracao& conf);
    ~XmlDisciplinaRepository() override;

    Disciplina get(int id) const override;
    int insert(const Disciplina& disciplina) override;
    void update(int id, const Disciplina& disciplina) override;
    void remove(int id) override;
    std::vector<Disciplina> list() const override;
    bool exist(const std::string& matricula, int ano, int semestre) const override;

    bool exist(int id) const; // se estiver no contrato base, implementamos aqui

private:
    using XmlDoc  = pugi::xml_document;
    using XmlNode = pugi::xml_node;

    ILogger&    log;
    std::string filename;

    // Carrega ou cria documento com raiz <disciplinas>.
    void loadDocument(XmlDoc& doc) const;
    void saveDocument(const XmlDoc& doc) const;

    static XmlNode getRoot(XmlDoc& doc);

    static XmlNode      getDisciplinaNodeById(XmlNode root, int id);
    static int          getRecordCount(XmlNode root);

    static void         fillNodeFromDisciplina(XmlNode node, const Disciplina& d);
    static Disciplina   makeDisciplinaFromNode(XmlNode node, int id);
};

#endif // XML_DISCIPLINA_REPOSITORY_HPP
