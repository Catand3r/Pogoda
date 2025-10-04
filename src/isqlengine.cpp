#pragma once
#include <string>
#include <vector>

class ISQLEngine
{
  public:
    ISQLEngine(const std::string &dbsrc) : dbsrc_(dbsrc)
    {
    }

    using Row = std::vector<std::pair<std::string, std::string>>;

    using QueryResult = std::vector<Row>;

    virtual bool open() = 0;
    virtual bool exec(const std::string &, QueryResult &) = 0;
    virtual int getLastRC() const = 0;
    virtual bool isOpen() const = 0;

  protected:
    const std::string dbsrc_;
};
