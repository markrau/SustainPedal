/** \mainpage Process Gain demo

  Demonstrates a simple audio processing implementation.
  
  Reads data from codec audio IN, scales the data by a Gain parameter (< 1) and sends the
  output to the codec OUT which can be listened on headphone.
  
  The Gain parameter is hardcoded in the code.
*/

// Include audio library
#include <Audio.h>
#include <dsplib.h>
#include <OLED.h>
#include "Onset.h"
#include "ExtractFundamental.h"

//================================

#define MAX_INT16           32767
#define MIN_INT16           -32768
#define MAX_INT32           2147483647
#define MIN_INT32           -2147483648

//=================================

// Globals
const int  BufferLength = 1024;   ///< Buffer length
const int maxDataLength = 2048;
const long Gain         = 32768; ///< Gain parameter



// Gloal Variables for Onset
long previousBuffFFTSum = MAX_INT16*BufferLength;        // Pervious fft buffer sum. Make the initial (negative time) buffer have the max FFT sum so no onset is detected
int onsetFlag = 0;                                       // Keep track of whether or not a buffer has an onset
int onsetThresh = MAX_INT16/4 ;                          // Onset detection threshold. Initialize to 4 (Q11), meaning that an onset is detected if a new buffer has 4 times the spectral energy as the previous buffer



// Declare Onset object =========================================
//===============================================================
Onset onset(BufferLength,previousBuffFFTSum);
//===============================================================

// Declare ExtractFundamental oject =============================
//===============================================================
ExtractFundamental extract;
//===============================================================






/** Setup function

  The setup function runs once at the begining of the program.
  The setup function is part of the Energia framework.

  Tasks:
  - Allocate memory for input/output arrays.
  - Initialize OLED display and Audio module.

*/
void setup()
{
    int status;
    int index;
    
    // Set pin as output
    pinMode(LED0, OUTPUT);
    pinMode(LED2, OUTPUT);
    
    
    // Turn LED0 on
    digitalWrite(LED0, HIGH);

    //Initialize OLED module for status display	
    disp.oledInit();
    disp.clear();
    disp.flip();    
    disp.setline(1);
    
    // Audio library is configured for non-loopback mode and the specified buffer length for the ADC and DAC, respectively.
    status = AudioC.Audio(TRUE, BufferLength, BufferLength);
    
    // Set codec sampling rate.  Valid sampling rates:
    //   SAMPLING_RATE_8_KHZ
    //   SAMPLING_RATE_12_KHZ
    //   SAMPLING_RATE_16_KHZ
    //   SAMPLING_RATE_24_KHZ
    //   SAMPLING_RATE_32_KHZ
    //   SAMPLING_RATE_44_KHZ
    //   SAMPLING_RATE_48_KHZ (default)
    AudioC.setSamplingRate(SAMPLING_RATE_48_KHZ);
    
    if (status == 0)
    {
        disp.print("Process ON");
    }        
}

/** Main application loop

  The loop function runs repetitively and immediatelly after calling the setup() function.
  The loop function is part of the Energia framework.

*/
void loop()
{
    // An LED flashes to show if an onset occured. 
    if(onsetFlag){
          digitalWrite(LED2, HIGH);
    }
    else{
        digitalWrite(LED2, LOW);
    }
}

/** Audio callback function
 
  You must define this function to use the Audio.h library.
  The processAudio() function is declared as an extern function inside the Audio.cpp
  file of the Audio library.
  
  The Audio library creates an instance of the AudioClass, named AudioC, that is accesible
  by processAudio.  The AudioClass in turns defines arrays for the input and output buffers.
  The buffers are of type int (16 bits in the C55x family of processors).
  
  AudioC.inputLeft   = Left input channel buffer
  AudioC.inputRight  = Right input channel buffer
  AudioC.outputLeft  = Left output channel buffer
  AudioC.outputRight = Right output channel buffer
  
  The processAudio() fucntion is called by the interrupt service routine (ISR)
  once a full block of data is received from the audio codec digital interface and available in
  the input buffers.
  After the audioProcess() function executes to completion, data on the output buffers is sent
  to the codec audio output.
*/
void processAudio()
{
  
    onsetFlag = onset.isOnset(AudioC.inputLeft, onsetThresh);
  
  
    for(int n = 0; n < BufferLength; n++)
    {
        AudioC.outputLeft[n]  = (Gain * AudioC.inputLeft[n])  >> 15;
        AudioC.outputRight[n] = (Gain * AudioC.inputRight[n]) >> 15;
    }
}
