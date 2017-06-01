A = importdata('~/src/autopilot_git/data.csv');
indexYaw = 20;
B = A
for n = 2:size(A.data,1)
    
    if A.data(n, indexYaw) -  A.data(n-1, indexYaw) > 180
        A.data(n, indexYaw)
        A.data(n, indexYaw) = A.data(n, indexYaw) - 360;
        A.data(n, indexYaw)
    end
    if A.data(n, indexYaw) -  A.data(n-1, indexYaw) < -180
        A.data(n, indexYaw)
        A.data(n, indexYaw) = A.data(n, indexYaw) + 360;
        A.data(n, indexYaw)
     end
    
end
A
plot(A.data(:,1), A.data(:,indexYaw),B.data(:,1), B.data(:,indexYaw))
