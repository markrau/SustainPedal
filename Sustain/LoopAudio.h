
#include <math.h>

#ifndef ___LoopAudio__
#define ___LoopAudio__
class LoopAudio{
	public:
		LoopAudio(int buf_len,int th);
		~LoopAudio();
		int getPitchPeriod(int *curInBuf);
		void loopBuffer(int *curInBuf, int *curOutBuf, int periodLength);
		
	private:
		int buf_len;
		int s1;
		int s2;
		//int periodLength;
		int val;
		int prevVal;
		int thresh;
		int buffPosition;
		int prevBuffPosition;
};


#endif
