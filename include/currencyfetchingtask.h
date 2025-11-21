#pragma once
#include "itask.h"
#include "isqlengine.h"
#include "currencydataparser.h"

#include <chrono>
#include <string>

class IIniWrapper;
class IDataParser;

class CurrencyFetchingTask : public ITask
{
  public:
    using Currencies = std::vector<std::string>;
    using Range = std::pair<std::chrono::sys_days, std::chrono::sys_days>;

    CurrencyFetchingTask(IIniWrapper &ini, ISQLEngine &db, IDataParser &parser, uint64_t interval)
        : ini_(ini), db_(db), parser_(parser), ITask(std::chrono::milliseconds(interval))
    {
    }

    void Run() override;

    void Init() override;

  private:
    std::vector<Range> setRanges(const std::vector<std::chrono::sys_days> &result,
                                 const std::chrono::sys_days &currentTime, const std::chrono::sys_days &oldestTime);
    void RunForCurrency(const std::string& currency);
    ISQLEngine::QueryResult GetExistingDatesFromDB(const std::string& currency, const std::chrono::year_month_day& ymdRange);
    CurrencyData FetchingDataForRange(const std::string& currency, const Range& range);
    bool SaveDataInDB(const std::vector<CurrencyData>& currencyDataList);

    Range range_;
    Currencies currencies_;
    std::string rateminhistory_;

    IIniWrapper &ini_;
    ISQLEngine &db_;
    IDataParser &parser_;

    void ReadIniFile();
};
