A = importdata('~/src/autopilot_git/data/data.csv');
B = A;
yaw_index = 21;
yaw_smooth_index = 19;
A = correctAngle(A,yaw_smooth_index);
A = correctAngle(A,21);

d = diff(A.data(:,yaw_smooth_index));
d = [0;d];
e = diff(A.data(:,yaw_index));
e = [0;e];
plot(   A.data(:,1), A.data(:,yaw_smooth_index), B.data(:,1), B.data(:,yaw_smooth_index),A.data(:,1), A.data(:,yaw_index), B.data(:,1), B.data(:,yaw_index));

u = A.data(:,7);
y = A.data(:,5);

t = A.data(:,1);
t = t./1000;
ruder_soll = timeseries(u,t);
ruder_ist = timeseries(y,t);
yawrate = timeseries(d,t);

plant_output = timeseries(A.data(1:end,yaw_smooth_index),t(1:end));
speed = timeseries(A.data(1:end,31),t(1:end));

