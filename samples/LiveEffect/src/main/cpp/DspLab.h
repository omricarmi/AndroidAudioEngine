//
// Created by Omri Carmi on 19/08/2019.
//

#ifndef SAMPLES_DSPLAB_H
#define SAMPLES_DSPLAB_H

void processAudio(float *inData, float *outData);


#endif //SAMPLES_DSPLAB_H

/**
 *
static long int startUsecTime = 0 ;
static long int endUsecTime = 0 ;
static struct timeval initialUsecTime ;

static void ResetUsecTime (void)
{
    if (gettimeofday (&initialUsecTime, NULL) != 0) {
        ERR ("System call gettimeofday returned error." );
    }
}

void GetStartTime (void)
{
    struct timeval currentTime        ;

    ResetUsecTime () ;

    if (gettimeofday (&currentTime, NULL) != 0) {
        ERR ("System call gettimeofday returned error." );
    }
    else {
        startUsecTime = (  (     (currentTime.tv_sec - initialUsecTime.tv_sec)
                               * 1000000)
                            + (currentTime.tv_usec - initialUsecTime.tv_usec)) ;
    }
}

void GetEndTime (void)
{
    struct timeval currentTime        ;

    if (gettimeofday (&currentTime, NULL) != 0) {
        ERR ("System call gettimeofday returned error." );
    }
    else {
        endUsecTime = (  (  (currentTime.tv_sec - initialUsecTime.tv_sec)
                            * 1000000)
                          + (currentTime.tv_usec - initialUsecTime.tv_usec)) ;
    }
}

//... code here

   GetStartTime();
signal_processing((short*)inputBuffer, (short*)outputBuffer, (envPtr->blksize)/2);
GetEndTime();

//memcpy(outputBuffer, inputBuffer, envPtr->blksize);
sem_post(&envPtr->finished_proc);

usecTimeTaken = (endUsecTime - startUsecTime) ;

total_time_taken += usecTimeTaken;
total_samp_proc += ((envPtr->blksize)/2) / 2;

if (total_samp_proc > 3*11025)
{
double proc_rate;
proc_rate = ((1000000.0 * (double)(total_samp_proc))/(double)(total_time_taken));
//fprintf(stderr, "Average processing rate: %8.2f Samples/sec\n", proc_rate);
fprintf(stderr, "System utilization: %3.2f\%\n", (((double)SAMPLE_RATE) / proc_rate) * 100);

total_time_taken = 0;
total_samp_proc = 0;
}
**/