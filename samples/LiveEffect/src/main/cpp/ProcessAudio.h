//
// Created by Omri Carmi on 13/04/2019.
//

#ifndef SAMPLES_PROCESSAUDIO_H
#define SAMPLES_PROCESSAUDIO_H

#include <cstdint>
#include "FifoBuffer.h"
void processAudio(float *inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,
                  int32_t outNumChannel, int32_t outNumFrames,bool isBlockDataOn);
void processBlockAudio(float *inData, float *outData, int32_t numChannel, int32_t numFrames);
#endif //SAMPLES_PROCESSAUDIO_H
