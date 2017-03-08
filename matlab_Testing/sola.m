function [y] = sola(xp, period)

%with overlap
win_size = 1024;
%since I am using a hamming window here, COLA needs to have an overlap
%factor of M/4 to prevent humming due to half of the main lobe aliasing
%onto itself (decreasing hop size increases frame rate).
%See Julius' book for reference on COLA
ola = 0.25;
hop = ola*(win_size);

N = length(xp);
nwin_size = ceil(N/hop);
xp = [xp, zeros(1,nwin_size*hop - N + (win_size - hop))]; 
y = zeros(1, length(xp));
win = hamming(win_size)';
y(1:hop) = xp(1:hop);
start = 1;
for n = 1:nwin_size   
    y(start:start + win_size - 1) = y(start:start + win_size - 1) + (xp(start:start + win_size - 1).*win);
    start = start + hop;
end

%need to get rid of :
%1)couple of periods at the beginning, because there is no overlap with previous frame there - 
%we just have windowed waveform, which is not equal to the original, but smaller in
%amplitude - this causes problems, so ignore it
%2)trailing zeros at the end of y

%why multiply with 0.5? to normalise amplitude - find out more
y = y(2*period+1:N) * 0.5;

figure(3);
plot(xp(1:5*win_size));grid on;hold on;
plot(y(1:5*win_size), 'r-');grid on; hold off;
legend('Original signal', 'Reconstructed Signal');
    
end

