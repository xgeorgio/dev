function alertsound( sdur, ntimes )

for k=1:ntimes,
    sound(randn(1*8192,1));
    pause(2);
end;
