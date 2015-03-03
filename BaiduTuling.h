/*
 * BaiduTuling.h
 *
 *  Created on: Mar 2, 2015
 *      Author: qinyinghao
 */

#ifndef BAIDUTULING_H_
#define BAIDUTULING_H_
#include <pthread.h>

#include <string>

#include "Tts.h"
#include "Vtt.h"
#include "AlsaRecorder.h"
#include "AlsaPlayer.h"
#include "TulingApi.h"
#include "SpeexVAD.h"

namespace qyh {

class BaiduTuling {
public:
    BaiduTuling();
    virtual ~BaiduTuling();
    bool init(std::string &err);
public:
    static void *ContinuousRecord(void *arg);
    static void *SpeekResponse(void *arg);
private:
    void continueRecordProcess();
    void SpeekResponseProcess();
    static const int CHECK_BUFF_MAX_LEN = 6400;
    static const int BUFF_MAX_LEN = 320000;
    unsigned char *pcmDataBuff;
    int pcmDataBuffLen;
    bool CPing;
    pthread_t pt_record;
    pthread_t pt_response;
    Vtt vtt;
};

} /* namespace qyh */

#endif /* BAIDUTULING_H_ */
