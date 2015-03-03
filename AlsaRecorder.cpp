/*
 * AlsaRecorder.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */

#include <iostream>

#include "AlsaRecorder.h"

#define PCM_SAMPLE_RATE         16000
#define PCM_BUFFER_UNIT_LEN     160
#define ONE_LOOP_MAX_UNIT       2

namespace qyh {

AlsaRecorder::AlsaRecorder() :
        capture_handle(NULL), capture_data(NULL) {
    rate = PCM_SAMPLE_RATE;
    format = SND_PCM_FORMAT_S16_LE;
    channels = 1;
    loopTime = ONE_LOOP_MAX_UNIT;
    _period_size = PCM_BUFFER_UNIT_LEN;
    _period_bytes = _period_size * 2;
}

AlsaRecorder::~AlsaRecorder() {
    if (capture_data != NULL) {
        free(capture_data);
    }
    snd_pcm_close(capture_handle);
}

bool AlsaRecorder::init(std::string &errMessage) {
    snd_pcm_hw_params_t *hw_params;
    int err;
    char *handler;
    //handler = "Capture Source";
    handler = "plug:dsnooped";
    if ((err = snd_pcm_open(&capture_handle, handler,
            SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        if ((err = snd_pcm_open(&capture_handle, "default",
                SND_PCM_STREAM_CAPTURE, 0)) < 0) {
            std::string alsaErr = snd_strerror(err);
            errMessage = "ALSA::cannot open audio device default:" + alsaErr;
            return false;
        }
    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot allocate hardware parameter structure :"
                + alsaErr;
        return false;
    }

    std::cout << "ALSA::snd_pcm_hw_params_malloc OK" << std::endl;
    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot initialize hardware parameter structure:"
                + alsaErr;
        return false;
    }
    std::cout << "ALSA::snd_pcm_hw_params_any OK" << std::endl;
    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,
            SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot set access type:" + alsaErr;
        return false;
    }
    std::cout << "ALSA::snd_pcm_hw_params_set_access OK" << std::endl;
    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, format))
            < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot set sample format:" + alsaErr;
        return false;
    }

    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate,
            0)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot set sample rate:" + alsaErr;
        return false;
    }

    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params,
            channels)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot set channel count:" + alsaErr;
        return false;
    }
    std::cout << "ALSA::snd_pcm_hw_params_set_channels OK" << std::endl;

    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot set parameters :" + alsaErr;
        return false;
    }
    std::cout << "ALSA::snd_pcm_hw_params OK" << std::endl;
    snd_pcm_hw_params_free(hw_params);
    std::cout << "ALSA::snd_pcm_hw_params_free OK" << std::endl;
    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
        std::string alsaErr = snd_strerror(err);
        errMessage = "ALSA::cannot prepare audio interface for use:" + alsaErr;
        return false;
    }
    capture_data = (unsigned char *) malloc(
            _period_bytes * sizeof(unsigned char));
    std::cout << "ALSA:: init OK!" << std::endl;
    return true;
}
bool AlsaRecorder::record(unsigned char *&data, int &data_len,
        std::string &errMessage) {
    int err;
    int getNumber = 0;
    if (data == NULL) {
        data = (unsigned char *) malloc(
                _period_bytes * loopTime * sizeof(unsigned char));
    }
    data_len = 0;
    while (getNumber < loopTime) {
        if ((err = snd_pcm_readi(capture_handle, capture_data, _period_size))
                != _period_size) {
            std::string alsaErr = snd_strerror(err);
            std::cout
                    << "read from audio interface failed :=================== "
                    << alsaErr.c_str() << std::endl;
            {
                snd_pcm_close(capture_handle);
                std::string strErrMsg;
                init(strErrMsg);
            }
            break;
        }
        memcpy(data + getNumber * _period_bytes, capture_data, _period_bytes);
        data_len += _period_bytes;
        getNumber++;
    }
    return true;
}

} /* namespace qyh */
