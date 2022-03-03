% a = arduino('COM10', 'uno')
% a = arduino('COM8', 'mega2560')
TIME_MAX = 10;
times = 1:TIME_MAX;
iteration = 1;

analog_a1 = zeros(1, TIME_MAX);
analog_a2 = zeros(1, TIME_MAX);
analog_a3 = zeros(1, TIME_MAX);
analog_a4 = zeros(1, TIME_MAX);

figure(1); clf;    hold on;
% 
while (true)
    analog_a1(iteration) =  readVoltage(a, 'A1');
    analog_a2(iteration) =  readVoltage(a, 'A2');
    analog_a3(iteration) =  readVoltage(a, 'A3');
    analog_a4(iteration) =  readVoltage(a, 'A4');

iteration = iteration + 1;

    
     plot(times, analog_a1);
 
     drawnow;
    if( iteration >
end

