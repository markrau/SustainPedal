

#include <math.h>
#include "LoopAudio.h"
#include <stdio.h>
#include <stdlib.h>


#define DEFAULT_BUFFER_SIZE 1024
#define MAX_INT16   	    32767

#define MAX_LONG32          2147483647



//=================================================================================================================================
/*@brief	Extracts fundamental and loops it in consecutive output buffers		
* @param 	minDistance					Minimum distance between two consecutive maxima
* @param	buf_len 					Length of buffer incoming buffer
* @param	diffthresh					Threshold to measure similarity between differences in positions of candidate peaks
* @param	candidatePeakPos                                Positions of candidate peaks in buffer
* @param        diffPos                                         Differences between consecutive positions of candidate peaks
* @param        possiblePeriod                                  List of possible pitch candidates					
* @param	buffPosition, prevBuffPosition		        Tracks position of samples for phase alignment
*/
//==================================================================================================================================

LoopAudio::LoopAudio(int bufferLen)
{
	if (buf_len < 1) {
          buf_len = DEFAULT_BUFFER_SIZE;
        } 
        else {
          buf_len = bufferLen;
        }	
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

	//initialising parameters for AMDF pitch detection
	//minimum lag = fs/1500
	tau_min = 32;
	//maximum lag = fs/80;
	tau_max = 500;
	//AMDF array
	D = new long[tau_max - tau_min + 1];
        //threshold for detecting minimum
        thresh = 600; //equivalent to decimal 0.01

}
	
LoopAudio::~LoopAudio(){
	delete [] candidatePeakPos;
	delete [] diffPos;
	delete [] possiblePeriod;
	delete [] D;
}

//function to find pitch period - calculations should work on fixed point
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


//function to implement AMDF pitch detection
int LoopAudio::getPitchAMDF(int *curInBuf){

	//generate AMDF function
	for(int i = tau_min; i <= tau_max; i++){
		for(int j = i; j < buf_len; j++){
			D[i - tau_min] += (long)(abs(curInBuf[j] - curInBuf[j-i]));
		}
		//divide by buflen, since it's a power of 2 we can just do bitshift
		D[i - tau_min] = D[i - tau_min] >> 10;
	}

	//find dip in AMDF by computing the minimum

	long minimum = MAX_LONG32; 
	float minPos = -1.0;
        //int minPos = -1;
        //find global minimum
	/*for(int i = 0; i <= tau_max - tau_min; i++){
		if(D[i] < minimum){
		        minPos = i;
			minimum = D[i];
		}
	}*/
         
         
        //find first local minimum below threshold (first dip in AMDF)
        for (int i = 1; i < tau_max - tau_min;i++){
                if(D[i-1] > D[i] && D[i+1] > D[i] && D[i] <= thresh){
                //if first local minumum is detected, do parabolic interpolation
                        minPos = ((D[i-1] - D[i+1])/(D[i-1] - (D[i]<<1) + D[i+1])) >> 1;
                        minPos = (float)i;
                        minimum = D[i];
                        break;
                 }
        }
        

	//now compute number of samples in pitch period
	int periodLength = 0;
	if(minPos > -1.0){
                //periodLength = minPos + tau_min;
		periodLength = (int)minPos + tau_min;
	}

	
	//set prevBuffPosition to zero here to avoid hearing discontinuities between
	//initial attack and looped signal
          prevBuffPosition = 0;
	return periodLength;
	

}



//function to loop audio with phase alignment
void LoopAudio::loopBuffer(int *curInBuf, int *curOutBuf, int periodLength){
	
	//loop it in output buffer
	for(int i = 0; i < buf_len; i++){
		buffPosition = (i+prevBuffPosition)%(periodLength);
		curOutBuf[i] = curInBuf[buffPosition];
	}
	prevBuffPosition = buffPosition;
			
}
	

