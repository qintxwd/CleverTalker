/*
 * AlsaPlayer.h
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */

#ifndef ALSAPLAYER_H_
#define ALSAPLAYER_H_
#include <string>
#include <errno.h>
#include <string.h>
#include <alsa/asoundlib.h>

using std::string;

#define SAMPLE_RATE 16000
#define CHANNELS 1
#define DATASZ 640      // Bytes

namespace qyh {

class AlsaPlayer {
public:
    AlsaPlayer();
    virtual ~AlsaPlayer();
    bool init();
    void play(const char *pcm_data, const int pcm_len);
    void waitForPlayFinished();
private:
    snd_pcm_t *pcm_handle;
    int bytes_per_frame;
    int bytes_per_second;
};

} /* namespace qyh */

#endif /* ALSAPLAYER_H_ */
