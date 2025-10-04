#pragma once
#include "isqlengine.h"
#include <memory>
#include <sqlite3.h>

class SQLiteDB : public ISQLEngine
{
  private:
    int lastrc_ = SQLITE_EMPTY;

    char *errmsg_ = nullptr;

    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> db_ = {nullptr, sqlite3_close};

    static int callback(void *queryResult, int colc, char **colv, char **coln);

  public:
    SQLiteDB(const std::string &dbsrc) : ISQLEngine(dbsrc)
    {
    }

    bool open() override;

    bool exec(const std::string &query, QueryResult &queryRes) override;

    int getLastRC() const override;

    bool isOpen() const override;
};
