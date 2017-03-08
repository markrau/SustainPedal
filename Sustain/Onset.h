//
//  Onset.h
//
//
//  Created by Mark Rau
//
//

#ifndef Onset_h
#define Onset_h

// Helpful Definitions
#define MAX_BUFFER_SIZE 2048

#include "dsplib.h"
#include "Q15Arithmetic.h"



class Onset {
public:
    Onset(int bufferLen, long previousBuffFFTSum);
    ~Onset();
    int isOnset(int* input, int thresh);
    long returnCurrFFT();
    long returnPrevFFT();

    
    
private:
    Q15arithmetic q;
    int _bufferLen;
    int* _buffer;
    long _previousBuffFFTSum;
    long currBuffFFTSum;
    long threshMultByBuff;

};

#endif /* Onset_h */
