% =====================================================================
%  Taller de diferenciacion numerica - PARTE 1: GNU Octave
%  Estimacion de velocidad lineal y angular de un robot diferencial
%  a partir de posiciones discretas (x, y, t).
%
%  Procedimiento:  (x,y,t) -> (xdot,ydot) -> (v,theta) -> omega
% =====================================================================
clc; clear; close all;

% ---------------------------------------------------------------
% 1. Carga de datos
% ---------------------------------------------------------------
datos = csvread("../C_C++/diferenciacion/trayectoria_robot.csv", 1, 0);   % salta el encabezado
t = datos(:,1);
x = datos(:,2);
y = datos(:,3);

n = numel(t);
h = t(2) - t(1);

printf("==================================================\n");
printf("PARTE 1 - GNU OCTAVE\n");
printf("==================================================\n\n");
printf("Muestras cargadas: %d\n", n);
printf("Paso temporal h  : %.3f s\n\n", h);

% ---------------------------------------------------------------
% 2. Derivadas por diferencias centradas
%
%    Interior:  f'(t_i) ~ (f_{i+1} - f_{i-1}) / (2h)      O(h^2)
%    Extremos:  formulas unilaterales de tres puntos      O(h^2)
%               (se prefieren a las de dos puntos O(h)
%                para no degradar el orden en los bordes)
% ---------------------------------------------------------------
function d = derivada_central(f, h)
  n = numel(f);
  d = zeros(n,1);
  d(2:n-1) = (f(3:n) - f(1:n-2)) / (2*h);         % interior, vectorizado
  d(1)     = (-3*f(1) + 4*f(2) - f(3)) / (2*h);   % adelante, 3 puntos
  d(n)     = ( 3*f(n) - 4*f(n-1) + f(n-2)) / (2*h); % atras, 3 puntos
end

xdot = derivada_central(x, h);
ydot = derivada_central(y, h);

% ---------------------------------------------------------------
% 3. Velocidad lineal, orientacion y velocidad angular
% ---------------------------------------------------------------
v = sqrt(xdot.^2 + ydot.^2);

theta_crudo = atan2(ydot, xdot);
theta = unwrap(theta_crudo);      % DESENVOLVIMIENTO ANGULAR

omega = derivada_central(theta, h);

% ---------------------------------------------------------------
% 4. Verificacion contra las derivadas analiticas
% ---------------------------------------------------------------
xdot_ex = 0.16*t + 0.18*cos(0.45*t);
ydot_ex = 0.50   + 0.135*sin(0.45*t);
xdd_ex  = 0.16   - 0.081*sin(0.45*t);
ydd_ex  = 0.06075*cos(0.45*t);

v_ex     = sqrt(xdot_ex.^2 + ydot_ex.^2);
theta_ex = unwrap(atan2(ydot_ex, xdot_ex));
omega_ex = (xdot_ex.*ydd_ex - ydot_ex.*xdd_ex) ./ (xdot_ex.^2 + ydot_ex.^2);

interior = 2:n-1;
printf("Errores maximos frente a la solucion analitica:\n");
printf("  xdot   interior = %.3e    global = %.3e\n", ...
       max(abs(xdot(interior)-xdot_ex(interior))), max(abs(xdot-xdot_ex)));
printf("  v      interior = %.3e    global = %.3e\n", ...
       max(abs(v(interior)-v_ex(interior))), max(abs(v-v_ex)));
printf("  theta  interior = %.3e    global = %.3e\n", ...
       max(abs(theta(interior)-theta_ex(interior))), max(abs(theta-theta_ex)));
printf("  omega  interior = %.3e    global = %.3e\n", ...
       max(abs(omega(interior)-omega_ex(interior))), max(abs(omega-omega_ex)));

printf("\nRangos obtenidos:\n");
printf("  v     en [%.4f, %.4f] m/s\n",   min(v), max(v));
printf("  theta en [%.4f, %.4f] rad\n",   min(theta), max(theta));
printf("  omega en [%.6f, %.6f] rad/s\n", min(omega), max(omega));

% ---------------------------------------------------------------
% 5. Comparacion de esquemas sobre xdot (adelante / atras / centrada)
% ---------------------------------------------------------------
ad = (x(2:n) - x(1:n-1)) / h;        % evaluado en i = 1..n-1
at = (x(2:n) - x(1:n-1)) / h;        % evaluado en i = 2..n
ce = (x(3:n) - x(1:n-2)) / (2*h);    % evaluado en i = 2..n-1

printf("\nComparacion de esquemas (error maximo en xdot):\n");
printf("  hacia adelante  O(h)   = %.3e\n", max(abs(ad - xdot_ex(1:n-1))));
printf("  hacia atras     O(h)   = %.3e\n", max(abs(at - xdot_ex(2:n))));
printf("  centrada        O(h^2) = %.3e\n", max(abs(ce - xdot_ex(2:n-1))));

% ---------------------------------------------------------------
% 6. Graficas
% ---------------------------------------------------------------
if (~exist('figuras','dir')) mkdir('figuras'); end
figure('visible','off');
plot(x, y, 'b-', 'linewidth', 1.6); hold on;
plot(x(1), y(1), 'go', 'markersize', 9, 'linewidth', 2);
plot(x(n), y(n), 'rs', 'markersize', 9, 'linewidth', 2);
grid on; axis equal;
xlabel('x [m]'); ylabel('y [m]');
title('Trayectoria del robot');
legend('trayectoria','inicio','fin','location','northwest');
print -dpng 'figuras/octave_trayectoria.png';

figure('visible','off');
plot(t, v, 'b-', 'linewidth', 1.5); hold on;
plot(t, v_ex, 'r--', 'linewidth', 1.2);
grid on; xlabel('t [s]'); ylabel('v [m/s]');
title('Velocidad lineal'); legend('numerica','analitica','location','southeast');
print -dpng 'figuras/octave_velocidad.png';

figure('visible','off');
plot(t, theta, 'b-', 'linewidth', 1.5); hold on;
plot(t, theta_ex, 'r--', 'linewidth', 1.2);
grid on; xlabel('t [s]'); ylabel('\theta [rad]');
title('Orientacion con unwrap'); legend('numerica','analitica','location','southeast');
print -dpng 'figuras/octave_theta.png';

figure('visible','off');
plot(t, omega, 'b-', 'linewidth', 1.5); hold on;
plot(t, omega_ex, 'r--', 'linewidth', 1.2);
grid on; xlabel('t [s]'); ylabel('\omega [rad/s]');
title('Velocidad angular'); legend('numerica','analitica','location','southeast');
print -dpng 'figuras/octave_omega.png';

printf("\nGraficas guardadas en Octave/figuras/\n");

% ---------------------------------------------------------------
% 7. Exportar resultados
% ---------------------------------------------------------------
fid = fopen("resultados_octave.csv","w");
fprintf(fid, "t,x,y,xdot,ydot,v,theta,omega\n");
fprintf(fid, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n", ...
        [t x y xdot ydot v theta omega]');
fclose(fid);
printf("Resultados en resultados_octave.csv\n");
