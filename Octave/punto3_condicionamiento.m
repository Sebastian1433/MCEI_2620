% Punto 3 - Condicionamiento numerico y estabilidad
% Metodos Computacionales en Ingenieria - MCEI

clc;
clear;
format long g;

n = 5;
A = hilb(n);

x_real = ones(n,1);
b = A*x_real;

cond_A = cond(A);
x = A\b;

% Perturbacion pequena en el primer elemento de b
b_pert = b;
b_pert(1) = b_pert(1) + 1e-5;

x_pert = A\b_pert;
error_rel = norm(x_pert - x)/norm(x);

fprintf("==========================================\n");
fprintf("PUNTO 3 - CONDICIONAMIENTO Y ESTABILIDAD\n");
fprintf("==========================================\n\n");

disp("Matriz de Hilbert A:");
disp(A);

fprintf("Numero de condicion cond(A) = %.12e\n\n", cond_A);

disp("Solucion original:");
disp(x);

disp("Solucion con perturbacion:");
disp(x_pert);

fprintf("Error relativo = %.12e\n", error_rel);
fprintf("Error relativo porcentual = %.6f %%\n", 100*error_rel);

fprintf("\nInterpretacion:\n");
fprintf("La matriz presenta fuerte mal condicionamiento. Una perturbacion pequena\n");
fprintf("en b genera una variacion apreciable en la solucion.\n");
