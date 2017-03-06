#include "ExtractFundamental.h"
#include <stdio.h>
//#include <iostream>
#include <math.h>
using namespace std;

#define DEFAULT_BUFFER_SIZE 1024
#define FIXED_BITS        16
#define FIXED_WBITS       0
#define FIXED_FBITS       15
#define Q15_RESOLUTION   (1 << (FIXED_FBITS - 1))
#define MAX_INT16         32767

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
* @param 	input			Pointer to array of Q15 audio buffer
* @param	buf_len 		Length of buffer incoming buffer
* @param	fs	                Sampling rate
* @param	pitch_period	        Number of samples in pitch_period of input
* @param	output  		Exactly one pitch pitch_period extracted from input and looped
*/


//Constructor
ExtractFundamental::ExtractFundamental(int bufLength, long Fs){
	if(bufLength < 1)
		buf_len = DEFAULT_BUFFER_SIZE;
	else
		buf_len = bufLength;
	pitch_period = 0;
	fs = Fs;
}

//destructor
ExtractFundamental::~ExtractFundamental(){
}

//simple pitch detection from autocorrelation function
int ExtractFundamental::acorr_pitch(int *buffer, int thresh){
  int* xcorr = new int[buf_len];
  int oflag = acorr((DATA *)buffer, (DATA *)xcorr, buf_len, buf_len, raw);
  int ref = xcorr[0];
  for(int i = 1; i < buf_len; i++){
    if(xcorr[i] > ref - thresh && xcorr[i] < ref + thresh)
      return i;
  }
  return 0;
}
  

//simple pitch detection from FFT
int ExtractFundamental::fft_pitch(int *buffer){
  
  rfft((DATA *)buffer, buf_len, SCALE);
  //find magnitude from real and imaginary parts
  long mag;
  //find index corresponding to maximum amplitude
  long maximum = 0; int max_pos = -1;
  for(int i = 2; i < buf_len/2; i+=2){
    mag = (buffer[i]*buffer[i])+(buffer[i+1]*buffer[i+1]);
    //make sure DC and nyquist are not included in calculating fundamental
    if(i > 0 && mag > maximum){
      maximum = mag;
      max_pos = i;
    }
  }
  
  //parabolic interpolation to get more accurate estimate
  float peak = 0;
  if(max_pos > 0 && max_pos < buf_len-2){
      long prevMax = sqrt((buffer[max_pos-1]*buffer[max_pos-1])+(buffer[max_pos]*buffer[max_pos]));
      long nextMax = sqrt((buffer[max_pos+1]*buffer[max_pos+1])+(buffer[max_pos+2]*buffer[max_pos+2]));
      peak = ((prevMax-nextMax)/(prevMax - 2*maximum + nextMax)) * 0.5;
  }
  
  max_pos = max_pos + (int)peak;
  int pfreq = (int)((fs * (long)max_pos) >> 10);
  return (pfreq); 
}
   
//pitch estimation with harmonic product spectrum
int ExtractFundamental::hps_pitch(int *buffer, int nharmonics){
  
  rfft((DATA *)buffer, buf_len, SCALE);
  long long *mag = new long long[buf_len >> 1];
  //find magnitude of FFT
  int k = 0;
  for(int i = 2; i < buf_len/2; i+=2){
    mag[k++] = (buffer[i]*buffer[i])+(buffer[i+1]*buffer[i+1]);
  }
  int minIndex = 25;
  int maxIndex = (buf_len >>1)/nharmonics;
  
  int maxLocation = minIndex;
  for(int i = minIndex; i <= maxIndex; i++){
    for(int k = 1; k <= nharmonics; k++){
      mag[i] *= mag[k*i];
    }
  if(mag[i] > mag[maxLocation])
    maxLocation = i;
  }
   // Correct for octave too high errors.  If the maximum subharmonic
   // of the measured maximum is approximately 1/2 of the maximum
   // measured frequency, AND if the ratio of the sub-harmonic
   // to the total maximum is greater than 0.2, THEN the pitch value
   // is assigned to the subharmonic.

   int max2 = minIndex;
   int maxsearch = (maxLocation * 3) >> 2;
   for (int i=minIndex+1; i<maxsearch; i++) {
      if (mag[i] > mag[max2]) {
         max2 = i;
      }
   }
   if (abs(max2 * 2 - maxLocation) < 4) {
      if (mag[max2] > ((6554*mag[maxLocation]) >> 15)) {
         maxLocation = max2;
      }
   }  
   return (int)((fs * (long)maxLocation) >> 10);
} 
 
//Yin estimator to estimate pitch of incoming buffer
int ExtractFundamental::yin_pitch(int *input){
  	
	//normalise incoming input
	/*float* norm_input = new float[buf_len];
	
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
	float cumsum = 0;
	
	for(int tau = 1; tau < buf_len; tau++){
		cumsum += diff[tau];
		d_norm[tau] = ((float)tau * diff[tau])/cumsum;
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
		int min = 1000;
		for(int i =0; i < buf_len; i++){
			if(d_norm[i] < min){
				min = d_norm[i];
				lag = i;
			}
		}
	}
	//cout << "Lag : " << lag << endl;
	
	//Step 4 - parabolic interpolation
	float peak = 0;
	if(lag > 0 && lag < buf_len-1){
		float alpha = d_norm[lag-1];
		float beta = d_norm[lag];
		float gamma = d_norm[lag+1];
		peak = 0.5*(alpha-gamma)/(alpha - 2*beta + gamma);
	}
	
	//cout << "Peak : " << peak << endl;
	pitch_period = (int)(round((float)lag + peak));
	return pitch_period; */
	
	
	//trying a fixed point implementation
	int* diff =  new int[buf_len];
	
	//Step 1 - calculate difference function
	int sub = 0;
	long long temp = 0;
	
	for (int tau = 0; tau < buf_len; tau++){
		temp = 0;
		for(int j = 0; j < tau; j++){
			//MAC - Multiply and accumulate
			sub = abs(input[j] - input[j+tau]);
			temp += (long)sub * (long)sub;
		}
		//round off
		temp = (temp + Q15_RESOLUTION) >> FIXED_FBITS;
		//saturate if value is too large
		if(temp > MAX_INT16){
			temp = MAX_INT16;
		}
		diff[tau] = (int)temp;
	}
	
	//Step 2 - cumulative mean normalised difference function
	int *d_norm = new int[buf_len];
	d_norm[0] = 1;
	long cumsum = 1;;
	long mult = 0;
	
	for(int tau = 1; tau < buf_len; tau++){
		cumsum += diff[tau];
		//Q15 division  
		mult = ((long)diff[tau] * (long)tau);
		temp = (mult << FIXED_FBITS)/cumsum;
		d_norm[tau] = (int)temp;
		
	}
	
	//Step 3 - absolute thresholding
	int th = 3277;
	int lag = -1;
	for(int i =0; i < buf_len; i++){
		if(d_norm[i] < th){
			lag = i;
			break;
		}
	}
	//if no value lies below threshold, take minimum index of d_norm as lag
	if(lag < 0){
		int min = MAX_INT16;
		for(int i =0; i < buf_len; i++){
			if(d_norm[i] < min){
				min = d_norm[i];
				lag = i;
			}
		}
        }
        
	//Step 4 - parabolic interpolation
	float peak = 0;
	if(lag > 0 && lag < buf_len-1){
		int alpha = d_norm[lag-1];
		int beta = d_norm[lag];
		int gamma = d_norm[lag+1];
		peak = ((alpha-gamma)/(alpha - 2*beta + gamma)) * 0.5;
	}
	
	pitch_period = round(lag + (int)peak);
	return pitch_period;
}



/*this function uses the pitch information to extract exactly one pitch 
period from input signal and loops it into the output buffer till it has
same number of samples as input signal */

int* ExtractFundamental::get_fundamental(int *input){
	
	int *one_period = new int[pitch_period];
	
	//find position of peak in fundamental
	int peakpos = -1;
	int peakval = -MAX_INT16;
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
