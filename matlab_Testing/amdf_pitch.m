function [pitch] = amdf_pitch(xbuf,fs)

%Pitch detection with AMDF

tau_min = round(fs/1500);
tau_max = round(fs/80);

tau_min = 32;
tau_max = 600;

D = zeros(1,tau_max - tau_min + 1);
L = length(xbuf);

for n = tau_min:tau_max
    for k = n+1:L
      D(n-tau_min+1) = D(n-tau_min+1) + abs(xbuf(k) - xbuf(k-n));
    end
    D(n-tau_min+1) = D(n-tau_min+1)/L;
end

figure(1);
plot(0:tau_max-tau_min, D);grid on;
xlabel('Lags in samples');ylabel('AMDF function');

thresh = 600;
minPos = -1;
 for i = 2:tau_max-tau_min-1
                if(D(i-1) > D(i) && D(i+1) > D(i) && D(i) <= thresh)
                        minPos = ((D(i-1) - D(i+1))/(D(i-1) - (D(i)*2) + D(i+1)))/2;
                        minPos = i + minPos;
                        minimum = D(i);
                        break;
                end
 end
                 
 
pitch = minPos + tau_min;
i + tau_min
pause;
%[val,pitch] = min(D);
%pitch = pitch+tau_min;
%pitch = fs/pitch;




end

