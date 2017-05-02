//
//  LoopAudio.h
//  
//
//  Created by Orchi Das on 3/10/17.
//
//

#include <math.h>

#ifndef ___LoopAudio__
#define ___LoopAudio__

class LoopAudio{
	public:
		LoopAudio(int buf_len);
		~LoopAudio();
		void loopBuffer(int *curInBuf, int *curOutBuf, int period);
		int getPitchRobust(int *curInBuf);
		int getPitchAMDF(int *cufInBuf);
		
	private:
		int buf_len;
		int buffPosition;
		int prevBuffPosition;
		//required parameters for robust pitch detection
		int minDistance;
		int diffthresh;
		int *candidatePeakPos;
		int *diffPos;
		int *possiblePeriod;
		//required parameters for AMDF pitch detection
		int tau_min;
		int tau_max;
		long *D;
                long thresh;
};


#endif
