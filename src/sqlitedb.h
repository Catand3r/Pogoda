#pragma once
#include "isqlengine.h"
#include "sqlite3.h"
#include <memory>

class SQLiteDB : public ISQLEngine
{
  private:
    int lastrc_ = SQLITE_EMPTY;

    char *errmsg_ = nullptr;

    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> db_ = {nullptr, sqlite3_close};

    static int callback(void *queryResult, int colc, char **colv, char **coln)
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

  public:
    SQLiteDB(const std::string &dbsrc) : ISQLEngine(dbsrc)
    {
    }

    bool open() override
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

    bool exec(const std::string &query, QueryResult &queryRes) override
    {
        sqlite3_free(errmsg_);
        lastrc_ = sqlite3_exec(db_.get(), query.data(), callback, &queryRes, &errmsg_);
        return lastrc_ == SQLITE_OK;
    }

    int getLastRC() const override
    {
        return lastrc_;
    }

    bool isOpen() const override
    {
        return db_ != nullptr;
    }
};
