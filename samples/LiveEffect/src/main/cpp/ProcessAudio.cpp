//
// Created by Omri Carmi on 13/04/2019.
//

#include <assert.h>
#include <cstring>
#include "ProcessAudio.h"

enum STEREO_SIDE {RIGHT = 0, LEFT};
void processAudio(float *inData, int32_t inNumChannel, int32_t inNumFrames, float *outData,
                  int32_t outNumChannel, int32_t outNumFrames) {
    // presume in examples below that frames and channels are the same at input and ouput. but can make them be different
    const int32_t numSamples = inNumFrames * inNumChannel;

//    /** example of moving sound untouched to output **/
//    memcpy(outData,inData,numSamples* sizeof(float));

//    /** example of amplifying sound **/
//    for (int i = 0; i < numSamples; ++i) {
//        outData[i] = 10*inData[i];
//    }

    /** example to out put just to one channel **/
    STEREO_SIDE side = RIGHT;
    for (int i = 0; i < numSamples; ++i) {
        if (i%2 == side) {
            outData[i] = 0;
        } else {
            outData[i] = inData[i];
        }
    }
}
