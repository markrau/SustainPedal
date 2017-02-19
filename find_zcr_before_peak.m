function [pos] = find_zcr_before_peak(x, peakpos)
%this function finds the position of first zero crossing in x before peak
%if no zcr is found before peak, it looks for zcrs following the peak
%this is kind of hacky
pos = 0;
for i = peakpos:-1:2
    if(x(i) > 0 && x(i-1) <= 0)
        pos = i-1;
        break;
    end
end

if(pos == 0)
    for i = peakpos + 1:length(x)-1
        if(x(i) <= 0 && x(i-1) > 0)
            pos = i;
            break;
        end
    end
end
    
end

