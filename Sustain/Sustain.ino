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
#include "serial_array.h"
#include "Onset.h"
#include "ExtractFundamental.h"
//#include "FLWT.h"

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
const long baudRate      = 115200;
//const int maxBuffsAfterOnset =5;                   // number of buffers to wait before looking for new onset after one is detected
const int maxBuffUntilSteadyState = 15;          //  number of buffers after onset until it is declared as steady state


/// Create instance of the serial command class
SerialCmd cmd(BufferLength);



// Gloal Variables for Onset
long previousBuffFFTSum = MAX_INT16*BufferLength;        // Pervious fft buffer sum. Make the initial (negative time) buffer have the max FFT sum so no onset is detected
int onsetFlag = 0;   
int prevOnsetFlag = 0; // Keep track of whether or not a buffer has an onset
int onsetThresh = MAX_INT16*0.125 ;    // Onset detection threshold. Initialize to 4 (Q11), meaning that an onset is detected if a new buffer has 4 times the spectral energy as the previous buffer
//int numBuffsAfterOnset = 0;
int numBuffUntilSteadyState = 0;
int period = 0;
int pedalPressed = 1;           // variable to dictate if the effect is activated
int foundOnset = 0;                  //Keep track of if an onset was found
int foundSS = 0;                 // Keep track of weather steady state was hit
volatile int readyToProcess = 0;


int numMatlabCalls = 0;


volatile int GetAudioBufferFlag = 0; // Trigger audio buffer capture in processAudio()
volatile int CapturedBufferFlag = 0; // Indicate that audio buffer has been captured in processAudio()

#pragma DATA_ALIGN(32)
int InputLeft[BufferLength] = {0};
#pragma DATA_ALIGN(32)
int InputRight[BufferLength] = {0};
#pragma DATA_ALIGN(32)
int OutputLeft[BufferLength] = {0};
#pragma DATA_ALIGN(32)
int OutputRight[BufferLength] = {0};

#pragma DATA_ALIGN(32)
int AudioCaptureBufferLeft[BufferLength]  = {0};



// Declare Onset object =========================================
//===============================================================
Onset onset(BufferLength,previousBuffFFTSum);
//===============================================================

// Declare ExtractFundamental object =============================
//===============================================================
ExtractFundamental extract(BufferLength,SAMPLING_RATE_48_KHZ);
//===============================================================

//Declare FLWT pitch detection object
//================================================================
//FLWT flwt(10, BufferLength);
//================================================================

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
    digitalWrite(LED2, LOW);

    //Initialize OLED module for status display	
    disp.oledInit();
    disp.clear();
    disp.flip();    
    disp.setline(1);
    

    
    // Audio library is configured for non-loopback mode and the specified buffer length for the ADC and DAC, respectively.
    status = AudioC.Audio(TRUE, BufferLength, BufferLength);
    AudioC.setInputGain(0, 0);
    
    // Set codec sampling rate.  Valid sampling rates:
    //   SAMPLING_RATE_8_KHZ
    //   SAMPLING_RATE_12_KHZ
    //   SAMPLING_RATE_16_KHZ
    //   SAMPLING_RATE_24_KHZ
    //   SAMPLING_RATE_32_KHZ
    //   SAMPLING_RATE_44_KHZ
    //   SAMPLING_RATE_48_KHZ (default)
    AudioC.setSamplingRate(SAMPLING_RATE_48_KHZ);
    AudioC.setInputGain(40,40);
    
    if (status == 0)
    {
        disp.clear();
        disp.print("Process ON");
    }        
  
    
    /*InputLeft = new int[BufferLength];
    InputRight = new int[BufferLength];
    OutputLeft = new int[BufferLength];
    OutputRight = new int[BufferLength];
    for(int i = 0; i < BufferLength; i++){
      InputLeft[i] = 0;
      InputRight[i] = 0;
      OutputLeft[i] = 0;
      OutputRight[i] = 0;
    }*/
    
    // connect to matlab
    serial_connect(baudRate);
}

/** Main application loop

  The loop function runs repetitively and immediatelly after calling the setup() function.
  The loop function is part of the Energia framework.

*/
void loop()
{
  

  
      // An LED flashes to show if an onset occured. 
      // This probably won't work if we are also printing the pitch detection to the screen. DSP shield is too slow
    if(onsetFlag){
       digitalWrite(LED2, HIGH);
    }
    else{
        digitalWrite(LED2, LOW);
    }
    if(readyToProcess){
      //period = extract.hps_pitch(InputLeft, 4);
      disp.clear();
      disp.setline(0);
      disp.print((long)period);
    }
    

   if(numMatlabCalls <1){
  
      // Matlab connection
      int command;
        
      cmd.recv();        
      command = cmd.getCmd();
        
      disp.clear();
      disp.setline(0);
      disp.print("command: ");
      disp.print((long)command);
   
      switch(command)
      {
          case 0: // Send buffer to matla
          
              GetAudioBufferFlag = 1;
              CapturedBufferFlag = 0;
  
              disp.clear();
              disp.setline(0);
              disp.print("Cmd 0: send buffer ");
              
              // wait until the buffer is fully copied
              while(!CapturedBufferFlag);
                
              serial_send_array(AudioCaptureBufferLeft, BufferLength);
              
              digitalWrite(LED0, HIGH);
              break;
          default:
              disp.clear();
              disp.setline(0);
              disp.print("Unknown Command");
              break;
      }
    numMatlabCalls++;
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
  to the codec audio output. */

void processAudio()
{
  
  
    //prevOnsetFlag = onsetFlag;
    
    onsetFlag = onset.isOnset(AudioC.inputLeft, onsetThresh);
    
   if(onsetFlag){
      numBuffUntilSteadyState = 0;
      foundSS =0;
   }     
   numBuffUntilSteadyState++;

    
    
    //if previous buffer was onset and current buffer is steady state
     if(numBuffUntilSteadyState > maxBuffUntilSteadyState && pedalPressed && !foundSS){
        readyToProcess = 0;       
        foundSS = 1;
        for(int n = 0; n < BufferLength; n++)
        {
          InputLeft[n] = AudioC.inputLeft[n]; 
          InputRight[n] = AudioC.inputRight[n];
        }
           
        //period = extract.yin_pitch(InputLeft);
        period = extract.fft_pitch(InputLeft);
        //period = extract.hps_pitch(InputLeft,4);
        /*copyShortBuf(AudioC.inputLeft,OutputLeft, BufferLength);
        copyShortBuf(AudioC.inputRight,OutputRight, BufferLength);*/
        readyToProcess = 1;
     }
    

    if(foundSS && pedalPressed){
      for(int n = 0; n < BufferLength; n++) {
        AudioC.outputLeft[n]  = (Gain * InputLeft[n])  >> 15;
        AudioC.outputRight[n] = (Gain * InputRight[n]) >> 15;
      }
    }
    else{
      for(int n = 0; n < BufferLength; n++) {
          AudioC.outputLeft[n]  = (Gain * AudioC.inputLeft[n])  >> 15;
          AudioC.outputRight[n] = (Gain * AudioC.inputRight[n]) >> 15;
      }
    }

    
    
    
    
    
    // for Matlab
    
    if (GetAudioBufferFlag)
    {
        digitalWrite(LED0, LOW);
        for(int n = 0; n <BufferLength; n++)
        {
            AudioCaptureBufferLeft[n]  = AudioC.inputLeft[n];
        }
        GetAudioBufferFlag = 0;
        CapturedBufferFlag = 1;
    }
    
}




 
 
