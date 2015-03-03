/*
 * TulingApi.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: qinyinghao
 */

#include <iostream>

#include "TulingApi.h"

namespace qyh {

TulingApi::TulingApi() :
        curl(NULL), UID_LEN(31), defaultLon("120.627641"), defaultLat(
                "31.260771") {
}

TulingApi::~TulingApi() {
    if (curl != NULL) {
        curl_easy_cleanup(curl);
        curl = NULL;
    }
}

//attention return null str if in null str
void TulingApi::split(const std::string& s, const std::string& delim,
        std::vector<std::string>& ret) {
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != std::string::npos) {
        ret.push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0) {
        ret.push_back(s.substr(last, index - last));
    }
}

std::string TulingApi::generateUid() {
    int flag;
    std::string s;
    int i = 0;
    char ch[UID_LEN + 1];
    srand((unsigned) time(NULL));
    for (i = 0; i < UID_LEN; ++i) {
        flag = rand() % 2;
        if (flag)
            ch[i] = 'A' + rand() % 26;
        else
            ch[i] = 'a' + rand() % 26;
    }
    ch[i] = '\0';
    printf("ch = %s\n",ch);
    printf("ch = %s\n",ch);
    printf("ch = %s\n",ch);
    s = std::string(ch);
    return s;
}

void TulingApi::parse(const std::string &data, TLRES &tulingResponse) {
    std::vector<std::string> v1;
    std::string splitStr1 = ",";
    std::string splitStr2 = ":";
    std::string temp = data;
    deleteFirstAndLast(temp, '{');
    deleteFirstAndLast(temp, '}');
    split(temp, splitStr1, v1);
    bool isList = false;
    std::string line;
    std::string listKey;
    std::string listValue;
    for (std::vector<std::string>::iterator pos = v1.begin(); pos != v1.end();
            ++pos) {
        if (isList) {
            if (pos->find("}]") != std::string::npos) {
                isList = false;
                line = pos->substr(0, pos->find_last_of("}]"));
                std::vector<std::string> t;
                split(line, splitStr2, t);
                if (t.size() == 2) {
                    listKey = t[0];
                    listValue = t[1];
                    deleteFirstAndLast(listKey, '\"');
                    deleteFirstAndLast(listValue, '\"');
                    tulingResponse.json.push_back(
                            std::make_pair(listKey, listValue));
                }
            } else {
                line = *pos;
                std::vector<std::string> t;
                split(line, splitStr2, t);
                if (t.size() == 2) {
                    listKey = t[0];
                    listValue = t[1];
                    deleteFirstAndLast(listKey, '\"');
                    deleteFirstAndLast(listValue, '\"');
                    tulingResponse.json.push_back(
                            std::make_pair(listKey, listValue));
                }
            }
        } else {
            if (pos->find("[{") != std::string::npos) {
                isList = true;
                line = pos->substr(pos->find_first_of("[{") + 2);
                std::vector<std::string> t;
                split(line, splitStr2, t);
                if (t.size() == 2) {
                    listKey = t[0];
                    listValue = t[1];
                    deleteFirstAndLast(listKey, '\"');
                    deleteFirstAndLast(listValue, '\"');
                    tulingResponse.json.push_back(
                            std::make_pair(listKey, listValue));
                }
            } else {
                line = *pos;
                std::vector<std::string> t;
                split(line, splitStr2, t);
                if (t.size() == 2) {
                    listKey = t[0];
                    listValue = t[1];
                    deleteFirstAndLast(listKey, '\"');
                    deleteFirstAndLast(listValue, '\"');
                    if (listKey == "code") {
                        tulingResponse.code = listValue;
                    } else if (listKey == "text") {
                        tulingResponse.text = listValue;
                    } else if (listKey == "url") {
                        tulingResponse.url = listValue;
                    }
                }
            }
        }
    }
}

void TulingApi::deleteFirstAndLast(std::string &s, const char c) {
    if (s.empty()) {
        return;
    }
    s.erase(0, s.find_first_not_of(c));
    s.erase(s.find_last_not_of(c) + 1);
}

bool TulingApi::getResponse(const std::string &info, TLRES &tulingResponse,
        const std::string &userid, const std::string &lon,
        const std::string &lat) {
    if(defaultUid.empty()||defaultUid.length()==0){
        defaultUid = generateUid();
    }
    std::string data;
    if (curl != NULL) {
        curl_easy_cleanup(curl);
        curl = NULL;
    }
    std::string sessionUrl = url + key;
    if (!userid.empty() && userid.length() > 0) {
        sessionUrl += "&userid=" + userid;
    } else {
        sessionUrl += "&userid=" + defaultUid;
    }
    if (!lon.empty() && lon.length() > 0) {
        sessionUrl += "&lon=" + lon;
    } else {
        sessionUrl += "&lon=" + defaultLon;
    }
    if (!lat.empty() && lat.length() > 0) {
        sessionUrl += "&lat=" + lat;
    } else {
        sessionUrl += "&lon=" + defaultLat;
    }
    if (!info.empty() && info.length() > 0) {
        sessionUrl += "&info=" + info;
    }
    curl = curl_easy_init();
    if (!curl) {
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, sessionUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }
    curl_easy_cleanup(curl);
    curl = NULL;
    parse(data, tulingResponse);
    return true;
}

size_t TulingApi::writeToString(void *ptr, size_t size, size_t count,
        void *stream) {
    ((std::string*) stream)->append((char*) ptr, 0, size * count);
    return size * count;
}

} /* namespace qyh */
