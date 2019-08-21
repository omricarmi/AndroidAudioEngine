//
// Created by Omri Carmi on 13/04/2019.
//

#ifndef SAMPLES_DSAPLAB_H
#define SAMPLES_DSAPLAB_H

#include <cstdint>
#include "FifoBuffer.h"

void initOutBuffers();
void processAudioBackend(float *inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,
                  int32_t outNumChannel, int32_t outNumFrames,bool isBlockDataOn);
void debugExample();

#endif //SAMPLES_DSAPLAB_H
