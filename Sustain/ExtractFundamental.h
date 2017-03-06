//Header file for ExtractFundamental.cpp

#ifndef ___ExtractFundamental__
#define ___ExtractFundamental__

#include<stdio.h>

class ExtractFundamental{
	public:
		ExtractFundamental(int bufLength,long Fs);
		~ExtractFundamental();
		int yin_pitch(int* buffer);
		int* get_fundamental();
		
	private:
		int* input;
		int buf_len;
		int pitch_period;
		long fs;	
};


#endif
