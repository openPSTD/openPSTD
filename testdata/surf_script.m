%
MATLAB script
to keep
my CPUs
busy
        function[Z] = surf_script(array)
fprintf(sprintf('Script for computing a surf plot of %s\n', inputname(1)))
figure
        n = size(array, 3);
fprintf('%d entries\n',n);
for
i = 1
:
n
        surf(array(:,
:,i));
title(inputname(1),
'interpreter','none');
pause(0.2)
end
        end

