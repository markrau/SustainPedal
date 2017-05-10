//
//  Onset.cpp
//  
//
//  Created by Mark Rau on 3/1/17.
//
//

#include "Onset.h"
#include <math.h>      
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
Onset::Onset(int bufferLen, long previousBuffFFTSum, long FS) {
    if (bufferLen < 1) {
        _bufferLen = DEFAULT_BUFFER_SIZE;
    } else {
        _bufferLen = bufferLen;
    }
    _buffer = new int[bufferLen];
    _previousBuffFFTSum = previousBuffFFTSum;
    levelEstimate = 5000;    //Initialize this to 5000(5000/2^15) so you don't always get an onset at the beginning of the audio
    fs = FS;
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


int Onset::isOnsetFFT(int* input, int thresh){
    
    for (int i = 0; i < _bufferLen; i++) {
        _buffer[i] = input[i];
    }
    
    currBuffFFTSum = 0;
    rfft((DATA *)_buffer, _bufferLen, SCALE);
    for(int i =0; i<_bufferLen; i=i+2){
        // ignored the square root to save computations
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


int Onset::isOnsetLeaky(int* input){
    int foundOnset = 0;
    for (int i = 0; i < _bufferLen; i++) {
        //_buffer[i] = input[i];
        
        if ( abs( input[i] ) > levelEstimate ){
            levelEstimate += q.Q15mult(b0_a , ( abs( input[i] ) - levelEstimate ));
            foundOnset = 1;
        }
        else{
            levelEstimate += q.Q15mult(b0_r , ( abs( input[i] ) - levelEstimate ));
        }
    }
    return foundOnset;
}






void Onset::setTauRelease(int tauRelease) {
    
    //a1_r = exp( -1.0 / ( tauRelease * fs ) );
    a1_r = 32760;
    b0_r = MAX_INT16 - a1_r;
}

void Onset::setTauAttack(int tauAttack) {
    //a1_a = exp( -1.0 / ( tauAttack * fs ) );
    a1_a = 32693;
    b0_a = 1 - a1_a;
}





