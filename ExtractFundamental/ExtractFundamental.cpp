#include "ExtractFundamental.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
using namespace std;

#define DEFAULT_BUFFER_SIZE 1024
#define FIXED_BITS        16
#define FIXED_WBITS       0
#define FIXED_FBITS       15
#define Q15_RESOLUTION   (1 << (FIXED_FBITS - 1))
#define LARGEST_Q15_NUM   32767

// Some helper functions ==================

// Q15 multiplication
int Q15mult(int x, int y) {
    long temp = (long)x * (long)y;
    temp += Q15_RESOLUTION;
    return temp >> FIXED_FBITS;
}

// Q15 wrapped addition
int Q15addWrap(int x, int y) {
    return x + y;
}

// Q15 saturation addition
int Q15addSat(int x, int y) {
    long temp = (long)x+(long)y;
    if (temp > 0x7FFFFFFF) temp = 0x7FFFFFFF;
    if (temp < -1*0x7FFFFFFF) temp = -1*0x7FFFFFFF;
    return (int)temp;
}


/*@brief	Estimates fundamental pitch_period and extracts exactly one pitch_period of waveform
* 			and loops it in output buffer
* @param 	input	Pointer to array of Q15 audio buffer
* @param	buf_len Length of buffer incoming buffer
* @param	fs		Sampling rate
* @param	pitch_period	Number of samples in pitch_period of input
* @param	output  Exactly one pitch pitch_period extracted from input and looped
*/

float cumsum(float*,int);

//Constructor
ExtractFundamental::ExtractFundamental(int* buffer, int bufLength, long Fs){
	if(bufLength < 1)
		buf_len = DEFAULT_BUFFER_SIZE;
	else
		buf_len = bufLength;
	input = new int[buf_len];
	//copy buffer to input
	for(int i = 0; i < buf_len; i++){
		input[i] = buffer[i];
	}
	pitch_period = 0;
	fs = Fs;
}

//destructor
ExtractFundamental::~ExtractFundamental(){
	delete[] input;
}

//Yin estimator to estimate pitch of incoming buffer

int ExtractFundamental::yin_pitch(){
	
	//normalise incoming input
	float* norm_input = new float[buf_len];
	
	for(int i=0;i<buf_len;i++){
		norm_input[i] = (float)input[i]/pow(2.0,15.0);
	}
	
	float* diff =  new float[buf_len];
	for(int i = 0; i < buf_len; i++)
		diff[i] = 0;
	
	//Step 1 -calculate difference function
	for (int tau = 0; tau < buf_len; tau++){
		for(int j = 0; j < tau; j++){
			diff[tau] += pow((norm_input[j] - norm_input[j+tau]),2.0);
		}
	}
	
	//Step 2 - cumulative mean normalised difference function
	float *d_norm = new float[buf_len];
	d_norm[0] = 1.0;
	
	for(int tau = 1; tau < buf_len; tau++){
		d_norm[tau] = (float)diff[tau]/((1/(float)tau) * cumsum(diff,tau));
	}
	
	//Step 3 - absolute thresholding
	float th = 0.1;
	int lag = -1;
	for(int i =0; i < buf_len; i++){
		if(d_norm[i] < th){
			lag = i;
			break;
		}
	}
	//if no value lies below threshold, take minimum index of d_norm as lag
	if(lag < 0){
		float min = 1000;
		for(int i =0; i < buf_len; i++){
			if(d_norm[i] < min){
				min = d_norm[i];
				lag = i;
			}
		}
	}
	cout << "Lag : " << lag << endl;
	
	//Step 4 - parabolic interpolation
	float peak = 0;
	if(lag > 0 && lag < buf_len-1){
		float alpha = d_norm[lag-1];
		float beta = d_norm[lag];
		float gamma = d_norm[lag+1];
		peak = 0.5*(alpha-gamma)/(alpha - 2*beta + gamma);
	}
	
	cout << "Peak : " << peak << endl;
	pitch_period = (int)(round((float)lag + peak));
	return pitch_period;
}


float cumsum(float *d, int tau){
	float sum = 0;
	for(int i = 0; i < tau; i++){
		sum += d[i];
	}
	return sum;
}

/*this function uses the pitch information to extract exactly one pitch 
period from input signal and loops it into the output buffer till it has
same number of samples as input signal */

int* ExtractFundamental::get_fundamental(){
	
	int *one_period = new int[pitch_period];
	
	//find position of peak in fundamental
	int peakpos = -1;
	int peakval = -LARGEST_Q15_NUM;
	for(int i= 0; i < pitch_period; i++){
		if(input[i] > peakval){
			peakval = input[i];
			peakpos = i;
		}
	}
	
	//look for zero crossings preceding or following the pitch_period
	int zcr_pos = -1;
	//it is possible that zero crossing precedes peakpos
	for(int i = peakpos; i > 0; i--){
		if(input[i] > 0 && input[i-1] <= 0){
			zcr_pos = i-1;
			break;
		}
	}
	
	//it is also possible that zero crossing follows peakpos
	if(zcr_pos == -1){
		for(int i = peakpos+1; i<buf_len; i++){
			if(input[i] <= 0 && input[i-1] >0){
				zcr_pos = i;
				break;
			}
		}
	}
	
	//extract exactly one pitch_period starting from zcr_pos
	for(int i = zcr_pos; i < zcr_pos+pitch_period; i++){
		one_period[i-zcr_pos] = input[i];
	}
	
	//loop extracted period into output buffer
	int *output = new int[buf_len]; 
	int k = 0;
	for(int i = 0; i < buf_len; i++){
		output[i] = one_period[k];
		k++;
		//if period is exhausted, go back to beginning
		if(k == pitch_period){
			k = 0;
		}
	}
	
	return output;
}
