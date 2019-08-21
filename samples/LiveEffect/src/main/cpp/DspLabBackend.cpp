//
// Created by Omri Carmi on 13/04/2019.
//

#include "DspLabBackend.h"

#include <cstring>
#include <iostream>
#include <logging_macros.h>
#include <mutex>
#include <pthread.h>
#include "Consts.h"
#include "DspLab.h"

using namespace std;

const int BLOCK_SIZE = AUDIO_PROCESS_BUFFER_SIZE;
FifoBuffer<float,BLOCK_SIZE> inBuffer[2];
FifoBuffer<float,BLOCK_SIZE> outBuffer[2];
enum STEREO_SIDE {RIGHT = 0, LEFT};
std::mutex currentBufferIdLock;
int currentProcessBufferId;
pthread_t tid;
void* processBlockAudioBackend(void*);
float measureTimeMiliSec(struct timeval *tvBefore,struct timeval *tvAfter);
void initOutBuffers() {
    float zeros[BLOCK_SIZE] = {0};
    outBuffer[0].pushAndOverride(zeros,BLOCK_SIZE);
    outBuffer[1].pushAndOverride(zeros,BLOCK_SIZE);
}

void processAudioBackend(float *inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,
                         int32_t outNumChannel, int32_t outNumFrames, bool isBlockDataOn) {
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
            pthread_create(&tid, NULL, &processBlockAudioBackend, NULL);
            currentBufferIdLock.unlock();
        }
    }else{
        memcpy(outData,inData,std::min(inNumSamples,outNumSamples) * sizeof(float));
    }
}

float blockDataOutTmp[BLOCK_SIZE];
float blockDataInTmp[BLOCK_SIZE];
struct timeval timeBefore;
struct timeval timeAfter;
bool timeMeasureSuccess = true;
float timeTookMs;
void* processBlockAudioBackend(void *arg) {
    //TODO maybe don't lock all the life of the func. do a loop and lock at each inner iteration.
    currentBufferIdLock.lock();
    // read from input
    const int blockSizeEffective = inBuffer[currentProcessBufferId].pop(blockDataInTmp,BLOCK_SIZE);
    // pad empty end part of block with zeros
    for (int i = blockSizeEffective; i < BLOCK_SIZE; ++i) {
        blockDataInTmp[i] = 0;
    }
    // measure time before process
    timeMeasureSuccess = true;
    if (gettimeofday(&timeBefore,NULL) != 0){
        LOGE("Failed to measure time for audio processing!\n");
        timeMeasureSuccess = false;
    }
    // call audio process frontend
    processAudio(blockDataInTmp,blockDataOutTmp);
    // measure time after process
    if (timeMeasureSuccess && gettimeofday(&timeAfter,NULL) != 0){
        LOGE("Failed to measure time for audio processing!\n");
        timeMeasureSuccess = false;
    }
    // write to output
    outBuffer[currentProcessBufferId].push(blockDataOutTmp,BLOCK_SIZE);
    currentBufferIdLock.unlock();
    // log time measurement
    if (timeMeasureSuccess)
    {
        timeTookMs = measureTimeMiliSec(&timeBefore,&timeAfter);
        LOGD("Block Process Took[msec]: %.2f",timeTookMs);
    }
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

float measureTimeMiliSec(struct timeval *tvBefore,struct timeval *tvAfter)
{
    return
    static_cast<float>((tvAfter->tv_sec - tvBefore->tv_sec) * 1e6 + (tvAfter->tv_usec - tvBefore->tv_usec))
            / 1000.0f;
}