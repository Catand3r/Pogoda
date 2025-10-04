#pragma once
#include "ihttp.h"

class HttpClient : public IHttp
{
  private:
    HttpClient();

    ~HttpClient();

    HttpClient(const HttpClient &) = delete;
    HttpClient &operator=(const HttpClient &) = delete;

  public:
    static HttpClient &getInstance();

    std::string get(const std::string &url) override;
};
