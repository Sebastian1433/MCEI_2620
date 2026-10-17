#include <stdio.h>
#include <math.h>
#include <omp.h>

/* Funcion a integrar */
double f(double x)
{
    return sin(x);
}

/* --- Version secuencial (para tener contra que comparar) --- */
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

/* --- Version paralela (la de la guia) --- */
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

/* Mide 3 veces y devuelve el MENOR tiempo. Los parametros pasan por
   variables volatile para que el compilador no pueda asumir que las
   tres llamadas dan lo mismo y borre dos de ellas. */
static double medir(double (*metodo)(double, double, int),
                    double a, double b, int n, double *resultado)
{
    double mejor = 1e30;

    for (int r = 0; r < 3; r++)
    {
        volatile double va = a, vb = b;
        volatile int vn = n;

        double t0 = omp_get_wtime();
        volatile double v = metodo(va, vb, vn);
        double t = omp_get_wtime() - t0;

        if (t < mejor)
            mejor = t;
        *resultado = v;
    }
    return mejor;
}
int main(void)
{
    double a = 0.0, b = M_PI;
    int n = 100000000;
    double exacto = 2.0;
    double r_sec = 0.0, r_par = 0.0;

    printf("Simpson 1/3 - integral de sin(x) de 0 a pi\n");
    printf("n = %d subintervalos\n", n);
    printf("Hilos disponibles: %d\n", omp_get_max_threads());
    printf("Cada tiempo es el menor de 3 corridas\n\n");

    /* Calentamiento: se descarta. Sube el CPU a turbo y llena las caches
       antes de que empiece la medicion que cuenta. */
    simpson_secuencial(a, b, n);
    simpson_paralelo(a, b, n);

    double t_sec = medir(simpson_secuencial, a, b, n, &r_sec);
    double t_par = medir(simpson_paralelo, a, b, n, &r_par);

    printf("Secuencial: %.14f   error = %.3e   tiempo = %.5f s\n",
           r_sec, fabs(r_sec - exacto), t_sec);
    printf("Paralelo:   %.14f   error = %.3e   tiempo = %.5f s\n",
           r_par, fabs(r_par - exacto), t_par);

    printf("\nDiferencia entre las dos versiones: %.3e\n", fabs(r_sec - r_par));
    printf("Speedup: %.3f\n", t_sec / t_par);
    printf("Eficiencia por nucleo fisico (2): %.1f %%\n",
           100.0 * (t_sec / t_par) / 2.0);

    return 0;
}