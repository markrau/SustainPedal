function [y] = sola(xp, period)

%without overlap
% N = length(xp)
% nperiod = ceil(N/period);
% xp = [xp, zeros(1, period*nperiod - N)];
% y = zeros(1, length(xp));
% start = 1;
% for n = 1:nperiod
%     win = xp(start:start + period -1) .* hann(period)';
%     y(start:start + period-1) = win;
%     start = start + period;
% end
% y = y(1:N);

%with overlap
ola = 0.5;
if(mod(period,2) == 0)
    hop = ola * period;
else
    hop = ola * (period-1);
end
N = length(xp);
nperiod = ceil(N/hop);
xp = [xp, zeros(1,nperiod*hop - N + (period - hop))]; 
y = zeros(1, length(xp));
win = hamming(period)';
start = 1;
for n = 1:nperiod
    y(start:start + period - 1) = y(start:start + period - 1) + (xp(start:start + period - 1).*win);
    start = start + hop;
end
%need to get rid of trailing zeros at the end of y
y = y(1:N);


figure(3);
plot(y(1:5*period));grid on;
title('Reconstructed signal');
    
end

