function [ onsetBuff, ssBuff ] = find_onset(xbuf, fs, thresh )
% Simple onset detection. It determines there was an onset if the spectral energy increased from one buffer to the next. 
% The steady state region can eb determined y comparing the specrral energy
% decay to a constant since it is perceptual. 
% xbuf -> buffered input signal (buffered signals as rows)
% fs -> sample rate
% thresh -> steady state threshold (1.05 might be an okay value)
% onsetBuff is the buffer of the onset, returns zero if no onset found
% ssBuff is the buffer of steady state, returns zero if none found

% fft the buffers, need buffers in the columns for matrix fft
NFFT = 2^(nextpow2(length(xbuf(1,:)))+1);
hannWin = hann(length(xbuf(1,:)));
Xbuf = fft(xbuf'.*hannWin,NFFT);
% mean spectral energy magnitude
specEnergyMeans = mean(abs(Xbuf(1:NFFT/2,:)));

foundOnset = false;
foundSS = false;
onsetBuff =0;
ssBuff = 0;

for i=1:length(specEnergyMeans)-1
    onsetRatio = specEnergyMeans(i+1)/specEnergyMeans(i);
    %The onset ratio was set to 4 based on looking at an example
    if(onsetRatio>4 && foundOnset==false)
        onsetBuff = i+1;
        foundOnset = true;
    end
    ssRatio = specEnergyMeans(i)/specEnergyMeans(i+1);
    if(ssRatio <1 && ssRatio>thresh && foundSS==false && foundOnset ==true)
        ssBuff = i+1;
        foundSS=true;
    end
    
end












end

