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

figure(2);
plot(time,1./P0_est);grid on;
xlabel('Time in seconds');ylabel('Estimated frequency');

%align periods selected from first 10 buffers after onset
xp = [];
%ideally this should be the onset buffer number
onset = 20;
nperiods = 20;
period = mode(nsamples(onset:onset+nperiods));
%this next block of code basically does the following - 
%it extracts exactly one period from each buffer starting at 0 phase
%uncomment line 46 to see what it does
xp = [];
for i = 0:nperiods
    [m,maxind] = max(xbuf(onset+i,1:period));
    ind = find_zcr_before_peak(xbuf(onset+i,:), maxind);
    %figure;plot(xbuf(onset+i,ind:ind+period));
    xp = [xp, xbuf(onset+i,ind:ind+period)];
end
y = sola(xp,period);
%let's hear what the reconstructed SOLA signal sounds like
%soundsc(y,fs);

%let's hear what it sounds like combined with the original signal
xcons = zeros(1, (onset-1)*win);
start = 1;
for n = 1:onset-1
    xcons(start:start+win-1) = xbuf(n,:);
    start = start+win;
end
yins = repmat(y,[1,5]);
figure(4);
plot(yins(1:2*length(y)));grid on;
z = [xcons, yins];
soundsc(z,fs);

%you will find obvious problems in the audio output.When I use repmat to repeat y,
%subsequent repetitions of y are audibly discontinuous. From figure 4, it looks as
%though phase is not aligned. I tried linear interpolation between subsequent y's 
%to smooth the transition, but that didn't work either. Any suggestions on how 
%to fix this? Or do you think something else is causing it?

%In real time C++, we will ideally keep one fundamental period of the
%waveform stored in an array and keep looping that in the output buffer,
%till the buffer length (1024) is exhausted. Then we need to save the last
%sample that we sent out, and go through a lookup table to see what
%followed the last sample in the waveform we preserved, and send that out to the next
%buffer.

% nrep = 5;
% %number of samples to be interpolated between subsequent frames
% interp = 50;
% ynew = zeros(1, length(y) + interp*nrep);
% start = 1;
% for i = 1:nrep
%     ynew(start:start + length(y)-1) = y;
%     ynew(start+length(y):start+length(y)+interp-1) = linspace(y(end), y(1), interp);
%     start = start + length(y) + interp;
% end
% pause;
% znew = [xcons, ynew];
% soundsc(znew,fs);


