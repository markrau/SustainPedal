//Header file for ExtractFundamental.cpp

#ifndef ___ExtractFundamental__
#define ___ExtractFundamental__

#include<stdio.h>
#include<OLED.h>
#include <serial_array.h>
#include "dsplib.h"

class ExtractFundamental{
	public:
		ExtractFundamental(int bufLength,long Fs);
		~ExtractFundamental();
                int acorr_pitch(int *buffer, int* xcorr, int thresh);
                int fft_pitch(int *buffer);
                int hps_pitch(int *buffer, long* mag, int nharmonics);
		int yin_pitch(int *buffer, int* diff, int* d_norm);
		void get_fundamental(int *input, int* one_period, int* output);

                //to communicate with MATLAB 
                SerialCmd cmd;
                
		
	private:
		int buf_len;
		int pitch_period;
		long fs;	
};


#endif
