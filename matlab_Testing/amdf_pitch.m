function [pitch] = amdf_pitch(xbuf,fs)

%Pitch detection with AMDF

tau_min = round(fs/1500);
tau_max = round(fs/80);

D = zeros(1,tau_max - tau_min + 1);
L = length(xbuf);

for n = tau_min:tau_max
    for k = n+1:L
      D(n-tau_min+1) = D(n-tau_min+1) + abs(xbuf(k) - xbuf(k-n));
    end
    D(n-tau_min+1) = D(n-tau_min+1)/L;
end

figure(1);
plot(tau_min:tau_max, D);grid on;
xlabel('Lags in samples');ylabel('AMDF function');

[val,pitch] = min(D);
pitch = pitch+tau_min;
pitch = fs/pitch;




end

