//
// Created by Omri Carmi on 13/04/2019.
//

#include <cstring>
#include <iostream>
#include <logging_macros.h>
#include <mutex>
#include <pthread.h>
#include "DspLab.h"

using namespace std;

const int BLOCK_SIZE = 4096;
FifoBuffer<float,BLOCK_SIZE> inBuffer[2];
FifoBuffer<float,BLOCK_SIZE> outBuffer[2];
enum STEREO_SIDE {RIGHT = 0, LEFT};
std::mutex currentBufferIdLock;
int currentProcessBufferId;
pthread_t tid;
void* processBlockAudio(void*);

void initOutBuffers() {
    float zeros[BLOCK_SIZE] = {0};
    outBuffer[0].pushAndOverride(zeros,BLOCK_SIZE);
    outBuffer[1].pushAndOverride(zeros,BLOCK_SIZE);
}

void processAudio(float *inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,
                  int32_t outNumChannel, int32_t outNumFrames,bool isBlockDataOn) {
    const int32_t inNumSamples = inNumFrames * inNumChannel;
    const int32_t outNumSamples = outNumFrames * outNumChannel;
    if (isBlockDataOn) {
        // write in to input buffer
        int inNumSamplesEffective = inBuffer[currentProcessBufferId^1].push(inData,inNumSamples);
        // write out from output buffer
        int outNumSamplesEffective = outBuffer[currentProcessBufferId^1].pop(outData,outNumSamples);
        LOGD("In: %d, Out: %d",inNumSamplesEffective,outNumSamplesEffective);
        // decide if time to switch buffers
        bool isOutBufferEmpty = (outBuffer[currentProcessBufferId^1].size() == 0);
        if (isOutBufferEmpty && currentBufferIdLock.try_lock()) {
            currentProcessBufferId ^= 1;
            pthread_create(&tid, NULL, &processBlockAudio, NULL);
            currentBufferIdLock.unlock();
        }
    }else{
        memcpy(outData,inData,std::min(inNumSamples,outNumSamples) * sizeof(float));
    }
}

void* processBlockAudio(void* arg) {
    //TODO maybe don't lock all the life of the func. do a loop and lock at each inner iteration.
    float blockDataTmp[BLOCK_SIZE];
    currentBufferIdLock.lock();
    // read from input
    int blockSizeEffective = inBuffer[currentProcessBufferId].pop(blockDataTmp,BLOCK_SIZE);
    // process Example amplify by factor of 10
    for (int i = 1; i < BLOCK_SIZE; ++i) {
      blockDataTmp[i] *= 10;
    }
    // write to output
    outBuffer[currentProcessBufferId].push(blockDataTmp,blockSizeEffective);
    currentBufferIdLock.unlock();

    return NULL;
//    const int32_t numSamples = numFrames * numChannel;
//    /** example of moving sound untouched to output **/
//    memcpy(outData,inData,numSamples* sizeof(float));
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

void debugExample() {
    LOGD("debug example called.");
}
