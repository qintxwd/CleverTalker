/*
 * AlsaPlayer.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */

#include "AlsaPlayer.h"

namespace qyh {

AlsaPlayer::AlsaPlayer() :
        pcm_handle(NULL), bytes_per_frame(-1), bytes_per_second(-1) {
}

AlsaPlayer::~AlsaPlayer() {
    snd_pcm_drop(pcm_handle);
    snd_pcm_close(pcm_handle);
    pcm_handle = NULL;
}

void AlsaPlayer::play(const char *pcm_data, const int pcm_size) {
    char *pcm_ptr = const_cast<char*>(pcm_data);
    int pcm_len = pcm_size / bytes_per_frame;
    while (pcm_len > 0) {
        snd_pcm_sframes_t written = snd_pcm_writei(pcm_handle, pcm_ptr,
                pcm_len);
        if (written < 0) {
            if (written == -EPIPE) {
                snd_pcm_prepare(pcm_handle);    // this is important!!
            } else if (written == EAGAIN) {
                snd_pcm_wait(pcm_handle, 100);
            } else {
                snd_pcm_recover(pcm_handle, written, 1);
            }
        } else if (written == pcm_len) {
            pcm_len -= written;
            pcm_ptr += written;
        } else if (written < pcm_len) {
            snd_pcm_wait(pcm_handle, 100);
        }
    }
//    int ret = snd_pcm_writei(pcm_handle, pcm_data, pcm_size / bytes_per_frame);
//    if (ret < 0) {
//        if (ret == -EPIPE) {
//            printf("!!! PCM underrun occurred, %s!\n", snd_strerror(ret));
//            snd_pcm_prepare(pcm_handle);    // this is important!!
//        } else {
//            printf("---!!! PCM write error: %s!\n", snd_strerror(ret));
//            snd_pcm_recover(pcm_handle, ret, 0);
//        }
//    }
}
bool AlsaPlayer::init() {
    int ret = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not open audio device!\n");
        return false;
    }

    snd_pcm_hw_params_t *params;
    ret = snd_pcm_hw_params_malloc(&params);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not allocate audio hardware parameter structure!\n");
        return false;
    }

    ret = snd_pcm_hw_params_any(pcm_handle, params);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not initialize hardware parameter structure!\n");
        return false;
    }

    ret = snd_pcm_hw_params_set_access(pcm_handle, params,
            SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not set access type!\n");
        return false;
    }

    // set pcm format
    unsigned int rate = SAMPLE_RATE;
    unsigned int channels = CHANNELS;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    bytes_per_frame = channels * 2;    // S16_LE is 2 Bytes
    bytes_per_second = bytes_per_frame * rate;
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
    snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    snd_pcm_hw_params_set_format(pcm_handle, params, format);

    ret = snd_pcm_hw_params(pcm_handle, params);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not set audio parameters!\n");
        return -1;
    }

    // get default buffer and period size, this must be called AFTER snd_pcm_hw_params()
    snd_pcm_uframes_t psz;
    snd_pcm_uframes_t bsz;
    snd_pcm_hw_params_get_period_size(params, &psz, NULL);
    snd_pcm_hw_params_get_buffer_size(params, &bsz);

    // dump info
    snd_output_t* out;
    snd_pcm_hw_params_free(params);

    // software parameters
    snd_pcm_sw_params_t *swparams;
    snd_pcm_sw_params_malloc(&swparams);
    snd_pcm_sw_params_current(pcm_handle, swparams);

    ret = snd_pcm_sw_params_set_start_threshold(pcm_handle, swparams, 0); // set start-thresh to a big enough value to prevent noise when starting
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! can not set start threshold!\n");
        return false;
    }

    ret = snd_pcm_sw_params_set_stop_threshold(pcm_handle, swparams,
            bsz + DATASZ);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not set stop threshold!\n");
        return false;
    }

    ret = snd_pcm_sw_params(pcm_handle, swparams);
    if (ret < 0) {
        printf(
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!can not set audio software parameters!\n");
        return false;
    }

    snd_pcm_sw_params_free(swparams);

    ret = snd_pcm_prepare(pcm_handle);
    if (ret < 0) {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio prepare failed!\n");
        return false;
    }
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!alsa init OKOKOKOKOKOK!\n");
    return true;
}
} /* namespace qyh */
