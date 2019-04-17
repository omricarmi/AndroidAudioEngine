//
// Created by Omri Carmi on 13/04/2019.
//

#ifndef SAMPLES_PROCESSAUDIO_H
#define SAMPLES_PROCESSAUDIO_H

#include <cstdint>

void processAudio(float* inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,int32_t outNumChannel, int32_t outNumFrames);

#endif //SAMPLES_PROCESSAUDIO_H
