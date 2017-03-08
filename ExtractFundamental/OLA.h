
#include "Q15arithmetic.h"
#include <math.h>

#ifndef ___OLA__
#define ___OLA__
class OLA{
	public:
		OLA(long fs, int buf_len, int overlap, int *win);
		~OLA();
		void overlap_add(int *curInBuf, int *curOutBuf);
		
	private:
	    Q15arithmetic q;
		int* window;
		int* win_mult;
		int buf_len;
		int ola;
		int hopSize;
		long fs;
		int s1;
		int s2;
};


#endif