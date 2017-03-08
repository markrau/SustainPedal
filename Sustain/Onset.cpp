//
//  Onset.cpp
//  
//
//  Created by Mark Rau on 3/1/17.
//
//

#include "Onset.h"
#include <math.h>      // std::complex, std::abs
#include <OLED.h>





#define DEFAULT_BUFFER_SIZE 1024





/** ==============================================================================
 * @brief       Function initializes onset detection module.
 *
 * @details     The onset detection module is designed for Q15 data.
 *
 * @param       bufferLen       Number of data the module expects when pitchCorrect() is called in order to optimize performance.

 * ================================================================================
 */
Onset::Onset(int bufferLen, long previousBuffFFTSum) {
    if (bufferLen < 1) {
        _bufferLen = DEFAULT_BUFFER_SIZE;
    } else {
        _bufferLen = bufferLen;
    }
    _buffer = new int[bufferLen];
    _previousBuffFFTSum = previousBuffFFTSum;
    
    //currBuffFFTSum = 0;
}

/** Standard destructor */
Onset::~Onset() {
    delete[] _buffer;
}


/** ====================================================
 * @brief       Checks if buffer is onsert
 *
 * @details     Takes FFT of cuffent buffer and takes a sum of the spectral magnitude. This sum is compared to that of the previous buffer. If the ratio is greater than a threshold value then an onset is detected.
 *
 * @param       input                   Pointer to array of Q15 data (bufferLen long)
 * @param       thresh                  onset threshold
 * @param       previousBuffFFTsum      sum of FFT magnitude values from previous buffer
 *
 * @todo        Maybe check this across multiple bandwidths. Maybe also use a window.
 *
 * ======================================================
 */


int Onset::isOnset(int* input, int thresh){
    
    for (int i = 0; i < _bufferLen; i++) {
        _buffer[i] = input[i];
    }
    
    currBuffFFTSum = 0;
    rfft((DATA *)_buffer, _bufferLen, SCALE);
    for(int i =0; i<_bufferLen; i=i+2){
        // ignored the square root, hopefully it isn't a problem
        currBuffFFTSum = currBuffFFTSum + ((long)(q.Q15mult(_buffer[i],_buffer[i])+q.Q15mult(_buffer[i+1],_buffer[i+1])) << 15);
    }
    // Need to shift by <<4 at the end to make up for thresh being a Q11 number
    threshMultByBuff = ((long)thresh*(_previousBuffFFTSum >> 15) <<4);
    if(currBuffFFTSum> threshMultByBuff ){
        _previousBuffFFTSum =currBuffFFTSum;
        return 1;
    }
    else{
        _previousBuffFFTSum =currBuffFFTSum;
        return 0;
    }

    
    
}
long Onset::returnCurrFFT(){
  return currBuffFFTSum;
  //return _buffer[10];
}
long Onset::returnPrevFFT(){
  return threshMultByBuff;
  //return _previousBuffFFTSum;
}





