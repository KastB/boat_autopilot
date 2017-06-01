A = importdata('~/src/autopilot_git/data/data.csv');
B = A;
A = correctAngle(A,20);
A = correctAngle(A,21);

plot(   A.data(:,1), A.data(:,20), B.data(:,1), B.data(:,20),A.data(:,1), A.data(:,21), B.data(:,1), B.data(:,21))