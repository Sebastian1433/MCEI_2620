---
title: "Taller sobre diferenciación numérica"
subtitle: "Estimación de velocidad lineal y angular de un robot de cinemática diferencial"
author:
  - Jimmy Alexander Calderón Trujillo
  - Sebastián Barahona
date: "Métodos Computacionales en Ingeniería --- MCEI\\_M \\newline Universidad Escuela Colombiana de Ingeniería Julio Garavito"
lang: es
toc: true
toc-depth: 2
geometry: "margin=2.3cm"
mainfont: "DejaVu Serif"
sansfont: "DejaVu Sans"
monofont: "DejaVu Sans Mono"
fontsize: 10pt
colorlinks: false
header-includes: |
  \usepackage{fancyhdr}
  \pagestyle{fancy}
  \fancyhf{}
  \fancyhead[L]{\small Diferenciación numérica}
  \fancyhead[R]{\small Robot de cinemática diferencial}
  \fancyfoot[C]{\thepage}
  \renewcommand{\headrulewidth}{0.4pt}
  \usepackage{longtable}
  \usepackage{booktabs}
  \usepackage{float}
  \setlength{\emergencystretch}{3em}
---

## 1. Procedimiento y notación

La cadena de cálculo es la misma en los tres entornos:

$$(x, y, t) \;\longrightarrow\; (\dot{x}, \dot{y}) \;\longrightarrow\; (v, \theta) \;\longrightarrow\; \omega$$

con

$$v = \sqrt{\dot{x}^2 + \dot{y}^2}, \qquad \theta = \operatorname{atan2}(\dot{y}, \dot{x}), \qquad \omega = \dot{\theta}$$

Hay **dos diferenciaciones encadenadas**: una para pasar de posición a velocidad y otra para pasar de orientación a velocidad angular. Esa estructura explica casi todo lo que se observa después.

### Esquemas utilizados

| Esquema | Fórmula | Orden |
|:---|:---|:---:|
| Hacia adelante | $(f_{i+1}-f_i)/h$ | $O(h)$ |
| Hacia atrás | $(f_i-f_{i-1})/h$ | $O(h)$ |
| Centrada | $(f_{i+1}-f_{i-1})/(2h)$ | $O(h^2)$ |

En el interior se usa siempre la diferencia centrada. En los extremos, Octave y C++ emplean fórmulas unilaterales **de tres puntos**, también $O(h^2)$:

$$f'(t_0) \approx \frac{-3f_0 + 4f_1 - f_2}{2h}, \qquad f'(t_{n-1}) \approx \frac{3f_{n-1} - 4f_{n-2} + f_{n-3}}{2h}$$

La razón de no usar las de dos puntos se desarrolla en la sección 6.2.

---

## 2. Datos del robot

51 muestras equiespaciadas, $t_i = 0.2\,i$ con $i = 0,\dots,50$:

$$x(t) = 0.08\,t^2 + 0.40\sin(0.45\,t), \qquad y(t) = 0.50\,t + 0.30\left(1 - \cos(0.45\,t)\right)$$

| Magnitud | Rango |
|:---|:---|
| $t$ | 0 a 10 s, $h$ = 0.2 s |
| $x$ | 0 a 7.6090 m |
| $y$ | 0 a 5.3632 m |

Como la trayectoria es analítica, se derivó a mano para disponer de una **referencia exacta** contra la cual medir el error de cada método:

$$\dot{x} = 0.16t + 0.18\cos(0.45t), \qquad \dot{y} = 0.50 + 0.135\sin(0.45t)$$

$$\ddot{x} = 0.16 - 0.081\sin(0.45t), \qquad \ddot{y} = 0.06075\cos(0.45t)$$

y la velocidad angular exacta, obtenida al derivar $\operatorname{atan2}(\dot y, \dot x)$:

$$\omega_{\text{exacta}} = \frac{\dot{x}\,\ddot{y} - \dot{y}\,\ddot{x}}{\dot{x}^2 + \dot{y}^2}$$

Esto permite reportar errores absolutos reales y no solo discrepancias entre implementaciones.

---

## 3. Resultados

Los valores obtenidos, idénticos en los tres entornos salvo en los extremos:

| Magnitud | Rango calculado |
|:---|:---|
| $v$ | 0.5316 a 1.6046 m/s |
| $\theta$ | 0.2312 a 1.2244 rad |
| $\omega$ | −0.2339 a −0.0441 rad/s |

La velocidad angular es negativa en todo el recorrido: el robot gira de forma sostenida en el mismo sentido, coherente con una trayectoria que se curva hacia la derecha.

### 3.1 Error frente a la solución analítica

| Magnitud | Error interior | Error global |
|:---|---:|---:|
| $\dot{x}$ | 2.429×10⁻⁴ | 4.846×10⁻⁴ |
| $v$ | 2.344×10⁻⁴ | 2.344×10⁻⁴ |
| $\theta$ | 3.925×10⁻⁴ | 8.421×10⁻⁴ |
| $\omega$ | 2.317×10⁻³ | 1.071×10⁻² |

El error de $\omega$ es un orden de magnitud mayor que el de $v$. No es casualidad y se analiza en la sección 6.2.

### 3.2 Comparación de esquemas

Error máximo en $\dot{x}$ con $h$ = 0.2 s:

| Esquema | Error máximo | Relación |
|:---|---:|---:|
| Hacia adelante | 2.380×10⁻² | 98× |
| Hacia atrás | 2.386×10⁻² | 98× |
| **Centrada** | **2.429×10⁻⁴** | 1× |

Casi cien veces mejor con el mismo número de datos y el mismo costo computacional. La diferencia centrada no requiere más operaciones que las unilaterales: la ganancia proviene de la cancelación del término de primer orden en la serie de Taylor.

### 3.3 Verificación del orden $O(h^2)$

| $h$ [s] | Error máximo en $\dot{x}$ | Razón |
|---:|---:|---:|
| 0.400 | 9.672×10⁻⁴ | — |
| 0.200 | 2.429×10⁻⁴ | 3.98 |
| 0.100 | 6.074×10⁻⁵ | 4.00 |
| 0.050 | 1.519×10⁻⁵ | 4.00 |
| 0.025 | 3.797×10⁻⁶ | 4.00 |

Al dividir $h$ entre dos el error se divide entre cuatro. La razón converge a 4.00 con tres cifras, que es la confirmación numérica del orden cuadrático.

---