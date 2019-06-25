//
// Created by Omri Carmi on 13/04/2019.
//

#include <assert.h>
#include <cstring>
#include <logging_macros.h>
#include "DspLab.h"

const int BLOCK_SIZE = 4096;
const int DATA_SAFETY_POWER = 8;
FifoBuffer<float,DATA_SAFETY_POWER*BLOCK_SIZE> inBuffer;
FifoBuffer<float,DATA_SAFETY_POWER*BLOCK_SIZE> outBuffer;
float inBlockData[DATA_SAFETY_POWER*BLOCK_SIZE];
float outBlockData[DATA_SAFETY_POWER*BLOCK_SIZE];
enum STEREO_SIDE {RIGHT = 0, LEFT};

void processAudio(float *inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,
                  int32_t outNumChannel, int32_t outNumFrames,bool isBlockDataOn) {
    const int32_t inNumSamples = inNumFrames * inNumChannel;
    const int32_t outNumSamples = outNumFrames * outNumChannel;
    if (isBlockDataOn) {
        // aggregate input to buffer
        inBuffer.push(inData,inNumSamples);
        // when input data in size of BLOCK_SIZE is available process it
        if (inBuffer.size() >= BLOCK_SIZE*inNumChannel) {
            inBuffer.pop(inBlockData,BLOCK_SIZE*inNumChannel);
            processBlockAudio(inBlockData,outBlockData,BLOCK_SIZE,inNumChannel);
            outBuffer.push(outBlockData,BLOCK_SIZE*inNumChannel);
        }
        // output process data as much as device can consume and as much as already has been processed
        const int32_t outNumSamples = outNumFrames * outNumChannel;
        outBuffer.pop(outData,outNumSamples);
    }else{
        processBlockAudio(inData,outData,inNumFrames,inNumChannel);
    }
}

//frames and channels sizes are the same at input and ouput
void processBlockAudio(float *inData, float *outData, int32_t numChannel, int32_t numFrames) {
    const int32_t numSamples = numFrames * numChannel;
    /** example of moving sound untouched to output **/
    memcpy(outData,inData,numSamples* sizeof(float));
//    /** OR **/
//    for (int i = 0; i < numSamples; ++i) {
//        outData[i] = inData[i];
//    }
//    /** example of amplifying sound **/
//    for (int i = 0; i < numSamples; ++i) {
//        outData[i] = 20*inData[i];
//    }
//    /** example to output just to one channel **/
//    STEREO_SIDE side = LEFT;
//    for (int i = 0; i < numSamples; ++i) {
//        if (i%2 == side) {
//            outData[i] = 0;
//        } else {
//            outData[i] = inData[i];
//        }
//    }
}