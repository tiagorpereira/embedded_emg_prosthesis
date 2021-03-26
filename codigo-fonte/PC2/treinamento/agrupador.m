local = 'sofia';
M=cell(0);
t=[];
for index = 1:5
    load([local sprintf('/A/emg_0%d.mat',index)],'classe','x_windowed');
    M = [M;x_windowed.'];
    t=[t;classe];
end
load([local '/Ref/emg_01.mat'],'classe','x_windowed');
M = [M;x_windowed];
t=[t;classe];