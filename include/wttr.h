#pragma once
#include "currencydataparser.h"
#include "iniwrapper.h"
#include "sqlitedb.h"
#include "weatherdataparser.h"

class Wttr
{
  public:
    using Cities = std::vector<std::string>;

  public:
    Wttr(const std::string &iniFileSrc);

    ~Wttr();

    void run();

  private:
    std::unique_ptr<ISQLEngine> db_;

    std::unique_ptr<IIniWrapper> ini_;

    std::ofstream outFile_;

    std::unique_ptr<IDataParser> weatherparser_;

    std::unique_ptr<IDataParser> currencyparser_;

    Cities cities_;

    bool openDB();

    bool createDatabase();
};
