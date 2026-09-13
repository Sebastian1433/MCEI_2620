% Punto 1 - Pseudoinversa de Moore-Penrose
% Metodos Computacionales en Ingenieria - MCEI

clc;
clear;
format long g;

A = [1 0 2;
     2 -1 5;
     0 1 -1;
     1 3 -1];

Ap = pinv(A);

fprintf("==========================================\n");
fprintf("PUNTO 1 - PSEUDOINVERSA DE MOORE-PENROSE\n");
fprintf("==========================================\n\n");

disp("Matriz A:");
disp(A);

disp("Pseudoinversa A+:");
disp(Ap);

% Verificacion de las cuatro propiedades de Moore-Penrose
e1 = norm(A*Ap*A - A, "fro");
e2 = norm(Ap*A*Ap - Ap, "fro");
e3 = norm((A*Ap)' - A*Ap, "fro");
e4 = norm((Ap*A)' - Ap*A, "fro");

fprintf("Errores de verificacion:\n");
fprintf("1) ||A*A+*A - A||_F       = %.12e\n", e1);
fprintf("2) ||A+*A*A+ - A+||_F     = %.12e\n", e2);
fprintf("3) ||(A*A+)' - A*A+||_F   = %.12e\n", e3);
fprintf("4) ||(A+*A)' - A+*A||_F   = %.12e\n", e4);

fprintf("\nConclusion: errores cercanos a cero confirman las cuatro propiedades.\n");
