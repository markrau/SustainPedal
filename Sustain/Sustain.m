% Test the DSP Lib FIR implementation
% 
% This script is a companion to the FilterMatlabTest.ino sketch, which
% accepts the following serial commands from Matlab:
% cmd (0, filterCoeff): sets the FIR filter coefficients
% cmd (1, input): sets input array, apply filter and return output
%
% Note: we are using Q.15 fixed-point format throughout this lab.
%
% This script is missing an appropriate fixed-point model for the FIR
% operation.

%% Define constants for the connection
baud = 115200;
% comport = 'COM6';
comport = '/dev/tty.usbserial-145B';

% Connect to the DSP Shield using a handshake
[s, connected] = serial_connect(comport, baud);

% Filter%
%filterCoeff = 2^14 * [0, 0, 1, 0, 0];
%filterCoeff = 2^12 * ones(1,8);

% Input data
%dataLength = 64;
%input = 2*(1:dataLength)';
%input = 2^8*(1:dataLength)';

bufferLength = 1024;

%% Only communicate if we successfully connect
if connected
    % Set FIR filter coefficients
    % serial_cmd(s, 0, [int16(filterCoeff)], 'int16');
    
    % Send first half of data
    serial_cmd(s, 0, [int16(0)], 'int16');

    % Receive first half of output
    buffer1 = serial_recv_array(s, 'int16');
    
    
    % Try to fix the hold on command
    %serial_cmd(s, 1, [int16(0)], 'int16');
    
    % Send second half of data
  %  serial_cmd(s, 0, [int16(0)], 'int16');
    
    % Receive second half of output
   % buffer2 = serial_recv_array(s, 'int16');
    
    %output = [output1; output2];
end

% Compare results
% Need to create fixed-point model of FIR filter
% output_ref = filter(filterCoeff / 2^15, 1, input);
%output_ref = filterFixedPointModel(filterCoeff, input);

%error = sum(abs(output - output_ref))

% plot(1:dataLength, input, 'k+', ...
%     1:dataLength, output,'ro', ...
%     1:dataLength, output_ref,'bx');
% legend('Input', 'Output', 'Reference', 'Location', 'NorthWest');
% xlabel('Sample index');

%% Close the stream
fclose(s);