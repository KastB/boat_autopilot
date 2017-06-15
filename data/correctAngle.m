function  A = correctAngle(A, indexYaw)
    for n = 2:size(A.data,1)

        if A.data(n, indexYaw) -  A.data(n-1, indexYaw) > 180
            A.data(n, indexYaw) = A.data(n, indexYaw) - 360;
        end
        if A.data(n, indexYaw) -  A.data(n-1, indexYaw) < -180
            A.data(n, indexYaw) = A.data(n, indexYaw) + 360;
         end

    end
end