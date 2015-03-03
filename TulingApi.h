/*
 * TulingApi.h
 *
 *  Created on: Jan 28, 2015
 *      Author: qinyinghao
 */

#ifndef TULINGAPI_H_
#define TULINGAPI_H_

#include <string>
#include <curl/curl.h>
#include <utility>
#include <vector>

namespace qyh {

class TulingApi {
public:
    typedef struct TulingResponse {
        std::string code;
        std::string text;
        std::string url;
        std::vector<std::pair<std::string, std::string> > json;
    } TLRES;
    TulingApi();
    virtual ~TulingApi();
    bool getResponse(const std::string &info, TLRES &tulingResponse,
            const std::string &userid = "", const std::string &lon = "",
            const std::string &lat = "");
private:
    std::string generateUid();
    void parse(const std::string &data, TLRES &tulingResponse);
    void split(const std::string& s, const std::string& delim,
            std::vector<std::string>& ret);
    void deleteFirstAndLast(std::string &s, const char c);
    const std::string url = "http://www.tuling123.com/openapi/api?key=";
    const std::string key = "f0352a9021020adf5a7dbeefa9250d8e";
    CURL *curl;
    CURLcode res;
    static size_t writeToString(void *ptr, size_t size, size_t count,
            void *stream);
    const int UID_LEN;
    std::string defaultUid;
    std::string defaultLon;
    std::string defaultLat;
};

} /* namespace qyh */

#endif /* TULINGAPI_H_ */
