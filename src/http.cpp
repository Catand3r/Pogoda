#include "http.h"
#include <curl/curl.h>
#include <sstream>
#include <stdexcept>
#include <utility>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::ostringstream *stream = static_cast<std::ostringstream *>(userp);
    stream->write(static_cast<char *>(contents), totalSize);
    return totalSize;
}

HttpClient::HttpClient()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpClient::~HttpClient()
{
    curl_global_cleanup();
}

HttpClient &HttpClient::getInstance()
{
    static HttpClient instance;
    return instance;
}

std::string HttpClient::get(const std::string &url)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("Failed to init CURL");

    std::ostringstream response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::string err = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL request failed: " + err);
    }

    curl_easy_cleanup(curl);
    return response.str();
}
