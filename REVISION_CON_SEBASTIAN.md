# Guía de revisión con Sebastián

La idea es que ambos puedan validar la entrega sin tener que reconstruir el trabajo.

## Revisar juntos

1. **Punto 1**
   - La matriz A es la del enunciado.
   - Octave y Eigen3 producen la misma pseudoinversa.
   - Se verifican las 4 propiedades de Moore-Penrose.

2. **Punto 2**
   - Confirmar que el enunciado realmente no contiene el vector b.
   - Preguntar al profesor antes de asumir datos.

3. **Punto 3**
   - Se usa Hilbert(5) como ejemplo de matriz mal condicionada.
   - cond(A) ≈ 4.77e5.
   - La perturbación de 1e-5 produce un error relativo ≈ 0.8426 %.

4. **Punto 4**
   - Se comparan inversa directa, QR y SVD.
   - No concluir que la inversa directa sea siempre más estable solamente porque en este caso tenga menor residuo.

5. **Punto 5**
   - CSV con 100 muestras.
   - Revisar modelo, MSE, residuos y cond(X).
   - Confirmar que la influencia relativa se compara con variables estandarizadas.
   - Resultado: Corriente > Voltaje > Temperatura en magnitud.

## Prueba rápida

Desde Ubuntu:

```bash
cd ~/MCEI_2620/C_C++/matrices/build
cmake ..
make
./matrices
./punto5
```

Después:

```bash
cd ~/MCEI_2620
octave
```

Y en Octave:

```octave
run("Octave/punto1_pseudoinversa.m")
run("Octave/punto3_condicionamiento.m")
run("Octave/punto4_inversa_qr_svd.m")
run("Octave/punto5_minimos_cuadrados.m")
```

Si todos los resultados coinciden con el informe, la entrega está lista salvo el Punto 2.
