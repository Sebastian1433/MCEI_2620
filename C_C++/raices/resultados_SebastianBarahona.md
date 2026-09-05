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

Intervalo [−3, −2] para los métodos cerrados.

| Método         |          Raíz | Iteraciones |     f(raíz) |
|:---------------|--------------:|------------:|------------:|
| Bisección      | −2.3300587460 |          26 | −7.31×10⁻⁸  |
| False Position | −2.3300587396 |           8 | −1.78×10⁻¹⁵ |
| Brent          | −2.3300587396 |           6 |  7.11×10⁻¹⁵ |
