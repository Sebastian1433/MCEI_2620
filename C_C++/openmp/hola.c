#include <stdio.h>
#include <omp.h>

int main(void)
{
    printf("Hilos disponibles: %d\n\n", omp_get_max_threads());

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int tot = omp_get_num_threads();
        printf("Hola desde el hilo %d de %d\n", id, tot);
    }

    return 0;
}