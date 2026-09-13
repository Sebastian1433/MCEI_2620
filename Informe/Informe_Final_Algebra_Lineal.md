# Taller sobre Álgebra Lineal Numérica

**Métodos Computacionales en Ingeniería — MCEI**
Escuela Colombiana de Ingeniería Julio Garavito

**Integrantes:**

- Jimmy Alexander Calderón Trujillo
- Sebastián Barahona

> **Nota metodológica.** En el Punto 2 el enunciado suministra la matriz A de
> 10×10, pero no especifica el vector independiente b. Para completar la
> comparación numérica se asumió explícitamente
> **b = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]ᵀ**.

---

## 1. Punto 1 – Pseudoinversa de Moore-Penrose

Para la matriz A = [[1,0,2],[2,-1,5],[0,1,-1],[1,3,-1]], se calculó la pseudoinversa en GNU Octave mediante pinv(A) y en C++/Eigen3 mediante descomposición SVD.

A⁺ ≈ [[0.0512821, 0.0769231, 0.0256410, 0.1282051], [0.0435897, 0.0153846, 0.0717949, 0.2589744], [0.0589744, 0.1384615, -0.0205128, -0.0025641]].

Se verificaron las cuatro propiedades de Moore-Penrose. En Octave los errores fueron 2.50×10⁻¹⁵, 2.15×10⁻¹⁶, 3.91×10⁻¹⁶ y 3.70×10⁻¹⁶, respectivamente. Estos valores son compatibles con el redondeo de punto flotante.

---

## 2. Punto 2 – Sistema lineal 10×10: Gauss, LU y QR

Para completar el sistema se asumió b = [1,2,3,4,5,6,7,8,9,10]ᵀ. En C++ se implementó eliminación gaussiana con pivoteo parcial y se emplearon las factorizaciones LU y QR de Eigen3. En Octave se compararon A\b, LU y QR.

Los tres métodos produjeron la misma solución, dentro de la precisión numérica:
x ≈ [-0.5402100241, -4.1975720146, -2.1442633550, -4.0005796843, 3.1458128957, 1.3559261571, 3.5632413270, -2.4104833675, 3.1812739677, 3.0679679836]ᵀ.

| Método      | Residuo C++ | Residuo Octave |
|-------------|-------------|----------------|
| Gauss / A\b | 8.57×10⁻¹⁵  | 6.40×10⁻¹⁵     |
| LU          | 1.29×10⁻¹⁴  | 6.40×10⁻¹⁵     |
| QR          | 2.12×10⁻¹⁴  | 1.56×10⁻¹⁴     |

El número de condición fue κ(A) ≈ 51.715, correspondiente a un condicionamiento moderado. Los tiempos de una única ejecución de una matriz 10×10 no deben interpretarse como una comparación universal. LU es especialmente conveniente cuando se reutiliza la misma matriz con varios vectores b, mientras que QR ofrece buenas propiedades de estabilidad numérica.

---

## 3. Punto 3 – Condicionamiento y estabilidad numérica

Se utilizó una matriz de Hilbert de orden 5 como ejemplo de matriz mal condicionada. Se obtuvo cond(A) = 4.7661×10⁵. Tomando x = [1,1,1,1,1]ᵀ y perturbando únicamente la primera componente de b en 10⁻⁵, el error relativo de la solución fue 8.4262×10⁻³, equivalente a 0.842618 %.

El resultado evidencia que una perturbación pequeña en los datos puede producir una variación apreciable en la solución cuando la matriz está fuertemente mal condicionada.

---

## 4. Punto 4 – Inversa mediante método directo, QR y SVD

Para la matriz de Hilbert de orden 5 se calculó la inversa mediante inv(A), factorización QR y descomposición SVD. En Octave los errores ||AA⁻¹−I|| fueron aproximadamente 6.20×10⁻¹² (directa), 2.74×10⁻¹¹ (QR) y 3.22×10⁻¹¹ (SVD). En C++/Eigen3 se obtuvieron valores del mismo orden.

Que la inversión directa presente el menor residuo en esta prueba particular no significa que sea universalmente el método más estable. QR y SVD son herramientas importantes cuando aparecen problemas mal condicionados o cercanos a singularidad.

---

## 5. Punto 5 – Mínimos cuadrados: convertidor DC-DC

Con 100 muestras de voltaje V, corriente I, temperatura T y potencia P se ajustó el modelo P = β₀ + β₁V + β₂I + β₃T mediante mínimos cuadrados. Octave y C++/Eigen3 produjeron coeficientes coincidentes dentro de la precisión numérica.

| Coeficiente | Valor       | Interpretación                                    |
|-------------|-------------|---------------------------------------------------|
| β₀          | -123.418350 | Intercepto matemático                             |
| β₁          | 6.322569    | Cambio estimado en P por +1 V, manteniendo I y T  |
| β₂          | 19.042068   | Cambio estimado en P por +1 A, manteniendo V y T  |
| β₃          | -0.206825   | Cambio estimado en P por +1 °C, manteniendo V e I |

Modelo obtenido:
P = -123.418350 + 6.322569V + 19.042068I - 0.206825T.

Se calculó el vector residual completo r = y - Xβ̂ para las 100 observaciones. Los indicadores resumidos fueron ||r||₂ = 155.040807, MSE = 240.376517 y residuo absoluto máximo = 47.957115. El número de condición de X fue 347.216931. La diferencia entre los coeficientes obtenidos mediante QR y ecuaciones normales fue del orden de 10⁻¹².

Al estandarizar las variables explicativas se obtuvieron magnitudes: Voltaje = 37.617343, Corriente = 55.813528 y Temperatura = -3.034362. Por magnitud absoluta, la corriente presenta la mayor influencia relativa, seguida del voltaje y finalmente la temperatura.

---

## 6. Respuestas al análisis del ejercicio práctico

**1. Variable de mayor impacto:** la corriente, según los coeficientes obtenidos con las variables explicativas estandarizadas.

**2. Influencia de la temperatura:** es negativa y pequeña en el rango analizado; β₃ ≈ -0.206825 W/°C manteniendo las demás variables constantes.

**3. Aumento del número de muestras:** si las nuevas muestras son representativas y mantienen las condiciones del modelo, las estimaciones tienden a ganar precisión y estabilidad.

**4. ¿Por qué QR frente a ecuaciones normales?:** QR evita formar explícitamente XᵀX; en norma 2 y con rango completo, el condicionamiento de XᵀX es aproximadamente el cuadrado del de X, por lo que las ecuaciones normales pueden amplificar problemas numéricos.

**5. Interpretación física:** β₁, β₂ y β₃ representan cambios marginales estimados de potencia frente a V, I y T manteniendo las otras variables constantes; β₀ es el intercepto y puede no tener significado físico directo fuera del rango de datos.

---

## 7. Conclusiones

- Octave y Eigen3 entregaron resultados consistentes en los ejercicios desarrollados.

- Las cuatro propiedades de la pseudoinversa de Moore-Penrose se verificaron con errores cercanos a la precisión de máquina.

- Gauss, LU y QR resolvieron consistentemente el sistema 10×10 asumido, con residuos del orden de 10⁻¹⁴ a 10⁻¹⁵.

- La matriz de Hilbert mostró cómo un número de condición elevado incrementa la sensibilidad de la solución frente a perturbaciones.

- En el modelo del convertidor DC-DC, la corriente fue la variable de mayor influencia relativa y QR proporcionó una formulación numéricamente recomendable para mínimos cuadrados.

---

## 8. Archivos de entrega

La entrega se complementa con los códigos C++/Eigen3, los scripts de GNU Octave, el archivo CMakeLists.txt y el CSV de 100 muestras empleados para reproducir los resultados.
