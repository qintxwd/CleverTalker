/*
 * Vtt.h
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */

#ifndef VTT_H_
#define VTT_H_
#include <string>

namespace qyh {

class Vtt {
public:
    Vtt();
    virtual ~Vtt();
    bool init();
    bool convert(const unsigned char *pcmData, int pcmLen, std::string &result,
            std::string &err);

public:
    static size_t writefunc(void *ptr, size_t size, size_t nmemb,
            char **result);
private:
    bool getAccessToken(std::string &accessToken);
    std::string accessToken;

};
} /* namespace qyh */

#endif /* VTT_H_ */
