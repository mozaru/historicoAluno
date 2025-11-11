#include "SQLiteDisciplinaRepository.hpp"

#include <stdexcept>
#include <sstream>

#include "Errors.hpp"

using namespace std;

// --------------------------------------------------------
// Helpers internos
// --------------------------------------------------------

static void checkSqlite(int rc, sqlite3* db, const char* msg)
{
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        std::ostringstream oss;
        oss << msg << " (sqlite rc=" << rc << ", msg=" << (db ? sqlite3_errmsg(db) : "null") << ")";
        throw InfraError(oss.str());
    }
}

// --------------------------------------------------------
// Construtor / Destrutor
// --------------------------------------------------------

SQLiteDisciplinaRepository::SQLiteDisciplinaRepository(ILogger& aLog,
                                                       const Configuracao& conf)
    : log(aLog)
    , filename(conf.getConnectionString())
    , db(nullptr)
{
    LOG_INF("SQLiteDisciplinaRepository abrindo banco: ", filename);
    openDatabase();
    ensureSchema();
}

SQLiteDisciplinaRepository::~SQLiteDisciplinaRepository()
{
    closeDatabase();
}

// --------------------------------------------------------
// Controle de conexao
// --------------------------------------------------------

void SQLiteDisciplinaRepository::openDatabase()
{
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        std::string err = db ? sqlite3_errmsg(db) : "erro desconhecido";
        if (db)
            sqlite3_close(db);
        db = nullptr;
        throw InfraError("Falha ao abrir banco SQLite: " + err);
    }

    // Modo mais previsível em caso de acesso concorrente (mesmo que aqui não tenha):
    sqlite3_busy_timeout(db, 2000);
}

void SQLiteDisciplinaRepository::closeDatabase()
{
    if (db)
    {
        int rc = sqlite3_close(db);
        if (rc != SQLITE_OK)
        {
            // Aqui não vamos jogar exceção em destrutor, só logar se tiver mecanismo.
            // Se quiser: LOG_ERR("Falha ao fechar SQLite: ", sqlite3_errmsg(db));
        }
        db = nullptr;
    }
}

// --------------------------------------------------------
// Schema
// --------------------------------------------------------

void SQLiteDisciplinaRepository::ensureSchema() const
{
    const char* sql =
        "CREATE TABLE IF NOT EXISTS disciplinas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  matricula TEXT NOT NULL,"
        "  nome TEXT NOT NULL,"
        "  semestre INTEGER NOT NULL,"
        "  ano INTEGER NOT NULL,"
        "  creditos INTEGER NOT NULL,"
        "  nota1 REAL NOT NULL,"
        "  nota2 REAL NOT NULL"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::string msg = errMsg ? errMsg : "erro desconhecido";
        sqlite3_free(errMsg);
        throw InfraError("Falha ao criar tabela disciplinas: " + msg);
    }
}

// --------------------------------------------------------
// Mapear linha -> Disciplina
// stmt: SELECT id, matricula, nome, semestre, ano, creditos, nota1, nota2
// --------------------------------------------------------

Disciplina SQLiteDisciplinaRepository::mapRowToDisciplina(sqlite3_stmt* stmt)
{
    Disciplina d;

    int idx = 0;
    int id         = sqlite3_column_int(stmt, idx++);
    const unsigned char* mat  = sqlite3_column_text(stmt, idx++);
    const unsigned char* nome = sqlite3_column_text(stmt, idx++);
    int semestre    = sqlite3_column_int(stmt, idx++);
    int ano         = sqlite3_column_int(stmt, idx++);
    int creditos    = sqlite3_column_int(stmt, idx++);
    double nota1    = sqlite3_column_double(stmt, idx++);
    double nota2    = sqlite3_column_double(stmt, idx++);

    d.setId(id);
    d.setMatricula(mat  ? reinterpret_cast<const char*>(mat)  : "");
    d.setNome     (nome ? reinterpret_cast<const char*>(nome) : "");
    d.setSemestre(semestre);
    d.setAno(ano);
    d.setCreditos(creditos);
    d.setNota1(nota1);
    d.setNota2(nota2);

    return d;
}

// --------------------------------------------------------
// get(int id)
// --------------------------------------------------------

Disciplina SQLiteDisciplinaRepository::get(int id) const
{
    LOG_DBG("sqlite.get id=", id);

    const char* sql =
        "SELECT id, matricula, nome, semestre, ano, creditos, nota1, nota2 "
        "FROM disciplinas WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar SELECT em get()");

    rc = sqlite3_bind_int(stmt, 1, id);
    checkSqlite(rc, db, "Falha ao bind id em get()");

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        Disciplina d = mapRowToDisciplina(stmt);
        sqlite3_finalize(stmt);
        LOG_DBG("sqlite.get ok id=", id, " nome=", d.getNome());
        return d;
    }

    sqlite3_finalize(stmt);
    throw InfraError("Disciplina nao encontrada (id=" + std::to_string(id) + ")");
}

// --------------------------------------------------------
// insert
// --------------------------------------------------------

int SQLiteDisciplinaRepository::insert(const Disciplina& disciplina)
{
    LOG_DBG("sqlite.insert nome=", disciplina.getNome());

    const char* sql =
        "INSERT INTO disciplinas "
        "(matricula, nome, semestre, ano, creditos, nota1, nota2) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar INSERT");

    int idx = 1;
    rc = sqlite3_bind_text (stmt, idx++, disciplina.getMatricula().c_str(), -1, SQLITE_TRANSIENT);
    checkSqlite(rc, db, "Falha ao bind matricula em INSERT");

    rc = sqlite3_bind_text (stmt, idx++, disciplina.getNome().c_str(), -1, SQLITE_TRANSIENT);
    checkSqlite(rc, db, "Falha ao bind nome em INSERT");

    rc = sqlite3_bind_int  (stmt, idx++, disciplina.getSemestre());
    checkSqlite(rc, db, "Falha ao bind semestre em INSERT");

    rc = sqlite3_bind_int  (stmt, idx++, disciplina.getAno());
    checkSqlite(rc, db, "Falha ao bind ano em INSERT");

    rc = sqlite3_bind_int  (stmt, idx++, disciplina.getCreditos());
    checkSqlite(rc, db, "Falha ao bind creditos em INSERT");

    rc = sqlite3_bind_double(stmt, idx++, disciplina.getNota1());
    checkSqlite(rc, db, "Falha ao bind nota1 em INSERT");

    rc = sqlite3_bind_double(stmt, idx++, disciplina.getNota2());
    checkSqlite(rc, db, "Falha ao bind nota2 em INSERT");

    rc = sqlite3_step(stmt);
    checkSqlite(rc, db, "Falha ao executar INSERT");

    sqlite3_finalize(stmt);

    int newId = static_cast<int>(sqlite3_last_insert_rowid(db));
    LOG_DBG("sqlite.insert ok id=", newId);
    return newId;
}

// --------------------------------------------------------
// update
// --------------------------------------------------------

void SQLiteDisciplinaRepository::update(int id, const Disciplina& disciplina)
{
    LOG_DBG("sqlite.update id=", id, " novo_nome=", disciplina.getNome());

    const char* sql =
        "UPDATE disciplinas SET "
        "matricula = ?, "
        "nome = ?, "
        "semestre = ?, "
        "ano = ?, "
        "creditos = ?, "
        "nota1 = ?, "
        "nota2 = ? "
        "WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar UPDATE");

    int idx = 1;
    rc = sqlite3_bind_text (stmt, idx++, disciplina.getMatricula().c_str(), -1, SQLITE_TRANSIENT);
    checkSqlite(rc, db, "bind matricula UPDATE");

    rc = sqlite3_bind_text (stmt, idx++, disciplina.getNome().c_str(), -1, SQLITE_TRANSIENT);
    checkSqlite(rc, db, "bind nome UPDATE");

    rc = sqlite3_bind_int  (stmt, idx++, disciplina.getSemestre());
    checkSqlite(rc, db, "bind semestre UPDATE");

    rc = sqlite3_bind_int  (stmt, idx++, disciplina.getAno());
    checkSqlite(rc, db, "bind ano UPDATE");

    rc = sqlite3_bind_int  (stmt, idx++, disciplina.getCreditos());
    checkSqlite(rc, db, "bind creditos UPDATE");

    rc = sqlite3_bind_double(stmt, idx++, disciplina.getNota1());
    checkSqlite(rc, db, "bind nota1 UPDATE");

    rc = sqlite3_bind_double(stmt, idx++, disciplina.getNota2());
    checkSqlite(rc, db, "bind nota2 UPDATE");

    rc = sqlite3_bind_int(stmt, idx++, id);
    checkSqlite(rc, db, "bind id WHERE UPDATE");

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        checkSqlite(rc, db, "Falha ao executar UPDATE");
    }

    int changes = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (changes == 0)
        throw InfraError("Disciplina nao encontrada para atualizacao (id=" + std::to_string(id) + ")");

    LOG_DBG("sqlite.update ok id=", id);
}

// --------------------------------------------------------
// remove
// --------------------------------------------------------

void SQLiteDisciplinaRepository::remove(int id)
{
    LOG_DBG("sqlite.remove id=", id);

    const char* sql = "DELETE FROM disciplinas WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar DELETE");

    rc = sqlite3_bind_int(stmt, 1, id);
    checkSqlite(rc, db, "bind id DELETE");

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        checkSqlite(rc, db, "Falha ao executar DELETE");
    }

    int changes = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (changes == 0)
        throw InfraError("Disciplina nao encontrada para remocao (id=" + std::to_string(id) + ")");

    LOG_DBG("sqlite.remove ok id=", id);
}

// --------------------------------------------------------
// list
// --------------------------------------------------------

std::vector<Disciplina> SQLiteDisciplinaRepository::list() const
{
    LOG_DBG("sqlite.list");

    std::vector<Disciplina> out;

    const char* sql =
        "SELECT id, matricula, nome, semestre, ano, creditos, nota1, nota2 "
        "FROM disciplinas "
        "ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar SELECT em list()");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        out.push_back(mapRowToDisciplina(stmt));
    }

    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        checkSqlite(rc, db, "Erro em iteracao de list()");
    }

    sqlite3_finalize(stmt);

    LOG_DBG("sqlite.list retornou=", out.size());
    return out;
}

// --------------------------------------------------------
// exist(matricula, ano, semestre)
// --------------------------------------------------------

bool SQLiteDisciplinaRepository::exist(const std::string& matricula,
                                       int ano,
                                       int semestre) const
{
    LOG_DBG("sqlite.exist m=", matricula,
            " ano=", ano, " sem=", semestre);

    const char* sql =
        "SELECT 1 FROM disciplinas "
        "WHERE matricula = ? AND ano = ? AND semestre = ? "
        "LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar SELECT em exist(m,a,s)");

    int idx = 1;
    rc = sqlite3_bind_text(stmt, idx++, matricula.c_str(), -1, SQLITE_TRANSIENT);
    checkSqlite(rc, db, "bind matricula exist");

    rc = sqlite3_bind_int(stmt, idx++, ano);
    checkSqlite(rc, db, "bind ano exist");

    rc = sqlite3_bind_int(stmt, idx++, semestre);
    checkSqlite(rc, db, "bind semestre exist");

    rc = sqlite3_step(stmt);

    bool found = (rc == SQLITE_ROW);

    if (rc != SQLITE_ROW && rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        checkSqlite(rc, db, "Erro em exist(m,a,s)");
    }

    sqlite3_finalize(stmt);

    LOG_DBG("sqlite.exist(m,a,s) = ", found ? "true" : "false");
    return found;
}

bool SQLiteDisciplinaRepository::exist(int id) const
{
    LOG_DBG("sqlite.exist(id) id=", id);

    const char* sql =
        "SELECT 1 FROM disciplinas WHERE id = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    checkSqlite(rc, db, "Falha ao preparar SELECT em exist(id)");

    rc = sqlite3_bind_int(stmt, 1, id);
    checkSqlite(rc, db, "bind id exist(id)");

    rc = sqlite3_step(stmt);

    bool found = (rc == SQLITE_ROW);

    if (rc != SQLITE_ROW && rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        checkSqlite(rc, db, "Erro em exist(id)");
    }

    sqlite3_finalize(stmt);

    LOG_DBG("sqlite.exist(id) = ", found ? "true" : "false");
    return found;
}
