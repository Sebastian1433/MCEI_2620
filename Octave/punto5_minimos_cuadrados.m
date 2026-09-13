% Punto 5 - Minimos cuadrados: convertidor DC-DC
% Metodos Computacionales en Ingenieria - MCEI
% Modelo: P = beta0 + beta1*V + beta2*I + beta3*T

clc;
clear;
format long g;

% Buscar el CSV tanto si se ejecuta desde la raiz del repositorio
% como desde la carpeta Octave.
csvfile = "C_C++/matrices/datos_convertidor_realista.csv";

if ~exist(csvfile, "file")
    csvfile = "../C_C++/matrices/datos_convertidor_realista.csv";
end

if ~exist(csvfile, "file")
    csvfile = "datos_convertidor_realista.csv";
end

if ~exist(csvfile, "file")
    error("No se encontro datos_convertidor_realista.csv");
end

data = csvread(csvfile, 1, 0);

V = data(:,1);
I = data(:,2);
T = data(:,3);
P = data(:,4);

n = length(P);
X = [ones(n,1), V, I, T];
y = P;

% Solucion por QR / operador backslash de Octave
beta = X\y;

y_pred = X*beta;
residuos = y - y_pred;

MSE = mean(residuos.^2);
norma_residuos = norm(residuos);
max_residuo = max(abs(residuos));
cond_X = cond(X);

% Ecuaciones normales para comparacion
beta_normal = (X'*X)\(X'*y);
dif_qr_normal = norm(beta - beta_normal);

% Analisis de influencia relativa:
% estandarizar variables explicativas
Vz = (V - mean(V))/std(V);
Iz = (I - mean(I))/std(I);
Tz = (T - mean(T))/std(T);

Xz = [ones(n,1), Vz, Iz, Tz];
beta_std = Xz\P;

fprintf("==========================================\n");
fprintf("PUNTO 5 - MINIMOS CUADRADOS\n");
fprintf("==========================================\n\n");

fprintf("Numero de muestras: %d\n\n", n);

fprintf("Coeficientes del modelo:\n");
fprintf("beta0 = %.6f\n", beta(1));
fprintf("beta1 (Voltaje) = %.6f\n", beta(2));
fprintf("beta2 (Corriente) = %.6f\n", beta(3));
fprintf("beta3 (Temperatura) = %.6f\n\n", beta(4));

fprintf("Modelo:\n");
fprintf("P = %.6f + (%.6f)V + (%.6f)I + (%.6f)T\n\n", ...
        beta(1), beta(2), beta(3), beta(4));

fprintf("Indicadores del ajuste:\n");
fprintf("Norma de residuos = %.6f\n", norma_residuos);
fprintf("MSE = %.6f\n", MSE);
fprintf("Maximo residuo absoluto = %.6f\n", max_residuo);
fprintf("Numero de condicion de X = %.6f\n\n", cond_X);

fprintf("Comparacion QR vs ecuaciones normales:\n");
fprintf("Diferencia entre coeficientes = %.12e\n\n", dif_qr_normal);

fprintf("Coeficientes con variables explicativas estandarizadas:\n");
fprintf("Voltaje = %.6f\n", beta_std(2));
fprintf("Corriente = %.6f\n", beta_std(3));
fprintf("Temperatura = %.6f\n\n", beta_std(4));

[~, idx] = max(abs(beta_std(2:4)));
nombres = {"Voltaje", "Corriente", "Temperatura"};
fprintf("Variable de mayor influencia relativa: %s\n", nombres{idx});

fprintf("\nInterpretacion de coeficientes:\n");
fprintf("- beta1: cambio estimado en P por +1 V, manteniendo I y T constantes.\n");
fprintf("- beta2: cambio estimado en P por +1 A, manteniendo V y T constantes.\n");
fprintf("- beta3: cambio estimado en P por +1 grado C, manteniendo V e I constantes.\n");
fprintf("- beta0: intercepto matematico; puede no tener interpretacion fisica directa.\n");
