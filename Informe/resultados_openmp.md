# Comparación de métodos de integración con OpenMP

**Autor:** Sebastián Barahona

Integral de $\sin(x)$ entre 0 y $\pi$, cuyo valor exacto es **2**.
$n = 10^8$ subintervalos.

**Máquina:** Intel i7-7500U @ 2.70 GHz — 2 núcleos físicos, 4 hilos lógicos
(Hyper-Threading). `gcc 15.2.0` con `-fopenmp -O2`, sobre WSL/Ubuntu.

## Metodología de medición

Cada tiempo reportado es el **menor de 3 corridas**, precedidas por una corrida
de calentamiento que se descarta. Se tomó el mínimo y no el promedio porque la
interferencia del sistema solo puede hacer que una corrida tarde más, nunca
menos: el mínimo es el valor más cercano al tiempo real del cálculo.

Esto no es un detalle menor. Las primeras mediciones de este trabajo, hechas
con una sola corrida y sin calentamiento, dieron 4.82 s para el trapecio
secuencial contra 1.72 s en la corrida siguiente — el mismo código, los mismos
datos, casi tres veces de diferencia. El arranque del proceso, las cachés frías
y el escalado de frecuencia del procesador dominaban la medición.

Los parámetros se pasan a través de variables `volatile`. Sin eso, `gcc -O2`
detecta que la función secuencial es pura y que las tres llamadas tienen los
mismos argumentos, calcula una sola vez y reporta tiempos de 0.00000 s.

## Resultados

Se ejecutó cada programa varias veces para estimar la dispersión:

| Corrida | Trapecio | Simpson 1/3 |
|---:|---:|---:|
| 1 | 1.426 | 1.786 |
| 2 | 1.766 | 1.905 |
| 3 | 2.171 | 1.721 |
| 4 | — | 1.878 |

**Speedup observado: 1.8 ± 0.3 en ambos métodos.** Los rangos (1.43–2.17 para
el trapecio, 1.72–1.91 para Simpson) se solapan por completo, de modo que los
datos no permiten afirmar que un método escale mejor que el otro.

Tiempos de una corrida representativa:

| Método | Secuencial | Paralelo (4 hilos) | Speedup | Eficiencia |
|:---|---:|---:|---:|---:|
| Trapecio | 1.27462 s | 0.89365 s | 1.43 | 71.3 % |
| Simpson 1/3 | 1.28983 s | 0.67708 s | 1.91 | 95.2 % |

La eficiencia está calculada sobre **2 núcleos físicos**, no sobre los 4 hilos
lógicos. Una de las corridas del trapecio superó el 100 % (108.6 %), lo que es
posible porque el Hyper-Threading permite aprovechar huecos del pipeline que un
solo hilo deja vacíos.

Los tiempos secuenciales de los dos métodos coinciden (1.275 s y 1.290 s), como
era de esperarse: ambos hacen el mismo número de evaluaciones de $\sin(x)$, que
es lo que domina el costo.

## Precisión

| Método | Resultado secuencial | Error | Resultado paralelo | Error |
|:---|---:|---:|---:|---:|
| Trapecio | 2.00000000000025 | 2.478×10⁻¹³ | 1.99999999999985 | 1.550×10⁻¹³ |
| Simpson 1/3 | 2.00000000000036 | 3.642×10⁻¹³ | 2.00000000000009 | 9.281×10⁻¹⁴ |

**No hay condiciones de carrera.** La diferencia entre la versión secuencial y
la paralela es de 4.028×10⁻¹³ (trapecio) y 2.713×10⁻¹³ (Simpson), del orden del
redondeo de máquina. Si `reduction` estuviera mal planteado y los hilos
escribieran sobre el mismo acumulador, la diferencia sería visible a simple
vista y cambiaría entre corridas. Aquí es idéntica en todas.

**La versión paralela resultó más exacta que la secuencial** en los dos
métodos. No es casualidad: `reduction` hace que cada hilo acumule su propia
suma parcial y al final combine cuatro números. Sumar 25 millones de términos
cuatro veces acumula menos error de redondeo que sumar 100 millones seguidos
sobre un mismo acumulador, porque el acumulador va creciendo y los sumandos
pequeños pierden bits al alinearse con él.

**Los dos métodos dieron errores equivalentes**, pese a que Simpson es de orden
$O(h^4)$ y el trapecio de $O(h^2)$. Con $n = 10^8$ el paso es
$h \approx 3.1\times10^{-8}$, de modo que $h^2 \approx 10^{-15}$: el error
teórico del trapecio ya quedó por debajo del redondeo de la máquina. Ninguno de
los dos está midiendo su propia fórmula — los dos tocaron el piso de la
aritmética de punto flotante. Para ver la ventaja de Simpson habría que
**reducir** $n$, no aumentarlo.

## Conclusiones

La paralelización con `#pragma omp parallel for reduction(+:...)` es correcta:
no introduce condiciones de carrera y los resultados coinciden con el valor
analítico hasta el límite de la aritmética.

El speedup de ~1.8 sobre 2 núcleos físicos corresponde a una eficiencia cercana
al 90 %. Lo que falta para el 100 % es el costo de crear y sincronizar los
hilos, que no desaparece. Los 4 "hilos" que reporta el sistema son lógicos: no
hay forma de obtener un speedup de 4 en esta máquina.

Los datos no permiten distinguir el comportamiento de los dos métodos bajo
paralelización. La variación entre corridas idénticas del mismo programa llega
al 25 %, mayor que cualquier diferencia entre ellos.

## Compilación

    gcc -fopenmp -O2 simpson.c  -lm -o simpson
    gcc -fopenmp -O2 trapecio.c -lm -o trapecio
    ./simpson
    ./trapecio