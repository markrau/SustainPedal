close all, clc;

%try it with both audio files
[x,fs] = audioread('guitar files/G53-46201-1111-00015.wav');
%[x,fs] = audioread('guitar files/G53-50300-1111-00027.wav');
x = x';
win = 1024;
nframes = ceil(length(x)/win);
x = [x, zeros(1,nframes*win - length(x))];
start = 1;   
time = zeros(1,nframes);
xbuf = zeros(nframes, win);

%break into frames of 1024 samples
for i = 1:nframes
    xbuf(i,:) = x(start:start+win-1);
    time(i) = (start-1)/fs;
    start = start + win;
end

%ideally this should be the steady state buffer number following onset  
steady_state = 50;
nperiods = 800;
%detect pitch period
period = round(yin_pitch(xbuf(steady_state,:),fs)*fs);

%extract exactly one period of fundamental starting from 0 phase,
%loop that period for nperiods and send it to sola for overlap add
xp = [];
[m,maxind] = max(xbuf(steady_state,1:period));
ind = find_zcr_before_peak(xbuf(steady_state,:), maxind);
for i = 0:nperiods
    xp = [xp, xbuf(steady_state,ind:ind+period)];
end
y = sola(xp,period);
   
%initial part of reconstructed signal - attack is going to come from
%original signal
xcons = [];
start = 1;
for n = 1:steady_state-1 
    xcons(start:start+win-1) = xbuf(n,:);
    start = start+win;
end

%this maintains continuity between initial(onset) and looped(steady state) regions
%leading to a smooth transition
% th = 10^-4;
% for i = length(xbuf(steady_state,:)) : -1 : 2
%     if(abs(xbuf(steady_state,i) - y(1)) < th)  
%         xcons(start:start+i-2) = xbuf(steady_state,1:i-1);
%         break;
%     end
% end

for i = length(xbuf(steady_state,:)) : -1 : 2
    if(xbuf(steady_state,i) > 0 && xbuf(steady_state,i-1) <= 0)  
        xcons(start:start+i-2) = xbuf(steady_state,1:i-1);
        break;
    end
end

%multiply y with a slowly varying amplitude envelope - amplitude modulation
%makes it sound more like a real guitar signal
t = (0:length(y)-1)/fs;
mod_ind = 0.05;
fm = 2;
ys = (1 + mod_ind*sin(2*pi*fm.*t)).*y;
z = [xcons, ys];

%multiply end of reconstructed signal (last few periods) with linearly decaying window
num_decay_buf = 100;
lin_dec = linspace(1, 0, (num_decay_buf*period));
z = [z(1:end-length(lin_dec)), z(end-length(lin_dec)+1:end).*lin_dec];

soundsc(z,fs);
figure;
subplot(211);
plot((0:length(x)-1)/fs,x);title('Original signal');xlabel('Time in seconds');
subplot(212);plot((0:length(z)-1)/fs,z);xlabel('Time in seconds');
title('Reconstructed signal');
    

%In real time C++, we will ideally keep one fundamental period of the
%waveform stored in an array and keep looping that in the output buffer,
%till the buffer length (1024) is exhausted. Then we need to save the last
%sample that we sent out, and go through a lookup table to see what
%followed the last sample in the waveform we preserved, and send that out to the next
%buffer.



