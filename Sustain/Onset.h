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
    Onset(int bufferLen, long previousBuffFFTSum, long FS);
    ~Onset();
    int isOnsetFFT(int* input, int thresh);
    int isOnsetLeaky(int* input);
    void setTauRelease(int tauRelease);
    void setTauAttack(int tauAttack);
    long returnCurrFFT();
    long returnPrevFFT();
    
    

    
    
private:
    Q15arithmetic q;
    int _bufferLen;
    int* _buffer;
    long _previousBuffFFTSum;
    long currBuffFFTSum;
    long threshMultByBuff;
    int levelEstimate;
    int b0_r, a1_r, b0_a, a1_a;
    long fs;
    
    //int tauRelease, tauAttack;
    //long fs;

};

#endif /* Onset_h */
