

#include <math.h>
#include "LoopAudio.h"
#include <stdio.h>
#include <stdlib.h>
//using namespace std;

#define DEFAULT_BUFFER_SIZE 1024
#define MAX_INT16   	    32767


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
	val = -1;
	prevVal = -1;
	thresh = th;
	buffPosition = 0;
	prevBuffPosition = 0;
	//maximum frequency of guitar = 1500
	//sampling rate - 48000
	//minimum distance between peaks = 48000/1500 = 32
	//some tolerance allowed
	minDistance = 30;
	diffthresh = 5;
	candidatePeakPos = new int[32];
	diffPos = new int[32];
	possiblePeriod = new int[32];

}
	
LoopAudio::~LoopAudio(){
	delete [] candidatePeakPos;
	delete [] diffPos;
	delete [] possiblePeriod;
}

//function to find pitch period - no calculations should work on fixed point
int LoopAudio::getPitchRobust(int *curInBuf){
	
	//1. find maximum peak and its position
	int M0 = 0, D0 = -1;
	for(int i = 0; i < buf_len; i++){
		if(curInBuf[i] > M0){
			M0 = curInBuf[i];
			D0 = i;
		}
	}
	
	//2. find candidate peaks and their positions
	int k = 0;
	for(int i = 1 ; i < buf_len-1; i++){
		//1. check if local maxima (of positive part of signal only)
		if(curInBuf[i] > 0 && curInBuf[i] >= curInBuf[i-1] && curInBuf[i] > curInBuf[i+1]){
			//2. check if it is within a threshold of maximum peak
			//3. must be minDistance away from last detected candidatePeak
			if(curInBuf[i] >= (M0 >> 1) && (k == 0 || 
			(i - candidatePeakPos[k-1] >= minDistance))){
				candidatePeakPos[k] = i;
				k++;
			}
		}
	}
	
	int numCandidates = k;
		


	
	//3. find differences in positions of the array(which is already sorted)

	for(int i = 0; i < numCandidates-1; i++){
		diffPos[i] = candidatePeakPos[i+1] - candidatePeakPos[i];

	}
	
	
	//4. we see that the difference array has a specific pattern - exploit that pattern to get possible periods
	k = 0;
	for(int i = 0; i < numCandidates-2;i++){
		for(int j = i+1; j < numCandidates-1;j++){
			if(abs(diffPos[i] - diffPos[j]) <= diffthresh){
				possiblePeriod[k++] = candidatePeakPos[j] - candidatePeakPos[i];
				break;
			}
		}
	}
				

	
	int periodLength = 0;
	//5. final period is average of possible periods
	for(int i =0; i < k;i++){
		periodLength += possiblePeriod[i];
	}
	
	periodLength /= k;


	return periodLength;
			
}

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
	
	int periodLength = getPitchRobust(curInBuf);
        prevBuffPosition = 0;
	return periodLength;
	
}

//function to loop audio with phase alignment
void LoopAudio::loopBuffer(int *curInBuf, int *curOutBuf, int periodLength){
	
	//loop it in output buffer
	for(int i = 0; i < buf_len; i++){
		buffPosition = (i+prevBuffPosition)%(periodLength);
		curOutBuf[i] = curInBuf[s1+buffPosition];
	}
	prevBuffPosition = buffPosition;
			
}
	

