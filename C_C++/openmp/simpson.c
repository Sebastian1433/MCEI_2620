#include <stdio.h>
#include <math.h>
#include <omp.h>

double f(double x)
{
    return sin(x);
}

/* Version secuencial, para tener contra que comparar */
double simpson_secuencial(double a, double b, int n)
{
    if (n % 2 != 0)
        n++;

    double h = (b - a) / n;
    double suma_par = 0.0, suma_impar = 0.0;

    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        if (i % 2 == 0)
            suma_par += f(x);
        else
            suma_impar += f(x);
    }

    return (h / 3.0) * (f(a) + f(b) + 2.0 * suma_par + 4.0 * suma_impar);
}

/* Version paralela: misma formula, con reduction sobre los dos acumuladores */
double simpson_paralelo(double a, double b, int n)
{
    if (n % 2 != 0)
        n++;

    double h = (b - a) / n;
    double suma_par = 0.0, suma_impar = 0.0;

#pragma omp parallel for reduction(+ : suma_par, suma_impar)
    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        if (i % 2 == 0)
            suma_par += f(x);
        else
            suma_impar += f(x);
    }

    return (h / 3.0) * (f(a) + f(b) + 2.0 * suma_par + 4.0 * suma_impar);
}

int main(void)
{
    double a = 0.0, b = M_PI;
    int n = 100000000;   /* 1e8 subintervalos */
    double exacto = 2.0; /* integral de sin(x) de 0 a pi */

    printf("Integral de sin(x) de 0 a pi\n");
    printf("n = %d subintervalos\n", n);
    printf("Hilos disponibles: %d\n\n", omp_get_max_threads());

    double t0 = omp_get_wtime();
    double r_sec = simpson_secuencial(a, b, n);
    double t_sec = omp_get_wtime() - t0;

    t0 = omp_get_wtime();
    double r_par = simpson_paralelo(a, b, n);
    double t_par = omp_get_wtime() - t0;

    printf("Secuencial: %.14f   error = %.3e   tiempo = %.5f s\n",
           r_sec, fabs(r_sec - exacto), t_sec);
    printf("Paralelo:   %.14f   error = %.3e   tiempo = %.5f s\n",
           r_par, fabs(r_par - exacto), t_par);

    printf("\nDiferencia entre las dos versiones: %.3e\n", fabs(r_sec - r_par));
    printf("Speedup: %.3f\n", t_sec / t_par);
    printf("Eficiencia: %.1f %%\n",
           100.0 * (t_sec / t_par) / omp_get_max_threads());

    return 0;
}