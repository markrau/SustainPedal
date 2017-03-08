

#include <math.h>
#include "LoopAudio.h"


#define DEFAULT_BUFFER_SIZE 1024


/*@brief	Extracts fundamental and loops it in consecutive output buffers		
* @param 	s1,s2,periodLength					Period start, end, length
* @param	buf_len 							Length of buffer incoming buffer
* @param	thresh								threshold to measure similar values in buffer
* @param	val, prevVal						last and second last samples in input buffer
* @param	buffPosition, prevBuffPosition		tracks position of samples for phase alignment
*/

LoopAudio::LoopAudio(int bufferLen,int th)
	{
	if (buf_len < 1) {
        buf_len = DEFAULT_BUFFER_SIZE;
    } else {
        buf_len = bufferLen;
    }	
	s1 = 0;
	s2 = 0;
	val = -1;
	prevVal = -1;
	thresh = th;
	buffPosition = 0;
	prevBuffPosition = 0;

}
	
LoopAudio::~LoopAudio(){}


//function to extract one period from input buffer
int LoopAudio::getPitchPeriod(int *curInBuf){
	val = curInBuf[buf_len-1];
	prevVal = curInBuf[buf_len-2];
	
	//val is the last element of input buffer
	//prevVal is the second last element of input buffer
	for(int i = 1; i < buf_len; i++){
		if(val -  thresh < curInBuf[i] && curInBuf[i] < thresh + val &&
		prevVal -  thresh < curInBuf[i-1] && curInBuf[i-1] < thresh + prevVal){
			s1 = i+1;
			break;
		}
	}
	
	for(int i = s1; i < buf_len; i++){
		if(val -  thresh < curInBuf[i] && curInBuf[i] < thresh + val &&
		prevVal -  thresh < curInBuf[i-1] && curInBuf[i-1] < thresh + prevVal){
			s2 = i+1;
			break;
		}
	}
	
	//periodLength = s2-s1;
        //return periodLength;
        return (s2-s1);
	
}

void LoopAudio::loopBuffer(int *curInBuf, int *curOutBuf, int periodLength){
	
	//loop it in output buffer
	for(int i = 0; i < buf_len; i++){
		buffPosition = (i+prevBuffPosition)%(periodLength);
		curOutBuf[i] = curInBuf[s1+buffPosition];
	}
	prevBuffPosition = buffPosition;
			
}
	

