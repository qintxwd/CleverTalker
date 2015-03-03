/*
 * AlsaRecorder.h
 *
 *  Created on: Jan 27, 2015
 *      Author: qinyinghao
 */

#ifndef ALSARECORDER_H_
#define ALSARECORDER_H_
#include <string>
#include <alsa/asoundlib.h>

namespace qyh {

class AlsaRecorder {
public:
    AlsaRecorder();
    virtual ~AlsaRecorder();
    bool init(std::string &errMessage);
    bool record(unsigned char *&data, int &data_len, std::string &errMessage);
private:
    snd_pcm_t *capture_handle;
    snd_pcm_uframes_t _period_size;
    unsigned int _period_bytes;
    unsigned int rate;
    snd_pcm_format_t format;
    int channels;
    int loopTime;
    //data:
    unsigned char *capture_data;
};

} /* namespace qyh */

#endif /* ALSARECORDER_H_ */
