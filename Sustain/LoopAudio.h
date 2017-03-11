
#include <math.h>

#ifndef ___LoopAudio__
#define ___LoopAudio__

class LoopAudio{
	public:
		LoopAudio(int buf_len,int th);
		~LoopAudio();
		int getPitchPeriod(int *curInBuf);
		void loopBuffer(int *curInBuf, int *curOutBuf, int period);
		int getPitchRobust(int *curInBuf);
		
	private:
		int buf_len;
		int s1;
		int val;
		int prevVal;
		int thresh;
		int buffPosition;
		int prevBuffPosition;
		//required parameters for robust pitch detection
		int minDistance;
		int diffthresh;
		int *candidatePeakPos;
		int *diffPos;
		int *possiblePeriod;
};


#endif