#pragma once
#include "dataParser.h"
#include "iniwrapper.h"
#include "sqlitedb.h"

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

    std::unique_ptr<IDataParser> parser_;

    Cities cities_;

    uint64_t readPeriod_ = 0; // miliseconds

    void readIniFile();

    bool openDB();

    bool createDatabase();
};
