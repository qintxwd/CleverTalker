/*
 * BaiduTuling.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: qinyinghao
 */

#include <iostream>

#include "BaiduTuling.h"
using namespace std;

namespace qyh {

volatile bool goResponsing = false;

BaiduTuling::BaiduTuling() :
        pcmDataBuff(NULL), pcmDataBuffLen(0), CPing(false) {
}

BaiduTuling::~BaiduTuling() {
}

bool BaiduTuling::init(std::string &err) {
    CPing = false;
    if (!vtt.init()) {
        err = "vtt init  fail";
        std::cout << err << std::endl;
        return false;
    }
    int ret = pthread_create(&pt_record, NULL, ContinuousRecord, this);
    if (ret != 0) {
        return false;
    }
    ret = pthread_create(&pt_response, NULL, SpeekResponse, this);
    if (ret != 0) {
        return false;
    }

    return true;
}

void *BaiduTuling::ContinuousRecord(void *arg) {
    if (arg == NULL) {
        return NULL;
    }
    BaiduTuling *bt = reinterpret_cast<BaiduTuling *>(arg);
    bt->continueRecordProcess();
    return NULL;
}

void *BaiduTuling::SpeekResponse(void *arg) {
    BaiduTuling *bt = reinterpret_cast<BaiduTuling *>(arg);
    bt->SpeekResponseProcess();
    return NULL;
}
void BaiduTuling::continueRecordProcess() {
    SpeexVAD *svad;
    string err;
    int continueNotVoice = 0;
    unsigned char *pcmData = NULL;
    unsigned char CheckBuff[CHECK_BUFF_MAX_LEN];
    int pcmLen = 0;
    AlsaRecorder ar;
    if (!ar.init(err)) {
        std::cout << "alsa recorder init fail" << err << endl << endl;
        return;
    }
    int changeLine = 0;
    while (true) {
        if (!ar.record(pcmData, pcmLen, err)) {
            std::cout << "alsa recorder record fail" << err << endl;
            //TODO
            continue;//return;
        } else {
            if (goResponsing)
                continue;
            ++changeLine;
            if (changeLine >= 100) {
                cout << "\n";
                cout.flush();
                changeLine = 0;
            }
            if (pcmData == NULL && pcmLen <= 0) {
                continue;
            } else if (pcmDataBuff == NULL) {
                pcmDataBuff = (unsigned char *) malloc(BUFF_MAX_LEN);
            }
            //vad
            if (svad == NULL) {
                svad = new SpeexVAD(85);
                if (svad->initialize(640)) {
                    std::cout << "SpeexVAD initialize fail" << err << endl
                            << endl;
                    std::cout << "SpeexVAD initialize fail" << err << endl;
                    return;
                }
            }
            memcpy(CheckBuff, pcmData, pcmLen);
            if (svad->isVoice(CheckBuff, pcmLen)) {
                cout << "Y";
                cout.flush();
                continueNotVoice = 0;
                if (pcmDataBuffLen + pcmLen >= BUFF_MAX_LEN) {
                    CPing = false;
                    goResponsing = true;
                    delete (svad);
                    svad = NULL;
                    continue;
                }
                if (CPing) {
                    memcpy(pcmDataBuff + pcmDataBuffLen, CheckBuff, pcmLen);
                    pcmDataBuffLen += pcmLen;
                } else {
                    CPing = true;
                    memcpy(pcmDataBuff + pcmDataBuffLen, CheckBuff, pcmLen);
                    pcmDataBuffLen += pcmLen;
                }
            } else {
                cout << "N";
                cout.flush();
                if (CPing) {
                    ++continueNotVoice;
                    if (continueNotVoice > 40) {
                        CPing = false;
                        goResponsing = true;
                        delete (svad);
                        svad = NULL;
                    } else {
                        if (pcmDataBuffLen + pcmLen >= BUFF_MAX_LEN) {
                            CPing = false;
                            goResponsing = true;
                            delete (svad);
                            svad = NULL;
                            continue;
                        }
                        memcpy(pcmDataBuff + pcmDataBuffLen, CheckBuff, pcmLen);
                        pcmDataBuffLen += pcmLen;
                    }
                }
            }
        }
    }
    return;
}
void BaiduTuling::SpeekResponseProcess() {
    string err;
    TulingApi ta;
    Tts tts;
    if (!tts.init()) {
        err = "tts init failed!";
        std::cout << err << std::endl;
        return;
    }
    while (true) {
        while (goResponsing == false) {
            usleep(200000);
        }
        if (pcmDataBuff == NULL || pcmDataBuffLen <= 0) {
            goResponsing = false;
            pcmDataBuffLen = 0;
            continue;
        }
        string request;
        if (!vtt.convert(pcmDataBuff, pcmDataBuffLen, request, err)) {
            std::cout << "Voice to text  convert  fail" << err << endl;
            goResponsing = false;
            pcmDataBuffLen = 0;
            continue;
        }
        if (request.length() == 0) {
            goResponsing = false;
            pcmDataBuffLen = 0;
            continue;
        }
        TulingApi::TLRES response;
        if (!ta.getResponse(request, response)) {
            std::cout << "TulingApi getResponse failed!" << std::endl;
            goResponsing = false;
            pcmDataBuffLen = 0;
            continue;
        }
        if (response.code != "100000") {
            std::cout << "code!= 100000 text:" << response.text << std::endl;
            goResponsing = false;
            pcmDataBuffLen = 0;
            continue;
        }
        string tresponse = response.text;
        std::cout << "response  =" << tresponse << std::endl;
        if (tresponse.length() == 0) {
            goResponsing = false;
            pcmDataBuffLen = 0;
            continue;
        }
        int i = 0;
        char *ccc;
        tts.convert(tresponse.c_str(), ccc, i, false);
        goResponsing = false;
        pcmDataBuffLen = 0;
    }
    return;
}
} /* namespace qyh */
