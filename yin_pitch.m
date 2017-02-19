function [P0] = yin_pitch(xbuf,fs)
%pitch detection on a block basis
%xbuf - incoming buffer of 1024/512 samples
%fs - sampling rate
%P0 - fundamental period  estimate in samples

win = length(xbuf);
d = zeros(1,win);
xbuf = [xbuf, zeros(1,win)];

%step 1 - calculate difference function 
for tau = 0:win-1
    for j = 1:win  
         d(tau+1) = d(tau+1) + (xbuf(j) - xbuf(j+tau)).^2;         
    end
end

%step 2 - cumulative mean normalised difference function
d_norm = zeros(1,win);
d_norm(1) = 1;

for tau = 1:win-1
     d_norm(tau+1) = d(tau+1)/((1/tau) * sum(d(1:tau+1)));
end


%step 3 - absolute thresholding
th = 0.1;
lag = find(d_norm < th,1);
if(isempty(lag) == 1)
    [v,lag] = min(d_norm);
end

%step 4 - parabolic interpolation
if(lag > 1 && lag < win)
    alpha = d_norm(lag-1);
    beta = d_norm(lag);
    gamma = d_norm(lag+1);
    peak = 0.5*(alpha - gamma)/(alpha - 2*beta + gamma);
else
    peak = 0;
end

P0 = ((lag-1) + peak)/fs;

end


