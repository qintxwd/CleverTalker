/************************************************
 *  
 *  DM-Release@build:SpeexVAD.h
 *
 *  Created on: Sep 30, 2014
 *  Author: Andy Huang
 *	Email: andyspider@126.com
 *
 *	All rights reserved!
 *	
 ************************************************/

#ifndef PLUGINS_PCMPLAYER_SPEEXVAD_H_
#define PLUGINS_PCMPLAYER_SPEEXVAD_H_
#include <speex/speex_preprocess.h>

/*
 *
 */
class SpeexVAD
{
	public:
		SpeexVAD(int probStart = 80, int probContinue = 65);
		virtual ~SpeexVAD();
		int initialize(int framesize, unsigned int rate = 16000);
		bool isVoice(void *data, int samplesize);
	private:
		SpeexPreprocessState *preState;
		int frameSize;
		int probStart;
		int probContinue;
};

#endif /* PLUGINS_PCMPLAYER_SPEEXVAD_H_ */
