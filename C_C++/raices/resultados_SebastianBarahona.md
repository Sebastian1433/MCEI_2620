# Búsqueda de raíces con GNU Scientific Library

**Autor:** Sebastián Barahona
**Curso:** Métodos Computacionales para Ingeniería (MCEI_M)
**Universidad Escuela Colombiana de Ingeniería Julio Garavito**
**Fecha:** 4 de septiembre de 2026

---

## 1. Planteamiento

Se busca resolver f(x) = 0 para **f(x) = x³ − 5x + 1**.

Es una cúbica con tres raíces reales. Su derivada f'(x) = 3x² − 5 se anula en
x = ±√(5/3) ≈ ±1.2910, que son los puntos críticos donde los métodos abiertos
pueden fallar.

Evaluando la función en enteros consecutivos se localizan tres intervalos con
cambio de signo, uno por raíz:

| Intervalo  |   f(a) |   f(b) | Cambio de signo |
|:-----------|-------:|-------:|:---------------:|
| [−3, −2]   |    −11 |     +3 | Sí              |
| [ 0,  1]   |     +1 |     −3 | Sí              |
| [ 2,  3]   |     −1 |    +13 | Sí              |

---

## 2. Implementación

Los seis métodos pertenecen a dos familias de la GSL con APIs distintas:

| Familia                | Métodos                            | Entrada          | Requiere f'(x) |
|:-----------------------|:-----------------------------------|:-----------------|:--------------:|
| `gsl_root_fsolver`     | Bisección, False Position, Brent   | intervalo [a, b] | No             |
| `gsl_root_fdfsolver`   | Newton, Secante, Steffenson        | punto inicial x₀ | Sí             |

**Criterios de parada**

| Familia  | Función GSL              | Mide                | Tolerancia |
|:---------|:-------------------------|:--------------------|-----------:|
| Cerrada  | `gsl_root_test_interval` | ancho del intervalo |      10⁻⁸ |
| Abierta  | `gsl_root_test_delta`    | \|xₙ − xₙ₋₁\|       |      10⁻⁸ |

Tope de 100 iteraciones en ambos casos.

La columna **f(raíz)** de las tablas siguientes es una verificación
independiente: si la raíz es correcta, evaluar la función en ella debe dar
prácticamente cero.

---

## 3. Resultados — raíz negativa (≈ −2.33)

Métodos cerrados

Intervalo [−3, −2] para los métodos cerrados.

| Método         |          Raíz | Iteraciones |     f(raíz) |
|:---------------|--------------:|------------:|------------:|
| Bisección      | −2.3300587460 |          26 | −7.31×10⁻⁸  |
| False Position | −2.3300587396 |           8 | −1.78×10⁻¹⁵ |
| Brent          | −2.3300587396 |           6 |  7.11×10⁻¹⁵ |

Métodos abiertos

| Newton         | −2.3300587396 |           4 | −1.78×10⁻¹⁵ |
| Secante        | −2.3300587396 |           5 | −1.27×10⁻¹² |
| Steffenson     | −2.3300587396 |           5 | −1.78×10⁻¹⁵ |

Los métodos abiertos parten de x₀ = −2.5.


---

## 4. Resultados — raíz central (≈ 0.20)

Intervalo [0, 1] para los cerrados, x₀ = 0.5 para los abiertos.

| Método         |         Raíz | Iteraciones |     f(raíz) |
|:---------------|-------------:|------------:|------------:|
| Bisección      | 0.2016396755 |          29 |  9.18×10⁻¹⁰ |
| False Position | 0.2016396757 |           8 |  1.11×10⁻¹⁶ |
| Brent          | 0.2016396757 |           6 |  1.11×10⁻¹⁶ |
| Newton         | 0.2016396757 |           4 |  1.11×10⁻¹⁶ |
| Secante        | 0.2016396757 |           5 | −2.00×10⁻¹⁵ |
| Steffenson     | 0.2016396757 |           5 |  1.11×10⁻¹⁶ |


---

## 5. Resultados — raíz positiva (≈ 2.13)

Intervalo [2, 3] para los cerrados, x₀ = 2.5 para los abiertos.

| Método         |         Raíz | Iteraciones |     f(raíz) |
|:---------------|-------------:|------------:|------------:|
| Bisección      | 2.1284190640 |          26 |  1.21×10⁻⁹  |
| False Position | 2.1284190638 |      **25** | −1.78×10⁻¹⁵ |
| Brent          | 2.1284190638 |           6 | −1.78×10⁻¹⁵ |
| Newton         | 2.1284190638 |           5 | −1.78×10⁻¹⁵ |
| Secante        | 2.1284190638 |           6 |  3.16×10⁻¹³ |
| Steffenson     | 2.1284190638 |           6 | −1.78×10⁻¹⁵ |

False Position necesitó 25 iteraciones aquí, contra 8 en las dos raíces
anteriores. Se analiza en la sección 6.


---

## 6. Resumen comparativo

### 6.1 Raíces obtenidas

| Raíz     | Intervalo | x₀ métodos abiertos |         Valor |
|:---------|:---------:|:-------------------:|--------------:|
| Negativa | [−3, −2]  |                −2.5 | −2.3300587396 |
| Central  | [ 0,  1]  |                 0.5 |  0.2016396757 |
| Positiva | [ 2,  3]  |                 2.5 |  2.1284190638 |

Los seis métodos coinciden en las tres raíces hasta el décimo decimal,
salvo la bisección (ver 6.3).

### 6.2 Iteraciones por método y por raíz

| Método         | Familia | Negativa | Central | Positiva | Promedio |
|:---------------|:-------:|---------:|--------:|---------:|---------:|
| Bisección      | Cerrada |       26 |      29 |       26 |     27.0 |
| False Position | Cerrada |        8 |       8 |       25 |     13.7 |
| Brent          | Cerrada |        6 |       6 |        6 |  **6.0** |
| Newton         | Abierta |        4 |       4 |        5 |  **4.3** |
| Secante        | Abierta |        5 |       5 |        6 |      5.3 |
| Steffenson     | Abierta |        5 |       5 |        6 |      5.3 |

Newton es el más rápido en promedio; Brent es el más **estable**: idéntico
conteo en las tres raíces, sin depender de la geometría del intervalo.

### 6.3 Precisión: valor de f(raíz)

| Método         |    Negativa |     Central |    Positiva | Orden típico |
|:---------------|------------:|------------:|------------:|:------------:|
| Bisección      | −7.31×10⁻⁸  |  9.18×10⁻¹⁰ |  1.21×10⁻⁹  |     10⁻⁹     |
| False Position | −1.78×10⁻¹⁵ |  1.11×10⁻¹⁶ | −1.78×10⁻¹⁵ |     10⁻¹⁵    |
| Brent          |  7.11×10⁻¹⁵ |  1.11×10⁻¹⁶ | −1.78×10⁻¹⁵ |     10⁻¹⁵    |
| Newton         | −1.78×10⁻¹⁵ |  1.11×10⁻¹⁶ | −1.78×10⁻¹⁵ |     10⁻¹⁵    |
| Secante        | −1.27×10⁻¹² | −2.00×10⁻¹⁵ |  3.16×10⁻¹³ |     10⁻¹³    |
| Steffenson     | −1.78×10⁻¹⁵ |  1.11×10⁻¹⁶ | −1.78×10⁻¹⁵ |     10⁻¹⁵    |

### 6.4 Degradación de False Position

False Position pasa de 8 iteraciones en las dos primeras raíces a 25 en la
tercera. Es su patología conocida: cuando la función es marcadamente convexa
dentro del intervalo, uno de los extremos nunca se actualiza y la reducción
ocurre por un solo lado, degradando el método hasta un desempeño comparable
al de la bisección.

Brent no lo sufre porque intercala pasos de bisección cuando detecta que la
interpolación no progresa. Esa es la razón del 6.0 constante en la tabla 6.2.

### 6.5 Precisión de la bisección

La bisección es la única que arrastra error visible en f(raíz): del orden de
10⁻⁹, frente a 10⁻¹⁵ del resto. Seis órdenes de magnitud.

La causa es su criterio de parada. `gsl_root_test_interval` mide el **ancho
del intervalo**, no la cercanía real a la raíz: cuando el ancho baja de 10⁻⁸
el método se detiene, pero el punto medio reportado puede estar hasta 10⁻⁸
lejos de la raíz verdadera, y f amplifica ese error.


---

## 7. Sensibilidad al valor inicial

Se repitieron los métodos abiertos variando x₀, incluyendo un punto muy
cercano al crítico x = 1.2910, donde f'(x) = 0. Las filas están ordenadas por
|f'(x₀)| creciente:

| Método     |     x₀ |  f'(x₀) | Distancia a la raíz | Resultado                | Iteraciones |
|:-----------|-------:|--------:|--------------------:|:-------------------------|------------:|
| Newton     | 1.2909 |  ≈ 0.00 |                1.09 | −2.3301 (raíz **lejana**) |          25 |
| Steffenson | 1.2909 |  ≈ 0.00 |                1.09 | −2.3301 (raíz **lejana**) |          25 |
| Secante    | 1.2909 |  ≈ 0.00 |                1.09 | −5024.31 (**divergió**)   |   100 (tope) |
| Newton     | 1.0000 |   −2.00 |                0.80 | 0.2016 (raíz cercana)     |           6 |
| Newton     | 0.5000 |   −4.25 |                0.30 | 0.2016 (raíz cercana)     |           4 |
| Newton     |     10 |    +295 |                7.87 | 2.1284                    |           9 |
| Secante    |     10 |    +295 |                7.87 | 2.1284                    |          12 |

### Interpretación

El resultado clave es que **la distancia a la raíz no predice el fracaso**.
Desde x₀ = 10, a casi 8 unidades de cualquier raíz, Newton converge sin
problema en 9 iteraciones. Desde x₀ = 1.2909, a poco más de una unidad de la
raíz central, la secante diverge hasta −5024.

Lo que gobierna el comportamiento es f'(x₀). La corrección de Newton

$$x_{n+1} = x_n - \frac{f(x_n)}{f'(x_n)}$$

divide por la derivada: cuando f'(x₀) → 0 el paso se dispara y la iteración
salta a una región arbitraria de la función. Las tres primeras filas de la
tabla lo muestran en orden — con |f'(x₀)| de 4.25 a 2.00 el costo sube de 4 a
6 iteraciones, y al llegar a ≈ 0 el método pierde la raíz de vista por
completo.

Los métodos cerrados son indiferentes a esto: entregan siempre la raíz
contenida en el intervalo, sin importar la geometría local.


