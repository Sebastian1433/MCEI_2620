# Taller de Álgebra Lineal Numérica - MCEI

**Integrantes:**  
- Jimmy Alexander Calderón Trujillo  
- Sebastián [completar apellido]

## Estado de la entrega

| Punto | Octave | C++ / Eigen3 | Informe |
|---|---|---|---|
| 1. Pseudoinversa de Moore-Penrose | Completo | Completo | Completo |
| 2. Sistema 10x10 | Pendiente de vector b | Avance preparado | Pendiente |
| 3. Condicionamiento y estabilidad | Completo | Completo | Completo |
| 4. Inversa directa / QR / SVD | Completo | Completo | Completo |
| 5. Mínimos cuadrados | Completo | Completo | Completo |

> El Punto 2 se mantiene pendiente porque el enunciado entrega la matriz A pero no especifica el vector independiente b.

## Archivos principales

### C++ / Eigen3
- `C_C++/matrices/main_jimmy_sebastian.cpp`
  - Punto 1
  - Punto 3
  - Punto 4
- `C_C++/matrices/punto2_sistema_10x10.cpp`
  - Avance del Punto 2
- `C_C++/matrices/punto5_minimos_cuadrados.cpp`
  - Punto 5
- `C_C++/matrices/datos_convertidor_realista.csv`
  - 100 muestras del convertidor
- `C_C++/matrices/CMakeLists.txt`

### Octave
- `Octave/punto1_pseudoinversa.m`
- `Octave/punto3_condicionamiento.m`
- `Octave/punto4_inversa_qr_svd.m`
- `Octave/punto5_minimos_cuadrados.m`

## Cómo ejecutar C++

Desde Ubuntu/WSL:

```bash
cd ~/MCEI_2620/C_C++/matrices/build
cmake ..
make
./matrices
./punto5
```

El ejecutable `./punto2` corresponde al avance del Punto 2.

## Cómo ejecutar Octave

Desde la raíz del repositorio:

```bash
cd ~/MCEI_2620
octave
```

Dentro de Octave:

```octave
run("Octave/punto1_pseudoinversa.m")
run("Octave/punto3_condicionamiento.m")
run("Octave/punto4_inversa_qr_svd.m")
run("Octave/punto5_minimos_cuadrados.m")
```

## Resultados principales

### Punto 1
La pseudoinversa obtenida coincide entre Octave y Eigen3 y satisface las cuatro propiedades de Moore-Penrose dentro de la precisión numérica.

### Punto 3
- `cond(A) ≈ 4.7661e5`
- Perturbación en `b`: `1e-5`
- Error relativo de la solución: `≈ 8.4262e-3 = 0.84262 %`

### Punto 4
Residuos de reconstrucción del orden de `1e-11` a `1e-12` para inversa directa, QR y SVD.

### Punto 5
Modelo estimado:

```text
P = -123.418 + 6.32257 V + 19.0421 I - 0.206825 T
```

Indicadores:
- 100 muestras
- Norma de residuos: `≈ 155.041`
- MSE: `≈ 240.377`
- `cond(X) ≈ 347.217`
- Diferencia QR vs ecuaciones normales: `≈ 3.88e-12`

Influencia relativa con variables estandarizadas:
- Corriente: `55.813528`
- Voltaje: `37.617343`
- Temperatura: `-3.034362`

Por magnitud, la corriente es la variable de mayor influencia relativa, seguida por el voltaje y finalmente la temperatura.

## Pendientes antes de entregar al profesor
1. Confirmar con el profesor el vector `b` del Punto 2.
2. Completar el apellido de Sebastián en el informe y en este README.
3. Incorporar cualquier observación de Sebastián.
4. Ejecutar una última prueba de todos los programas.
5. Hacer commit, push y Pull Request final.
