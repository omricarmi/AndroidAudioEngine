//
// Created by Omri Carmi on 19/04/2019.
//

#ifndef SAMPLES_FIFOBUFFER_H
#define SAMPLES_FIFOBUFFER_H

#include <cstdint>
#include <cstring>
#include <assert.h>

template <class T,int32_t SIZE>
class FifoBuffer {
public:
    int32_t push(T* source,int32_t size);
    int32_t pushAndOverride(T *source, int32_t size);
    int32_t pop(T* destination,int32_t size=0);
    int32_t size();
    int32_t sizeMax();
private:
    T mData[SIZE];
    const int32_t mSizeFull=SIZE;
    int32_t mSize=0;
    int32_t mNextWriteIdx = 0;
    int32_t mNextReadIdx = 0;

};

template<class T, int32_t SIZE>
int32_t FifoBuffer<T, SIZE>::push(T *source, int32_t size) {
    size = (size > mSizeFull - mSize) ? (mSizeFull - mSize) : size;
    for (int i = 0; i < size; ++i) {
        int nextWriteIdx = (mNextWriteIdx + i) % mSizeFull;
        mData[nextWriteIdx] = source[i];
    }
    mNextWriteIdx = (mNextWriteIdx + size) % mSizeFull;
    mSize += size;
    return size;
}

template<class T, int32_t SIZE>
int32_t FifoBuffer<T, SIZE>::pushAndOverride(T *source, int32_t size) {
    for (int i = 0; i < size; ++i) {
        int nextWriteIdx = (mNextWriteIdx + i) % mSizeFull;
        mData[nextWriteIdx] = source[i];
    }
    mNextWriteIdx = (mNextWriteIdx + size) % mSizeFull;
    mSize = ((size + mSize) < mSizeFull) ? (size + mSize) : mSizeFull;
    // if we override data we need to increment the read to the "new oldest" data
    if (size + mSize > mSizeFull) {
        mNextReadIdx = (mNextWriteIdx) % mSizeFull;
    }
    return size;
}

template<class T, int32_t SIZE>
int32_t FifoBuffer<T, SIZE>::pop(T *destination, int32_t size) {
    size = (size > mSize) ? mSize : size;
    for (int i = 0; i < size; ++i) {
        int nextReadIdx = (mNextReadIdx + i) % mSizeFull;
        destination[i] = mData[nextReadIdx];
    }
    mNextReadIdx = (mNextReadIdx + size) % mSizeFull;
    mSize -= size;
    return size;
}

template<class T, int32_t SIZE>
int32_t FifoBuffer<T, SIZE>::size() {
    return mSize;
}

template<class T, int32_t SIZE>
int32_t FifoBuffer<T, SIZE>::sizeMax() {
    return mSizeFull;
}


#endif //SAMPLES_FIFOBUFFER_H
