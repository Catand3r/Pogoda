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
    lastrc_ = sqlite3_open(dbsrc_.data(), &db);
    db_.reset(db);
    if (lastrc_ != SQLITE_OK)
    {
        db_.release();
        return false;
    }
    return true;
}

bool SQLiteDB::exec(const std::string &query, QueryResult &queryRes)
{
    sqlite3_free(errmsg_);
    lastrc_ = sqlite3_exec(db_.get(), query.data(), callback, &queryRes, &errmsg_);
    return lastrc_ == SQLITE_OK;
}

int SQLiteDB::getLastRC() const
{
    return lastrc_;
}

bool SQLiteDB::isOpen() const
{
    return db_ != nullptr;
}
