//
// Created by Omri Carmi on 19/08/2019.
//

#include <cstring>
#include "DspLab.h"
#include "Consts.h"

const int block_size = AUDIO_PROCESS_BUFFER_SIZE;

void processAudio(float *inData, float *outData)
{
    memcpy(outData,inData,block_size * sizeof(float));
}