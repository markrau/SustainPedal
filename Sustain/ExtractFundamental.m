baud = 115200;
comport = 'COM6';
%comport = '/dev/tty.usbserial-00001014B';

% Connect to the DSP Shield using a handshake
[s, connected] = serial_connect(comport, baud);

if connected
    diff = serial_recv_array(s, 'int16');
end

if(~isempty(diff))
    sprintf('Received data successfully')
    figure;plot(diff);
    diff
else
    sprintf('Data not received')
end

fclose(s);