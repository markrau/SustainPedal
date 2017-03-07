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
<<<<<<< HEAD
                int acorr_pitch(int *buffer, int* xcorr, int thresh);
=======
//<<<<<<< HEAD
//		int yin_pitch(int* buffer);
//		int* get_fundamental();
//=======
                int acorr_pitch(int *buffer, int thresh);
>>>>>>> 5293257a6390632d22f7f42ddbca00fedf69864b
                int fft_pitch(int *buffer);
                int hps_pitch(int *buffer, long* mag, int nharmonics);
		int yin_pitch(int *buffer, int* diff, int* d_norm);
		void get_fundamental(int *input, int* one_period, int* output);

                //to communicate with MATLAB 
                SerialCmd cmd;
                
//>>>>>>> 4523fbfe8dfe930b45a1e7ffa38adced65e6a6ea
		
	private:
		int buf_len;
		int pitch_period;
		long fs;	
};


#endif
