//
//  Onset.h
//
//
//  Created by Terry Kong on 2/12/16.
//
//

#ifndef Onset_h
#define Onset_h

// Helpful Definitions
#define MAX_BUFFER_SIZE 2048

#include "dsplib.h"



class Onset {
public:
    //Onset(int bufferLen);
    Onset(int bufferLen, long previousBuffFFTSum);
    ~Onset();
    int isOnset(int* input, int thresh);

    
    
private:
    int _bufferLen;
    int* _buffer;
    long _previousBuffFFTSum;

};

#endif /* Onset_h */
