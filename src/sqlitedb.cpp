#include "sqlitedb.h"

int SQLiteDB::callback(void *queryResult, int colc, char **colv, char **coln)
{
    Row row;

    for (int i = 0; i < colc; i++)
    {
        const std::string colName = coln[i];
        const std::string colValue = colv[i];
        row.push_back(std::make_pair(colName, colValue));
    }

    QueryResult *qr = reinterpret_cast<QueryResult *>(queryResult);

    qr->push_back(row);

    return 0;
}

bool SQLiteDB::open()
{
    sqlite3 *db = nullptr;
    auto &lastrc = execResults_[std::this_thread::get_id()].rc;
    lastrc = sqlite3_open(dbsrc_.data(), &db);
    db_.reset(db);
    if (lastrc != SQLITE_OK)
    {
        db_.release();
        return false;
    }
    return true;
}

bool SQLiteDB::exec(const std::string &query, QueryResult &queryRes)
{
    std::unique_lock<std::mutex> lock(mtx_);

    auto &execResult = execResults_[std::this_thread::get_id()];

    sqlite3_free(execResult.errmsg);
    execResult.rc = sqlite3_exec(db_.get(), query.data(), callback, &queryRes, &execResult.errmsg);
    return execResult.rc == SQLITE_OK;
}

int SQLiteDB::getLastRC() const
{
    /*std::unique_lock<std::mutex> lock(mtx_);*/

    const auto &execResult = execResults_.at(std::this_thread::get_id());
    return execResult.rc;
}

bool SQLiteDB::isOpen() const
{
    return db_ != nullptr;
}
