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

### 3.4 Gráficas

![Trayectoria del robot en el plano. El punto de inicio está en el origen.](../../Python/figuras/python_trayectoria.png){width=72%}

![Velocidad lineal. Las curvas numérica y analítica se superponen en toda la trayectoria.](../../Python/figuras/python_velocidad.png){width=92%}

![Velocidad angular. La curva numérica se despega de la analítica únicamente en los dos extremos, donde las fórmulas unilaterales de `np.gradient` son de orden $O(h)$.](../../Python/figuras/python_omega.png){width=92%}

![Convergencia de la diferencia centrada. Con datos limpios la pendiente es 2 en escala log-log, confirmando $O(h^2)$; con ruido de 10⁻³ m el error crece al refinar $h$.](../../Python/figuras/python_convergencia.png){width=92%}

\clearpage

## 4. Implementaciones

### 4.1 Parte 1 — GNU Octave

Carga con `csvread`, diferencias centradas vectorizadas sobre el interior y fórmulas de tres puntos en los extremos:

```octave
d(2:n-1) = (f(3:n) - f(1:n-2)) / (2*h);         % interior, vectorizado
d(1)     = (-3*f(1) + 4*f(2) - f(3)) / (2*h);   % adelante, 3 puntos
d(n)     = ( 3*f(n) - 4*f(n-1) + f(n-2)) / (2*h);
```

El desenvolvimiento angular usa la función `unwrap` incorporada. Se generan las cuatro gráficas pedidas: $y$ vs $x$, $v$ vs $t$, $\theta$ vs $t$ y $\omega$ vs $t$.

### 4.2 Parte 2 — Python con NumPy

```python
vx = np.gradient(x, t)
vy = np.gradient(y, t)
v  = np.sqrt(vx**2 + vy**2)
theta = np.unwrap(np.arctan2(vy, vx))
omega = np.gradient(theta, t)
```

**Qué operación reemplaza las diferencias explícitas.** `np.gradient` sustituye por completo el bucle: aplica diferencias centradas en el interior en una sola llamada vectorizada, y acepta el vector de tiempos como segundo argumento, de modo que funciona también con muestreo no uniforme.

**Cómo trata los extremos.** Aquí está la diferencia importante con las otras dos implementaciones: `np.gradient` usa fórmulas unilaterales **de dos puntos**, de orden $O(h)$. Por eso sus valores en $i=0$ e $i=n-1$ son menos exactos. Se puede pedir el comportamiento de segundo orden con `edge_order=2`.

### 4.3 Parte 3 — C/C++ con GSL

Lectura manual del CSV, diferencias centradas sobre los arreglos, y desenvolvimiento angular implementado explícitamente acumulando múltiplos de $2\pi$:

```cpp
for (size_t i = 1; i < th.size(); ++i) {
    double d = th[i] - th[i-1];
    if (d >  M_PI) correccion -= 2.0*M_PI;
    if (d < -M_PI) correccion += 2.0*M_PI;
    u[i] = th[i] + correccion;
}
```

**Exploración de `gsl_deriv_central`.** La rutina se aplicó a $x(t)$ como función evaluable, para contrastarla con la diferencia sobre datos tabulados:

| $t$ [s] | `gsl_deriv_central` | Error GSL | Error tabulado |
|---:|---:|---:|---:|
| 2.0 | 4.318898×10⁻¹ | 7.135×10⁻¹² | 1.510×10⁻⁴ |
| 4.0 | 5.991036×10⁻¹ | 3.015×10⁻¹¹ | 5.519×10⁻⁵ |
| 6.0 | 7.972670×10⁻¹ | 4.432×10⁻¹¹ | 2.196×10⁻⁴ |
| 8.0 | 1.118583×10⁰ | 2.184×10⁻¹¹ | 2.178×10⁻⁴ |

**Por qué los datos tabulados exigen otra estrategia.** Siete órdenes de magnitud separan los dos errores, y la causa no es que un algoritmo sea mejor que el otro: es que resuelven problemas distintos.

`gsl_deriv_central` recibe un puntero a función, así que puede **evaluar $f$ en cualquier punto**. Eso le permite escoger su propio paso, refinarlo, usar extrapolación de Richardson y hasta devolver una estimación del error.

Con 51 muestras fijas no existe $f(t)$ fuera de la malla. El paso lo impone el muestreo y no se puede reducir: la única información disponible son esos 51 pares. Por eso las diferencias se aplican directamente sobre los arreglos, y GSL queda disponible para otras operaciones numéricas del programa.

Es la distinción didáctica central del taller: **diferenciar una función no es lo mismo que diferenciar una colección de datos muestreados**.

---

## 5. Comparación de resultados

| Criterio | Octave | Python | C/C++ + GSL |
|:---|:---|:---|:---|
| Carga de datos | `csvread`, una línea | `np.loadtxt`, una línea | ~20 líneas con `ifstream` y `stringstream` |
| Cálculo de $\dot x,\dot y$ | Rebanadas vectorizadas | `np.gradient(x, t)` | Bucle explícito sobre el arreglo |
| Cálculo de $v$ | `sqrt(xdot.^2+ydot.^2)` | `np.hypot(vx, vy)` | Bucle con `std::sqrt` |
| Cálculo de $\theta$ | `unwrap(atan2(...))` | `np.unwrap(np.arctan2(...))` | `std::atan2` + unwrap propio |
| Cálculo de $\omega$ | Misma función de derivada | `np.gradient(theta, t)` | Misma función de derivada |
| Manejo de arreglos | Nativo, indexado desde 1 | Nativo, vistas sin copia | `std::vector`, gestión manual |
| Facilidad de implementación | Alta | Alta | Baja: I/O y unwrap a mano |
| Control sobre el algoritmo | Medio | Bajo con `gradient`; alto si se escribe a mano | Total: cada operación es explícita |
| Tiempo de ejecución | 1.47 s | 1.28 s | **0.003 s** |

Sobre la última fila: los tiempos de Octave y Python corresponden al programa completo, que incluye el arranque del intérprete, la generación de gráficas y un estudio de ruido por Monte Carlo. El núcleo de cálculo en Python son 97 µs. La comparación justa no es 1.28 s contra 0.003 s, pero el orden de magnitud a favor de C++ se mantiene en cualquier medición.

### 5.1 Coincidencia entre entornos

| Columna | Octave vs C++ | Octave vs Python | Python vs C++ |
|:---|---:|---:|---:|
| $\dot x$ | **0.00** | 2.37×10⁻² | 2.37×10⁻² |
| $v$ | **0.00** | 2.27×10⁻² | 2.27×10⁻² |
| $\theta$ | **0.00** | 2.27×10⁻² | 2.27×10⁻² |
| $\omega$ | **0.00** | 1.25×10⁻¹ | 1.25×10⁻¹ |

Octave y C++ coinciden **bit a bit**, porque implementan exactamente las mismas fórmulas, incluidas las de los extremos. Python difiere, pero solo en los bordes: restringido al interior, $v$ coincide también con cero absoluto en los tres entornos.

La discrepancia no mide calidad de implementación sino una decisión de diseño distinta en `np.gradient`.

---

## 6. Análisis

### 6.1 ¿Qué ocurre con el error en los extremos del arreglo?

En los extremos no se puede aplicar la diferencia centrada porque falta un vecino: en $i=0$ no existe $f_{-1}$ y en $i=n-1$ no existe $f_n$. Hay que usar una fórmula unilateral, y ahí se decide el orden del error.

La de dos puntos, $(f_1-f_0)/h$, es $O(h)$. La de tres puntos, $(-3f_0+4f_1-f_2)/(2h)$, recupera $O(h^2)$. En Octave y C++ se usó la de tres puntos; `np.gradient` en Python usa la de dos por defecto, y de ahí viene toda la discrepancia de la sección 5.1.

**El error del borde se propaga hacia adentro al derivar dos veces.** $\omega$ no se calcula de los datos crudos sino de $\theta$, que ya trae el error de borde en su primer punto. Al volver a derivar, ese punto contaminado entra en la fórmula de $i=1$:

| Índice | Error en $\omega$ |
|---:|---:|
| 0 | 1.357×10⁻¹ |
| 1 | 5.905×10⁻² |
| 2 | 8.354×10⁻⁴ |
| 3 | 7.829×10⁻⁴ |

El punto $i=1$ es interior y aun así su error es **70 veces** mayor que el de $i=2$. A partir de $i=2$ la curva se estabiliza: 8.354×10⁻⁴ y 7.829×10⁻⁴ son prácticamente el mismo valor. El deterioro está confinado a los dos primeros puntos, no se diluye gradualmente.

Restringido a $i = 2 \dots n-3$, el error de $\omega$ baja a 8.354×10⁻⁴, dos órdenes de magnitud por debajo del borde. Para el robot esto significa que $\omega$ al arrancar y al terminar la trayectoria no es confiable, que es justo donde el control necesitaría más cuidado.

### 6.2 ¿Cómo afecta el ruido en los datos a la derivada numérica?

Se agregó ruido gaussiano de desviación $\sigma$ a las posiciones, con $h = 0.2$ s fijo:

| $\sigma$ [m] | Error en $v$ | Error en $\omega$ |
|---:|---:|---:|
| 0 | 2.344×10⁻⁴ | 5.905×10⁻² |
| 10⁻⁴ | 9.818×10⁻⁴ | 5.914×10⁻² |
| 10⁻³ | 8.598×10⁻³ | 7.097×10⁻² |
| 10⁻² | 8.795×10⁻² | 5.263×10⁻¹ |

**La velocidad lineal responde proporcionalmente al ruido.** Cada vez que $\sigma$ se multiplica por 10, el error de $v$ también: 9.8×10⁻⁴ → 8.6×10⁻³ → 8.8×10⁻². Esto se sigue directamente de la fórmula. La diferencia centrada es

$$\frac{f_{i+1}-f_{i-1}}{2h}$$

y si cada $f$ trae un error del orden de $\sigma$, el numerador acumula $\sqrt{2}\,\sigma$ y se divide por $2h$. El error queda del orden de $\sigma/(\sqrt{2}\,h)$: **lineal en $\sigma$, amplificado por $1/h$**.

**La velocidad angular se comporta distinto, y es el hallazgo interesante.** Entre $\sigma = 0$ y $\sigma = 10^{-3}$ apenas cambia: 5.905×10⁻² contra 7.097×10⁻². Eso ocurre porque $\omega$ ya arrastra un error que no viene del ruido sino del borde (sección 6.1). Ese valor actúa como un **piso de truncamiento**, y mientras el ruido aporte menos que eso, no se nota. (El piso de 5.905×10⁻² corresponde al punto $i=1$: el estudio de ruido excluye los dos extremos, donde el error de borde es aún mayor.)

El aporte del ruido a $\omega$ es del orden de $\sigma/h^2$, porque se deriva dos veces. Con $h = 0.2$ eso da $25\sigma$:

- $\sigma = 10^{-3} \Rightarrow 2.5\times10^{-2}$, todavía por debajo del piso de 5.9×10⁻². El ruido queda escondido.
- $\sigma = 10^{-2} \Rightarrow 2.5\times10^{-1}$, ya por encima. Y en efecto el error observado salta a 5.26×10⁻¹.

Hay entonces un umbral: por debajo de $\sigma \approx 10^{-3}$ el error de $\omega$ lo manda la fórmula de borde, y por encima lo manda el sensor. Mejorar el sensor por debajo de ese umbral no sirve de nada si no se arregla primero el tratamiento de los extremos.

La columna de cocientes de la salida lo resume: $\omega$ es 252 veces peor que $v$ sin ruido, y solo 6 veces peor con $\sigma = 10^{-2}$. No es que $\omega$ mejore, es que $v$ se degrada más rápido cuando el ruido domina.

### 6.3 ¿Qué implicaciones tiene sobre la precisión reducir $h$?

Aquí está el resultado que más contradice la intuición.

**Con datos limpios, refinar sí mejora:**

| $h$ | Error en $\dot x$ | Razón |
|---:|---:|---:|
| 0.400 | 9.672×10⁻⁴ | — |
| 0.200 | 2.429×10⁻⁴ | 3.98 |
| 0.100 | 6.074×10⁻⁵ | 4.00 |
| 0.050 | 1.519×10⁻⁵ | 4.00 |
| 0.025 | 3.797×10⁻⁶ | 4.00 |

Cada vez que $h$ se parte a la mitad, el error se divide por 4. Eso es exactamente $O(h^2)$, y en escala log-log la pendiente da 2.

**Con ruido fijo de $10^{-3}$ m pasa lo contrario:**

| $h$ | Error en $v$ | Error en $\omega$ |
|---:|---:|---:|
| 0.400 | 4.182×10⁻³ | 5.525×10⁻² |
| 0.200 | 8.886×10⁻³ | 7.183×10⁻² |
| 0.100 | 1.945×10⁻² | 2.387×10⁻¹ |
| 0.050 | 4.160×10⁻² | 1.008×10⁰ |
| 0.025 | 8.900×10⁻² | 4.424×10⁰ |

El error **crece** al refinar. En $\omega$ pasa de 5.5×10⁻² a 4.42: un deterioro de **80 veces** por haber muestreado mejor.

Las razones de crecimiento confirman el análisis de 6.2. En $v$ el error se duplica al partir $h$ por la mitad, que es $\sigma/h$. En $\omega$ se cuadruplica en el tramo fino (2.387×10⁻¹ → 1.008 → 4.424, razones de 4.2 y 4.4), que es $\sigma/h^2$.

El error total tiene dos partes que se oponen:

$$E(h) \approx \underbrace{C h^2}_{\text{truncamiento}} + \underbrace{\frac{\sigma}{h}}_{\text{ruido}}$$

El primero baja al reducir $h$ y el segundo sube. Existe un $h$ óptimo donde la suma es mínima, y pasado ese punto refinar empeora las cosas. Con $\sigma = 10^{-3}$ el óptimo ya quedó por encima de 0.4 s, así que todos los pasos probados están del lado malo de la curva.

La conclusión práctica es que **el paso de muestreo debe escogerse según el ruido del sensor**, no según lo rápido que pueda muestrear el hardware. Un encoder más rápido no arregla nada si se le pide una resolución temporal que su ruido no soporta.

### 6.4 ¿Qué diferencias se observan entre los tres entornos?

**En los números.** Octave y C++ dan resultados idénticos a cero absoluto, en todas las columnas. Python difiere, pero únicamente en los dos puntos extremos; restringido al interior coincide también exactamente. La diferencia no viene del lenguaje ni de la aritmética de punto flotante, sino de que `np.gradient` eligió fórmulas unilaterales de dos puntos.

**En el esfuerzo de programación.** Octave y Python resuelven el problema en una docena de líneas. En C++ la lectura del CSV sola toma unas veinte, y el desenvolvimiento angular hay que escribirlo a mano. A cambio, cada operación queda explícita: no hay nada que la librería decida por uno, que es exactamente lo que generó la discrepancia en Python.

**En velocidad.** C++ corre en 0.003 s contra 1.28 s de Python. La comparación no es del todo justa porque los tiempos de los intérpretes incluyen el arranque y las gráficas, pero la ventaja se mantiene en cualquier medición. Para 51 muestras da igual; para un lazo de control en tiempo real no.

**Lo que se aprende de tener los tres.** Si solo se hubiera hecho la versión de Python, el error de borde habría pasado desapercibido, porque no hay con qué compararlo. La coincidencia bit a bit entre Octave y C++ es lo que permite afirmar que la diferencia de Python es una decisión de diseño de `np.gradient` y no un error de implementación.

### 6.5 Verificación del desenvolvimiento angular

Para comprobar que el `unwrap` hace falta de verdad, se corrió un caso de prueba con $\omega$ real constante de 1 rad/s a lo largo de un giro completo:

| | $\omega$ máximo calculado |
|:---|---:|
| Sin `unwrap` | 18.67 rad/s |
| Con `unwrap` | 1.00 rad/s |

`atan2` devuelve el ángulo restringido a $(-\pi, \pi]$, así que al cruzar esa frontera la señal salta $2\pi$ de golpe. Derivar ese salto produce un pico espurio: 18.67 rad/s donde el valor real es 1. El `unwrap` acumula múltiplos de $2\pi$ para volver la señal continua antes de derivarla, y con eso el resultado da exacto.

---

## 7. Conclusiones

**Sobre el método.** La diferencia centrada resultó cerca de cien veces más exacta que la unilateral con el mismo costo computacional (2.429×10⁻⁴ contra 2.380×10⁻²), y su orden $O(h^2)$ se verificó midiendo la razón de convergencia: 4.00 con tres cifras al partir $h$ por la mitad.

Pero el tratamiento de los extremos importa tanto como el del interior cuando hay diferenciaciones encadenadas. Usar fórmulas de tres puntos en lugar de dos redujo el error de borde en $\dot x$ de 2.37×10⁻² a un valor dos órdenes de magnitud menor, sin ningún costo adicional. Y como el error de borde se propaga un punto hacia adentro en cada diferenciación, en $\omega$ la diferencia se nota todavía más.

La lección que no estaba en el enunciado: **con datos ruidosos, refinar $h$ empeora el resultado**. El ruido se amplifica como $\sigma/h$ en la primera derivada y como $\sigma/h^2$ en la segunda, mientras el truncamiento solo decrece como $h^2$. Pasar de $h = 0.4$ a $h = 0.025$ con ruido de un milímetro degradó $\omega$ 80 veces.

**Sobre el entorno computacional.** Los tres producen resultados idénticos cuando implementan las mismas fórmulas: Octave y C++ coinciden bit a bit, con diferencia exactamente cero. Las discrepancias observadas no son de precisión del lenguaje sino de decisiones de diseño — `np.gradient` eligió fórmulas de dos puntos en los bordes, y esa sola elección explica toda la diferencia con Python.

Lo que separa a los entornos es el tiempo de desarrollo frente al control sobre el algoritmo. Una línea de NumPy reemplaza veinte de C++, pero esas veinte líneas son las que permiten decidir qué pasa exactamente en el primer y el último punto. Haber hecho las tres versiones fue lo que permitió detectar el problema: con una sola no hay contra qué comparar.

---

## 8. Reflexión

### 8.1 Reconstrucción del procedimiento

Partiendo de $(t, x, y)$ hay que calcular **dos derivadas en cadena**, en este orden:

**Primera derivada — de posición a velocidad.** Se diferencian $x$ e $y$ por separado respecto de $t$ para obtener $\dot x$ y $\dot y$. Hay que hacerlo por componentes, no sobre la distancia recorrida, porque la dirección se necesita en el paso siguiente.

**Composición — sin derivar.** De $\dot x$ y $\dot y$ salen dos cantidades por operaciones algebraicas: la rapidez $v = \sqrt{\dot x^2 + \dot y^2}$ y la orientación $\theta = \operatorname{atan2}(\dot y, \dot x)$. Aquí no se introduce error de diferenciación, pero sí se propaga el de la etapa anterior.

**Corrección — el unwrap.** Antes de seguir hay que desenvolver $\theta$, porque `atan2` la entrega acotada a $(-\pi,\pi]$ y los saltos de $2\pi$ no son cambios físicos de orientación. La prueba de la sección 6.5 muestra qué pasa si se omite: un pico de 18.67 rad/s donde el valor real es 1.

**Segunda derivada — de orientación a velocidad angular.** $\omega = \dot\theta$, diferenciando el $\theta$ ya desenvuelto.

**Dónde entra el error numérico.** En tres lugares distintos, y conviene no confundirlos:

- En cada diferenciación, por **truncamiento** de la serie de Taylor. Es $O(h^2)$ con diferencias centradas y decrece al refinar $h$.
- En cada diferenciación, por **amplificación del ruido** de los datos. Escala como $\sigma/h$ y crece al refinar $h$. Como hay dos diferenciaciones, en $\omega$ el efecto es $\sigma/h^2$.
- En los **extremos del dominio**, donde no existe el punto $i-1$ o el $i+1$. Si se usan fórmulas de dos puntos el orden cae a $O(h)$, y ese error se propaga un punto hacia el interior en cada diferenciación sucesiva.

La cantidad más delicada es $\omega$, porque acumula las tres fuentes al estar al final de la cadena. Todos los resultados anómalos del taller tienen que ver con ella.

### 8.2 Transferencia del aprendizaje

**Qué conservaría.** La estructura del procedimiento, que no depende de los datos: derivar por componentes, componer $v$ y $\theta$, desenvolver, derivar de nuevo. También conservaría la diferencia centrada como punto de partida, y la costumbre de tratar los extremos con fórmulas del mismo orden que el interior.

**Qué modificaría.** Tres cosas.

Primero, **no derivaría los datos crudos**. Con ruido de sensores aplicaría algún suavizado previo. Los resultados de la sección 6.3 muestran que sin eso el cálculo de $\omega$ puede entregar más ruido que señal.

Segundo, **no supondría muestreo uniforme**. Los datos reales llegan con intervalos irregulares, así que usaría la forma $(f_{i+1}-f_{i-1})/(t_{i+1}-t_{i-1})$ — que es la que implementé en C++ precisamente por eso — en lugar de dividir por un $2h$ constante.

Tercero, **mediría el ruido antes de elegir el paso**. Estimaría $\sigma$ con el sensor quieto, y a partir de ahí escogería $h$ cerca del óptimo que equilibra truncamiento y ruido, en lugar de tomar la frecuencia máxima que el hardware permita.

**Qué criterio usaría para escoger el método.** La pregunta decisiva no es cuál esquema tiene mejor orden, sino **cuál es la relación entre el ruido y el paso de muestreo**.

Si los datos son limpios, el truncamiento domina y conviene el esquema de mayor orden con el $h$ más fino disponible.

Si hay ruido apreciable, el orden del esquema deja de ser el factor limitante: da igual usar $O(h^2)$ o $O(h^4)$ si el error está dominado por $\sigma/h$. Ahí la decisión correcta es suavizar primero y aceptar un $h$ mayor.

Un criterio operativo: estimar los dos términos de $E(h) \approx Ch^2 + \sigma/h$ con los datos que se tengan y ver cuál domina. Si es el segundo, el esfuerzo va en el filtrado y no en un esquema de orden superior.

---

## 9. Reproducción

**Generar los datos y ejecutar Octave**

```bash
cd Octave
octave --no-gui -q generar_datos.m
octave --no-gui -q parte1_octave.m
```

**Python**

```bash
cd Python
python3 parte2_python.py
```

**C/C++ con GSL**

```bash
cd C_C++/diferenciacion
cmake -B build && cmake --build build
cd build && ./diferenciacion
```

Requiere `octave` (con `gnuplot` y `ghostscript` para exportar PNG), `python3` con NumPy y Matplotlib, y `libgsl-dev`.

### Archivos

| Ruta en el repositorio | Contenido |
|:---|:---|
| `Octave/generar_datos.m` | Construye `trayectoria_robot.csv` |
| `Octave/parte1_octave.m` | Parte 1 y las cuatro gráficas |
| `Python/parte2_python.py` | Parte 2, estudio de ruido y convergencia |
| `C_C++/diferenciacion/parte3_cpp.cpp` | Parte 3 y exploración de `gsl_deriv_central` |
| `C_C++/diferenciacion/CMakeLists.txt` | Configuración de compilación con GSL |
| `C_C++/diferenciacion/trayectoria_robot.csv` | 51 muestras de $(t, x, y)$ |
| `Octave/resultados_octave.csv`, `Python/resultados_python.csv` | Resultados numéricos exportados |
| `Octave/figuras/`, `Python/figuras/` | Gráficas en PNG |
| `Informe/diferenciacion/` | Este informe |