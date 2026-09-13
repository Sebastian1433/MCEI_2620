clear;
clc;
format long g;

disp("==========================================")
disp("PUNTO 2 - SISTEMA LINEAL 10x10")
disp("==========================================")
disp("")

A = [
2 1 0 3 2 1 0 2 1 4;
1 3 2 0 1 4 2 1 0 2;
0 2 4 1 3 0 1 2 4 1;
3 0 1 5 2 1 3 0 2 1;
2 1 3 2 6 2 1 4 0 3;
1 4 0 1 2 5 2 1 3 0;
0 2 1 3 1 2 4 0 2 1;
2 1 2 0 4 1 0 5 3 2;
1 0 4 2 0 3 2 3 6 1;
4 2 1 1 3 0 1 2 1 5
];

% El enunciado no suministra b.
% Para completar la comparacion se asume:
b = (1:10)';

disp("Vector b asumido:")
disp(b)

% -------------------------------------------------------
% Metodo 1: Eliminacion gaussiana
% En Octave, A\b utiliza metodos numericos internos robustos.
% -------------------------------------------------------
tic;
x_gauss = A \ b;
tiempo_gauss = toc;

% -------------------------------------------------------
% Metodo 2: LU
% -------------------------------------------------------
tic;
[L,U,P] = lu(A);
x_lu = U \ (L \ (P*b));
tiempo_lu = toc;

% -------------------------------------------------------
% Metodo 3: QR
% -------------------------------------------------------
tic;
[Q,R] = qr(A);
x_qr = R \ (Q' * b);
tiempo_qr = toc;

% -------------------------------------------------------
% Residuos
% -------------------------------------------------------
res_gauss = norm(A*x_gauss - b);
res_lu = norm(A*x_lu - b);
res_qr = norm(A*x_qr - b);

% -------------------------------------------------------
% Numero de condicion
% -------------------------------------------------------
cond_A = cond(A);

disp("")
disp("------------------------------------------")
disp("SOLUCION - A\\b")
disp("------------------------------------------")
disp(x_gauss)

disp("------------------------------------------")
disp("SOLUCION - LU")
disp("------------------------------------------")
disp(x_lu)

disp("------------------------------------------")
disp("SOLUCION - QR")
disp("------------------------------------------")
disp(x_qr)

disp("==========================================")
disp("COMPARACION NUMERICA")
disp("==========================================")

fprintf("Residuo A\\b = %.12e\n", res_gauss);
fprintf("Residuo LU   = %.12e\n", res_lu);
fprintf("Residuo QR   = %.12e\n", res_qr);

fprintf("\nTiempo A\\b = %.12e segundos\n", tiempo_gauss);
fprintf("Tiempo LU   = %.12e segundos\n", tiempo_lu);
fprintf("Tiempo QR   = %.12e segundos\n", tiempo_qr);

fprintf("\nNumero de condicion de A = %.12f\n", cond_A);

fprintf("\nDiferencia A\\b - LU = %.12e\n", norm(x_gauss-x_lu));
fprintf("Diferencia A\\b - QR = %.12e\n", norm(x_gauss-x_qr));
fprintf("Diferencia LU - QR   = %.12e\n", norm(x_lu-x_qr));

disp("")
disp("Conclusion:")
disp("Los tres metodos producen soluciones numericamente equivalentes.")
disp("LU es eficiente cuando se resuelven varios sistemas con la misma matriz.")
disp("QR suele ofrecer buena estabilidad numerica, aunque requiere mas operaciones.")