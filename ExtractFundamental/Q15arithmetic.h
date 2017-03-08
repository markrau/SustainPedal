//Q15 helper class

#define DEFAULT_BUFFER_SIZE 1024
#define FIXED_BITS        16
#define FIXED_WBITS       0
#define FIXED_FBITS       15
#define Q15_RESOLUTION   (1 << (FIXED_FBITS - 1))
#define MAX_INT16   	  32767
#define MAX_INT32		  2147483647

#ifndef ___Q15arithmetic__
#define ___Q15arithmetic__

class Q15arithmetic{

public:
	
// Some helper functions ==================
//constructor and destructor
Q15arithmetic(){}
~Q15arithmetic(){}

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
};

#endif