% =====================================================================
%  Taller de diferenciacion numerica - Seccion 3: datos del robot
%  Genera trayectoria_robot.csv con 51 muestras equiespaciadas.
%
%  t_i = 0.2*i,  i = 0..50
%  x(t) = 0.08 t^2 + 0.40 sin(0.45 t)
%  y(t) = 0.50 t  + 0.30 (1 - cos(0.45 t))
% =====================================================================
clc; clear;

h = 0.2;
t = (0:50)' * h;

x = 0.08*t.^2 + 0.40*sin(0.45*t);
y = 0.50*t    + 0.30*(1 - cos(0.45*t));

archivo = "../C_C++/diferenciacion/trayectoria_robot.csv";
fid = fopen(archivo, "w");
fprintf(fid, "t,x,y\n");
fprintf(fid, "%.10f,%.10f,%.10f\n", [t, x, y]');
fclose(fid);

printf("Archivo generado: %s\n", archivo);
printf("Muestras: %d   h = %.2f s   t en [%.1f, %.1f] s\n", numel(t), h, t(1), t(end));
printf("x en [%.4f, %.4f] m\n", min(x), max(x));
printf("y en [%.4f, %.4f] m\n", min(y), max(y));