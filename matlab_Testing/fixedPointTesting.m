close all;
clear all;
clc

[gtrTest_float,fs] = audioread('guitar files/G53-42102-1111-00003.wav');

gtrTest_fixed = fi(gtrTest_float);

bufLen = 1024;
sigLen = length(gtrTest_float);
numBufs = ceil(sigLen/bufLen);

periods = fi(zeros(1,numBufs));

for i=0:numBufs-2
    tempBuf = gtrTest_fixed(1+(i*bufLen):(i+1)*bufLen);
    amdf = fi(zeros(1,bufLen));
    
    for j=1:bufLen
       for k=1:bufLen
          amdf(j) = amdf(j) + abs(tempBuf(k)-tempBuf(k-j));
           
       end
       
    end
    amdf = amdf/bufLen;
    
    
    
    
    
end