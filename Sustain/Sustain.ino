/*
No Pain No Sustain: An audio effect which produced "infinite sustain" for input notes. 

The code assumes that there is a on/off switch connected to analog pin 2 of the arduino, and a potentiometer connected to analog pin 3. 

To run the code, upliad the Sustain.ino file to the DSP shield, then run the Sustain.m file to set up MATLAB communication. The code parameters are then controlled by the physical controls. 

Created by Orchisama Das and Mark Rau

*/

// Include audio library
#include <Audio.h>
#include <dsplib.h>
#include <OLED.h>
#include "serial_array.h"
#include "Onset.h"
#include "LoopAudio.h"


//================================

#define MAX_INT16           32767
#define MIN_INT16           -32768
#define MAX_INT32           2147483647
#define MIN_INT32           -2147483648

//=================================

// Globals
const int  BufferLength = 1024;               //< Buffer length
const int maxDataLength = 2048;
const long Gain         = 32768;              //< Gain parameter
const long baudRate      = 115200;            // baudRate for Matlab
const int maxBuffUntilSteadyState = 20;       //  number of buffers after onset until it is declared as steady state

int *filterState;                             // filter state for the LP filter for period detection
const int filterLength = 188;                 // length of filter for period detection

// Filter coefficients, taken from a Matlab Parks McClellan LP filter design with passband until 1kHz
const int coeff[filterLength] = 
            {-142,76,63,55,50,48,47,46,45,44,42,38,34,29,23,15,7,-3,-13,-23,-33,-44,-53,-62,-70,-76,-81,-83,-84,
            -82,-77,-70,-60,-47,-33,-16,2,21,41,62,82,101,119,134,147,156,161,163,159,151,138,120,97,70,39,4,-34,
            -73,-114,-154,-193,-230,-263,-291,-313,-329,-336,-334,-322,-300,-267,-223,-168,-102,-25,61,157,261,
            371,487,606,727,849,969,1085,1196,1300,1395,1480,1553,1612,1658,1689,1705,1705,1689,1658,1612,1553,
            1480,1395,1300,1196,1085,969,849,727,606,487,371,261,157,61,-25,-102,-168,-223,-267,-300,-322,-334,
            -336,-329,-313,-291,-263,-230,-193,-154,-114,-73,-34,4,39,70,97,120,138,151,159,163,161,156,147,134,
            119,101,82,62,41,21,2,-16,-33,-47,-60,-70,-77,-82,-84,-83,-81,-76,-70,-62,-53,-44,-33,-23,-13,-3,7,
            15,23,29,34,38,42,44,45,46,47,48,50,55,63,76,-142};
            


SerialCmd cmd(BufferLength);                             // Create instance of the serial command class


// Gloal Variables for Onset
long previousBuffFFTSum = MAX_INT16*BufferLength;        // Pervious fft buffer sum. Make the initial (negative time) buffer have the max FFT sum so no onset is detected
int onsetFlag = 0;                                       // Keep track of whether or not a buffer has an onset
int prevOnsetFlag = 0;                                   // Keep track of whether or not the previous buffer has an onset
int onsetThresh = MAX_INT16*0.25 ;                       // Onset detection threshold. Initialize to 4 (Q11), meaning that an onset is detected if a new buffer has 4 times the spectral energy as the previous buffer
int numBuffUntilSteadyState = 0;                         // counter for number of buffers until steady state
int period = 0;                                          // Period of the fundamental frequency detected in a buffer
int pedalPressed = 1;                                    // variable to dictate if the effect is activated
int foundOnset = 0;                                      // Keep track of if an onset was found
int foundSS = 0;                                         // Keep track of weather steady state was hit
int threshPot = 0;                                       // Onset detection threshold value to be changed bby potentiometer via arduino
int onSwitchRead = 0;                                    // variable to check if thew arduino on switch is pressed
int analogPin3 = 3;                                      // potentiometer wiper (middle terminal) connected to analog pin 3, outside leads to ground and +5V
int analogPin2 = 2;                                      // on off switch. 

int numMatlabCalls = 0;                                  // A counter to only call Matlab communitation a number of times


volatile int GetAudioBufferFlag = 0;                     // Trigger audio buffer capture in processAudio()
volatile int CapturedBufferFlag = 0;                     // Indicate that audio buffer has been captured in processAudio()

#pragma DATA_ALIGN(32)
int InputLeft[BufferLength] = {0};
#pragma DATA_ALIGN(32)
int InputRight[BufferLength] = {0};
#pragma DATA_ALIGN(32)
int filtOut[BufferLength] = {0};
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

//Declare LoopBuffer pitch detection object
//================================================================
LoopAudio loopAudio(BufferLength);
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
    pinMode(LED1, OUTPUT);
    
    // Turn LED0 on
    digitalWrite(LED0, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED1, LOW);
    
    //Initialize OLED module for status display	
    disp.oledInit();
    disp.clear();
    disp.flip();    
    disp.setline(1);
    
    filterState = new int[filterLength + 2]; // initialize filterstate 
       
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
    AudioC.setInputGain(0,0);
    
    if (status == 0)
    {
        disp.clear();
        disp.print("Process ON");
    }        
  
    serial_connect(baudRate);             // connect to matlab
}

/** Main application loop

  The loop function runs repetitively and immediatelly after calling the setup() function.
  The loop function is part of the Energia framework.

*/
void loop()
{
  // For some reason the max value read by the DSP shield is 120, so bit shift it by 7. Knob turned to left gives steep onset detection, knob turned to right gives easy onset detection
  threshPot = analogRead(analogPin3);
  onsetThresh = threshPot << 7;
   
  onSwitchRead = analogRead(analogPin2);
  if(onSwitchRead > 40){
    pedalPressed =1;
  }
  else{
    pedalPressed = 0;
    for(int n = 0; n < BufferLength; n++)
        {
          InputLeft[n] = 0; 
          InputRight[n] = 0;
        }
  }
  
     // Print the period so you can check if it was detected properly
     disp.clear();
     disp.setline(0);
     disp.print((long)period);
    

    // for the volume control using a bit shift of 3 on the output volume to bring the max value from 1023 to 127, hopefully this will work and not e too sketchy
    // Note: This is currently not implemented on the arduino but may be in the future
    // AudioC.setOutputVolume(outputVolume >>3, outputVolume >>3);
  

    // An LED flashes to show if an onset occured. 
    // This probably won't work if we are also printing the pitch detection to the screen. DSP shield is too slow
    if(onsetFlag){
       digitalWrite(LED2, HIGH);
    }
    else{
        digitalWrite(LED2, LOW);
    }
    
    if(foundSS){
      digitalWrite(LED1, HIGH);
    }
    else{
     digitalWrite(LED1, LOW); 
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
          case 0: // Send buffer to matlab         
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

    onsetFlag = onset.isOnset(AudioC.inputLeft, onsetThresh);
    
   if(onsetFlag){
      numBuffUntilSteadyState = 0;
      foundSS =0;
      numMatlabCalls = 0;
   }     
   numBuffUntilSteadyState++;

    //if previous buffer was onset and current buffer is steady state
     if(numBuffUntilSteadyState > maxBuffUntilSteadyState && pedalPressed && !foundSS){     
        foundSS = 1;
        
        for(int n = 0; n < BufferLength; n++)
        {
          InputLeft[n] = AudioC.inputLeft[n]; 
          //InputRight[n] = AudioC.inputRight[n];
        }
               
        fir((DATA*)InputLeft, (DATA*)coeff, (DATA*)filtOut, (DATA*)filterState, BufferLength, filterLength);
        period = loopAudio.getPitchAMDF(filtOut);
     }
    

    if(foundSS && pedalPressed && period!=0){
      loopAudio.loopBuffer(InputLeft,OutputLeft,period);
      for(int n = 0; n < BufferLength; n++) {
        AudioC.outputLeft[n]  = (Gain * OutputLeft[n])  >> 15;
       // AudioC.outputRight[n] = (Gain * OutputLeft[n]) >> 15;
      }  
    }
    else{
      for(int n = 0; n < BufferLength; n++) {
          AudioC.outputLeft[n]  = (Gain * AudioC.inputLeft[n])  >> 15;
        // AudioC.outputRight[n] = (Gain * AudioC.inputLeft[n]) >> 15;
      }
    }


    // for Matlab communication
    if (GetAudioBufferFlag)
    {
        digitalWrite(LED0, LOW);
        for(int n = 0; n <BufferLength; n++)
        {
            AudioCaptureBufferLeft[n]  = OutputLeft[n];
        }
        GetAudioBufferFlag = 0;
        CapturedBufferFlag = 1;
    }
}



