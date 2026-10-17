#include <stdio.h>
#include <math.h>
#include <omp.h>

double f(double x)
{
    return sin(x);
}

double trapecio_secuencial(double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.5 * (f(a) + f(b));

    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        sum += f(x);
    }

    return h * sum;
}

double trapecio_paralelo(double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.5 * (f(a) + f(b));

#pragma omp parallel for reduction(+ : sum)
    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        sum += f(x);
    }

    return h * sum;
}

int main(void)
{
    double a = 0.0, b = M_PI;
    int n = 100000000;
    double exacto = 2.0;

    printf("Regla del trapecio - integral de sin(x) de 0 a pi\n");
    printf("n = %d subintervalos\n", n);
    printf("Hilos disponibles: %d\n\n", omp_get_max_threads());

    double t0 = omp_get_wtime();
    double r_sec = trapecio_secuencial(a, b, n);
    double t_sec = omp_get_wtime() - t0;

    t0 = omp_get_wtime();
    double r_par = trapecio_paralelo(a, b, n);
    double t_par = omp_get_wtime() - t0;

    printf("Secuencial: %.14f   error = %.3e   tiempo = %.5f s\n",
           r_sec, fabs(r_sec - exacto), t_sec);
    printf("Paralelo:   %.14f   error = %.3e   tiempo = %.5f s\n",
           r_par, fabs(r_par - exacto), t_par);

    printf("\nDiferencia entre las dos versiones: %.3e\n", fabs(r_sec - r_par));
    printf("Speedup: %.3f\n", t_sec / t_par);

    return 0;
}