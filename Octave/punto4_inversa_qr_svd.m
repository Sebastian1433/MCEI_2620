% Punto 4 - Inversa directa, QR y SVD
% Metodos Computacionales en Ingenieria - MCEI

clc;
clear;
format long g;

A = hilb(5);
I = eye(5);

% 1. Inversa directa
Ainv_directa = inv(A);
error_directa = norm(A*Ainv_directa - I, "fro");

% 2. Inversa mediante QR
[Q,R] = qr(A);
Ainv_qr = R\(Q'*I);
error_qr = norm(A*Ainv_qr - I, "fro");

% 3. Inversa mediante SVD
[U,S,V] = svd(A);
tol = max(size(A))*eps(max(diag(S)));
Sinv = zeros(size(S));

for k = 1:min(size(S))
    if S(k,k) > tol
        Sinv(k,k) = 1/S(k,k);
    end
end

Ainv_svd = V*Sinv*U';
error_svd = norm(A*Ainv_svd - I, "fro");

fprintf("==========================================\n");
fprintf("PUNTO 4 - INVERSA: DIRECTA, QR Y SVD\n");
fprintf("==========================================\n\n");

disp("Inversa directa:");
disp(Ainv_directa);

disp("Inversa mediante QR:");
disp(Ainv_qr);

disp("Inversa mediante SVD:");
disp(Ainv_svd);

fprintf("Errores de reconstruccion ||A*Ainv-I||_F:\n");
fprintf("Directa = %.12e\n", error_directa);
fprintf("QR      = %.12e\n", error_qr);
fprintf("SVD     = %.12e\n", error_svd);

fprintf("\nNota: un menor residuo puntual no implica que ese metodo sea siempre\n");
fprintf("el mas estable. QR y SVD ofrecen ventajas importantes en problemas\n");
fprintf("mal condicionados o cercanos a singularidad.\n");
