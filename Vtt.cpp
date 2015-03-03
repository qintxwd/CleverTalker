/*
 * Vtt.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#include "base64.h"
#include "Vtt.h"

namespace qyh {

const int MAX_BODY_SIZE = 1000000;
const int MAX__SIZE = 512;
const char *cuid = "XXXX";
const char *apiKey = "XXXXXXXXXXXXXXXXXXXXXXX";
const char *secretKey = "XXXXXXXXXXXXXXXXXXXXXXXXX";
std::string resultTemp;

Vtt::Vtt() {
}

Vtt::~Vtt() {
}

bool Vtt::convert(const unsigned char *pcmData, int pcmLen, std::string &result,
        std::string &err) {
    if (pcmData == NULL || pcmLen <= 0) {
        err = "pcmData is null or pcmLen<=0";
        return false;
    }
    std::string decode_data = base64_encode(pcmData, pcmLen);
    if (0 == decode_data.length()) {
        err = "base64 encoded data is empty.";
        return false;
    }

    Json::Value buffer;
    Json::FastWriter trans;
    buffer["format"] = "pcm";
    buffer["rate"] = 16000;
    buffer["channel"] = 1;
    buffer["token"] = accessToken.c_str();
    buffer["cuid"] = cuid;
    buffer["speech"] = decode_data;
    buffer["len"] = pcmLen;

    char body[MAX_BODY_SIZE];
    memset(body, 0, sizeof(body));

    int bodyContentLen = trans.write(buffer).length();
    memcpy(body, trans.write(buffer).c_str(), bodyContentLen);

    CURL *curl;
    CURLcode res;
    char *resultBuf = NULL;
    struct curl_slist *headerlist = NULL;

    char tmp[MAX__SIZE];
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%s",
            "Content-Type: application/json; charset=utf-8");
    char host[MAX__SIZE];
    memset(host, 0, sizeof(host));
    snprintf(host, sizeof(host), "%s", "http://vop.baidu.com/server_api");
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Content-Length: %d", bodyContentLen);
    headerlist = curl_slist_append(headerlist, tmp);
    resultTemp = "";
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, host);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodyContentLen);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultBuf);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("perform curl error:%d.\n", res);
        return false;
    }
    curl_slist_free_all(headerlist);
    curl_easy_cleanup(curl);
    result = resultTemp;
    return true;
}

bool Vtt::init() {
    if (accessToken.length() <= 0) {
        if (!getAccessToken(accessToken)) {
            printf("could not get access token!\n");
            return false;
        }
    }
    return true;
}
size_t Vtt::writefunc(void *ptr, size_t size, size_t nmemb, char **result) {
    size_t result_len = size * nmemb;
    *result = (char *) realloc(*result, result_len + 1);
    if (*result == NULL) {
        printf("realloc failure!\n");
        return 1;
    }
    memcpy(*result, ptr, result_len);
    (*result)[result_len] = '\0';
    printf("%s\n", *result);
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(*result, value, false)) {
        int err_no = value["err_no"].asInt();
        if (err_no == 0) {
            resultTemp = value["result"][0].asString();
            std::cout << value["result"][0] << std::endl;
        } else {
            std::string err_msg = value["err_msg"].asString();
            std::cout << "baidu return error,errno = " << err_no << " msg= "
                    << err_msg << std::endl;
            if (err_msg.find("token") != std::string::npos
                    || err_msg.find("Token") != std::string::npos
                    || err_msg.find("TOKEN") != std::string::npos) {
                //TODO
                //needGetAccessToken = 1;
            }
        }
    }
    return result_len;
}
//get access token
bool Vtt::getAccessToken(std::string &accessToken) {
    std::string token;
    char host[MAX__SIZE];
    snprintf(host, sizeof(host),
            "https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s",
            apiKey, secretKey);
    FILE* fpp = NULL;
    char cmd[MAX__SIZE];
    char result[MAX__SIZE];
    //TODO: command curl can't find in arm-linux
    char* curl_cmd = "curl -s ";
    char* yinhao = "\"";
    strcpy(cmd, curl_cmd);
    strcat(cmd, yinhao);
    strcat(cmd, host);
    strcat(cmd, yinhao);
    fpp = popen(cmd, "r");
    fgets(result, MAX__SIZE, fpp);
    pclose(fpp);

    if (result != NULL) {
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(result, root, false)) {
            token = root.get("access_token", "").asString();
        }
    }
    if (token.length() > 0) {
        accessToken = token;
        std::cout << "accessToken=" << accessToken << std::endl;
        return true;
    }
    return false;
}
} /* namespace qyh */
