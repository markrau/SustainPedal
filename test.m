close all, clc;

[x,fs] = audioread('guitar files/G53-46201-1111-00015.wav');
x = x';
win = 1024;
nframes = ceil(length(x)/win);
x = [x, zeros(1,nframes*win - length(x))];
start = 1;   
P0_est = zeros(1,nframes);
time = zeros(1,nframes);
xbuf = zeros(nframes, win);
nsamples = zeros(1,nframes);

for i = 1:nframes
    xbuf(i,:) = x(start:start+win-1);
    P0_est(i) = yin_pitch(xbuf(i,:),fs);
    time(i) = (start-1)/fs;
    start = start + win;
    nsamples(i) = round(P0_est(i)*fs);
    %plot just to visually check if correct pitch was detected
%     if(i == 50)
%         figure(1);
%         plot(xbuf(i,:),'b');grid on;hold on;
%         plot(xbuf(i,1:nsamples(i)),'r');grid on;
%         hold off;
%     end
end

figure(1);
plot(time,1./P0_est);grid on;
xlabel('Time in seconds');ylabel('Estimated frequency');

%extract one period of waveform and loop infinitely
xp = [];
%ideally this should be the steady state buffer number following onset
%sorry for the misnomer.
onset = 20;
nperiods = 200;

%extract exactly one period of fundamental starting from 0 phase,
%loop that period for nperiods and send it to sola for overlap add
period = nsamples(onset);
xp = [];
[m,maxind] = max(xbuf(onset,1:period));
ind = find_zcr_before_peak(xbuf(onset,:), maxind);
for i = 0:nperiods
    xp = [xp, xbuf(onset,ind:ind+period)];
end
y = sola(xp,period);
        

% period = mode(nsamples(onset:onset+nperiods));
% %this next block of code basically does the following - 
% %it extracts exactly one period from each buffer starting at 0 phase
% %uncomment line 46 to see what it does
% xp = [];
% for i = 0:nperiods
%     [m,maxind] = max(xbuf(onset+i,1:period));
%     ind = find_zcr_before_peak(xbuf(onset+i,:), maxind);
%     figure(2);plot(xbuf(onset+i,ind:ind+period));hold on; grid on;
%     xp = [xp, xbuf(onset+i,ind:ind+period)];
% end
% figure(2);hold off;
% y = sola(xp,period);
%let's hear what the reconstructed SOLA signal sounds like
%soundsc(y,fs);

%let's hear what it sounds like combined with the original signal
xcons = zeros(1, (onset-1)*win);
start = 1;
for n = 1:onset-1
    xcons(start:start+win-1) = xbuf(n,:);
    start = start+win;
end
%linear interpolation between last sample in onset and first sample in
%steady state for smooth transition
interp = linspace(xcons(end), y(1), 100);
z = [xcons, interp, y];
sound(z,fs);

%audio sounds a bit 

%In real time C++, we will ideally keep one fundamental period of the
%waveform stored in an array and keep looping that in the output buffer,
%till the buffer length (1024) is exhausted. Then we need to save the last
%sample that we sent out, and go through a lookup table to see what
%followed the last sample in the waveform we preserved, and send that out to the next
%buffer.



