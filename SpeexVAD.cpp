/************************************************
 *  
 *  DM-Release@build:SpeexVAD.cpp
 *
 *  Created on: Sep 30, 2014
 *  Author: Andy Huang
 *	Email: andyspider@126.com
 *
 *	All rights reserved!
 *	
 ************************************************/

#include "SpeexVAD.h"
#include <stdio.h>

SpeexVAD::SpeexVAD(int ps, int pc) :
        preState(NULL), frameSize(0), probStart(ps), probContinue(pc) {
}

SpeexVAD::~SpeexVAD() {
    if (preState != NULL){
        speex_preprocess_state_destroy(preState);
        preState = NULL;
    }
}

int SpeexVAD::initialize(int framesize, unsigned int rate) {
    int vad = 1;
    float f;
    int i = 1;
    preState = speex_preprocess_state_init(framesize, rate);
    if (!preState) {
        printf("--- init speex failed\n");
        return -1;
    }
    int vadProbStart = probStart;
    int vadProbContinue = probContinue;

    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_VAD, &vad);
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_PROB_START,
            &vadProbStart);
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_PROB_CONTINUE,
            &vadProbContinue);
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_DENOISE, &i);
    int noiseSuppress = -40;
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,
            &noiseSuppress);
    i = 0;
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_AGC, &i);
    f = 16000;
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &f);
    i = 0;
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_DEREVERB, &i);
    f = .0;
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f = .0;
    speex_preprocess_ctl(preState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
    frameSize = framesize;
    return 0;
}

bool SpeexVAD::isVoice(void *data, int samplesize) {
    if (samplesize != frameSize) {
        printf("!!! sample size: %d != framesize: %d\n", samplesize, frameSize);
        return false;
    }
    spx_int16_t *ptr = (spx_int16_t *) data;		// S16
    if (speex_preprocess_run(preState, ptr))
        return true;
    else
        return false;
}
