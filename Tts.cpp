/*
 * Tts.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */
#include <time.h>
#include <stdio.h>
#include <iostream>
#include "Tts.h"

namespace qyh {

long Tts::getTimeUSec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}
Tts::Tts() {
    default_pcmwavhdr
    = { {   'R', 'I', 'F', 'F'}, 0,
        {   'W', 'A', 'V', 'E'}, {'f', 'm', 't', ' '}, 16, 1, 1, 16000,
        32000, 2, 16, {'d', 'a', 't', 'a'}, 0};
    login_configs= " appid = xxxxxx, work_dir =   .  ";
    param_without_number=
    "vcn=xiaoyan,aue = speex-wb,auf=audio/L16;rate=16000,spd = 5,vol = 5,tte = utf8";
}

Tts::~Tts() {
    MSPLogout();
}

bool Tts::convert(const char *text, char *&pcm_data, int &pcm_len,
        const bool isNumber) {
    pcm_len = 0;
    int ret;
    const char* sess_id = NULL;
    unsigned int audio_len = 0;
    int synth_status = 1;
    string param = param_without_number;
    if (isNumber) {
        param += ",rdn=2";
    }
    sess_id = QTTSSessionBegin(param.c_str(), &ret);
    if (ret != MSP_SUCCESS) {
        printf("QTTSSessionBegin: qtts begin session failed Error code %d.\n",
                ret);
        return ret;
    }

    ret = QTTSTextPut(sess_id, text, strlen(text), NULL);
    if (ret != MSP_SUCCESS) {
        printf("QTTSTextPut: qtts put text failed Error code %d.\n", ret);
        QTTSSessionEnd(sess_id, "TextPutError");
        return ret;
    }
    int x = 0;
    long tBegin = getTimeUSec();
    while (true) {
        const void *data = QTTSAudioGet(sess_id, &audio_len, &synth_status,
                &ret);
        if (NULL != data) {
            //TODO : memcpy to pcm_data
            //pcm_len+=memcpy(pcm_data+pcm_len,data,audio_len);
            player.play((char *) data, audio_len);   //now play derectly
            x += audio_len;
        }
        usleep(10000);
        if (synth_status == 2 || ret != 0)
            break;
    }
    long tEnd = getTimeUSec();
    if (125 * x / 4 > (tEnd - tBegin)) {    //640 ==> 20 ms ==>20 *1000
        usleep(125 * x / 4 - (tEnd - tBegin));
    }
    ret = QTTSSessionEnd(sess_id, NULL);
    if (ret != MSP_SUCCESS) {
        printf("QTTSSessionEnd: qtts end failed Error code %d.\n", ret);
    }
}

bool Tts::init() {
//    登录
    int ret = MSPLogin(NULL, NULL, login_configs.c_str());
    if (ret != MSP_SUCCESS) {
        printf("MSPLogin failed , Error code %d.\n", ret);
        return false;
    }
    if (!player.init()) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1player  init failed , Error code %d.\n",
                ret);
        return false;
    }
    return true;
}

} /* namespace qyh */
