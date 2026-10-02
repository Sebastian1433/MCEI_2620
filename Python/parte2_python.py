#!/usr/bin/env python3
# =====================================================================
#  Taller de diferenciacion numerica - PARTE 2: Python con NumPy/SciPy
#  Estimacion de velocidad lineal y angular de un robot diferencial.
#
#  Procedimiento:  (x,y,t) -> (xdot,ydot) -> (v,theta) -> omega
# =====================================================================
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import os

os.makedirs("figuras", exist_ok=True)

# Paleta validada (separacion CVD dE 24.7 en protanopia).
# La identidad de cada serie va tambien en el estilo de trazo,
# de modo que no depende unicamente del color.
NUM = "#2a78d6"   # resultado numerico  - linea continua
EXA = "#eb6834"   # referencia analitica - linea punteada
GRID = "#d8d7d2"

# ---------------------------------------------------------------
# 1. Carga de datos
# ---------------------------------------------------------------
data = np.loadtxt("../C_C++/diferenciacion/trayectoria_robot.csv", delimiter=",", skiprows=1)
t, x, y = data[:, 0], data[:, 1], data[:, 2]
n = t.size
h = t[1] - t[0]

print("=" * 50)
print("PARTE 2 - PYTHON (NumPy)")
print("=" * 50)
print(f"\nMuestras cargadas: {n}")
print(f"Paso temporal h  : {h:.3f} s\n")

# ---------------------------------------------------------------
# 2. Derivadas con np.gradient
#
#    np.gradient aplica diferencias centradas O(h^2) en el interior
#    y formulas unilaterales de DOS puntos O(h) en los extremos.
#    Es la operacion que reemplaza el bucle explicito de Octave/C++.
# ---------------------------------------------------------------
vx = np.gradient(x, t)
vy = np.gradient(y, t)

v = np.sqrt(vx**2 + vy**2)
theta = np.unwrap(np.arctan2(vy, vx))      # DESENVOLVIMIENTO ANGULAR
omega = np.gradient(theta, t)

# ---------------------------------------------------------------
# 3. Verificacion contra las derivadas analiticas
# ---------------------------------------------------------------
dx_e = 0.16 * t + 0.18 * np.cos(0.45 * t)
dy_e = 0.50 + 0.135 * np.sin(0.45 * t)
ddx_e = 0.16 - 0.081 * np.sin(0.45 * t)
ddy_e = 0.06075 * np.cos(0.45 * t)

v_e = np.hypot(dx_e, dy_e)
th_e = np.unwrap(np.arctan2(dy_e, dx_e))
om_e = (dx_e * ddy_e - dy_e * ddx_e) / (dx_e**2 + dy_e**2)

I = slice(1, -1)
print("Errores maximos frente a la solucion analitica:")
print(f"  v      interior = {np.abs(v-v_e)[I].max():.3e}    global = {np.abs(v-v_e).max():.3e}")
print(f"  theta  interior = {np.abs(theta-th_e)[I].max():.3e}    global = {np.abs(theta-th_e).max():.3e}")
print(f"  omega  interior = {np.abs(omega-om_e)[I].max():.3e}    global = {np.abs(omega-om_e).max():.3e}")

print("\nPerfil del error en omega cerca de los bordes:")
for i in [0, 1, 2, 3, n-4, n-3, n-2, n-1]:
    print(f"  i={i:2d}  t={t[i]:4.1f}  err={abs(omega[i]-om_e[i]):.3e}")
print(f"  -> interior estricto (i=2..n-3): {np.abs(omega-om_e)[2:-2].max():.3e}")
print("  El error O(h) de los extremos se propaga UN punto hacia adentro")
print("  al volver a diferenciar theta.")

# ---------------------------------------------------------------
# 4. Convergencia de la diferencia centrada
# ---------------------------------------------------------------
print("\nConvergencia al refinar h (sin ruido):")
print(f"{'h':>8} {'err_max(xdot)':>16} {'razon':>8}")
prev = None
for hh in [0.4, 0.2, 0.1, 0.05, 0.025]:
    tt = np.arange(0, 10 + hh / 2, hh)
    xx = 0.08 * tt**2 + 0.40 * np.sin(0.45 * tt)
    de = 0.16 * tt + 0.18 * np.cos(0.45 * tt)
    cc = (xx[2:] - xx[:-2]) / (2 * hh)
    e = np.abs(cc - de[1:-1]).max()
    print(f"{hh:8.3f} {e:16.3e} {'' if prev is None else f'{prev/e:8.2f}'}")
    prev = e
print("  Razon ~ 4 al dividir h entre 2 confirma el orden O(h^2).")

# ---------------------------------------------------------------
# 5. Efecto del ruido (preguntas 2 y 3 del taller)
# ---------------------------------------------------------------
rng = np.random.default_rng(42)

def errores(tt, xx, yy, vex, omex, sigma, reps=200):
    ev, eo = [], []
    for _ in range(reps):
        xn = xx + rng.normal(0, sigma, xx.size)
        yn = yy + rng.normal(0, sigma, yy.size)
        vxn, vyn = np.gradient(xn, tt), np.gradient(yn, tt)
        vn = np.hypot(vxn, vyn)
        thn = np.unwrap(np.arctan2(vyn, vxn))
        omn = np.gradient(thn, tt)
        ev.append(np.abs(vn - vex)[1:-1].max())
        eo.append(np.abs(omn - omex)[1:-1].max())
    return np.mean(ev), np.mean(eo)

print("\nAmplificacion del ruido (h = 0.2 s fijo):")
print(f"{'sigma[m]':>10} {'err_v':>12} {'err_omega':>12} {'cociente':>10}")
for s in [0, 1e-4, 1e-3, 1e-2]:
    ev, eo = errores(t, x, y, v_e, om_e, s)
    print(f"{s:10.0e} {ev:12.3e} {eo:12.3e} {eo/ev:10.1f}")

print("\nReducir h con ruido FIJO sigma = 1e-3 m:")
print(f"{'h':>8} {'err_v':>14} {'err_omega':>14}")
for hh in [0.4, 0.2, 0.1, 0.05, 0.025]:
    tt = np.arange(0, 10 + hh / 2, hh)
    xx = 0.08 * tt**2 + 0.40 * np.sin(0.45 * tt)
    yy = 0.50 * tt + 0.30 * (1 - np.cos(0.45 * tt))
    dxe = 0.16 * tt + 0.18 * np.cos(0.45 * tt)
    dye = 0.50 + 0.135 * np.sin(0.45 * tt)
    ddxe = 0.16 - 0.081 * np.sin(0.45 * tt)
    ddye = 0.06075 * np.cos(0.45 * tt)
    vex = np.hypot(dxe, dye)
    omex = (dxe * ddye - dye * ddxe) / (dxe**2 + dye**2)
    ev, eo = errores(tt, xx, yy, vex, omex, 1e-3)
    print(f"{hh:8.3f} {ev:14.3e} {eo:14.3e}")
print("  El error CRECE al refinar h: el ruido domina sobre el truncamiento.")

# ---------------------------------------------------------------
# 6. Por que hace falta el unwrap
# ---------------------------------------------------------------
ang = np.linspace(0, 3 * np.pi, 60)
hc = ang[1] - ang[0]
thr = np.arctan2(np.sin(ang), np.cos(ang))
print("\nDemostracion del unwrap sobre un giro completo (omega real = 1 rad/s):")
print(f"  sin unwrap: omega maximo = {np.abs(np.gradient(thr, hc)).max():7.2f} rad/s  (salto espurio)")
print(f"  con unwrap: omega maximo = {np.abs(np.gradient(np.unwrap(thr), hc)).max():7.2f} rad/s")

# ---------------------------------------------------------------
# 7. Graficas
# ---------------------------------------------------------------
def estilo(ax, xlabel, ylabel, title):
    ax.set_xlabel(xlabel); ax.set_ylabel(ylabel); ax.set_title(title, loc="left")
    ax.grid(True, color=GRID, linewidth=0.6)
    ax.set_axisbelow(True)
    for s in ("top", "right"):
        ax.spines[s].set_visible(False)
    for s in ("left", "bottom"):
        ax.spines[s].set_color(GRID)

fig, ax = plt.subplots(figsize=(6.2, 5.0))
ax.plot(x, y, color=NUM, lw=2, label="trayectoria")
ax.plot(x[0], y[0], "o", color=NUM, ms=9, label="inicio")
ax.plot(x[-1], y[-1], "s", color=EXA, ms=9, label="fin")
ax.set_aspect("equal")
estilo(ax, "x [m]", "y [m]", "Trayectoria del robot")
ax.legend(frameon=False)
fig.tight_layout(); fig.savefig("figuras/python_trayectoria.png", dpi=150)

for nombre, num, exa, ylab, tit in [
    ("velocidad", v, v_e, "v [m/s]", "Velocidad lineal"),
    ("theta", theta, th_e, "theta [rad]", "Orientacion (con unwrap)"),
    ("omega", omega, om_e, "omega [rad/s]", "Velocidad angular"),
]:
    fig, ax = plt.subplots(figsize=(6.6, 3.8))
    ax.plot(t, num, color=NUM, lw=2, label="numerica")
    ax.plot(t, exa, color=EXA, lw=2, ls="--", label="analitica")
    estilo(ax, "t [s]", ylab, tit)
    ax.legend(frameon=False)
    fig.tight_layout(); fig.savefig(f"figuras/python_{nombre}.png", dpi=150)

# convergencia log-log
hs = np.array([0.4, 0.2, 0.1, 0.05, 0.025])
err_lim, err_ruido = [], []
for hh in hs:
    tt = np.arange(0, 10 + hh / 2, hh)
    xx = 0.08 * tt**2 + 0.40 * np.sin(0.45 * tt)
    de = 0.16 * tt + 0.18 * np.cos(0.45 * tt)
    err_lim.append(np.abs((xx[2:] - xx[:-2]) / (2 * hh) - de[1:-1]).max())
    xn = xx + rng.normal(0, 1e-3, xx.size)
    err_ruido.append(np.abs((xn[2:] - xn[:-2]) / (2 * hh) - de[1:-1]).max())

fig, ax = plt.subplots(figsize=(6.6, 4.2))
ax.loglog(hs, err_lim, "o-", color=NUM, lw=2, ms=8, label="datos limpios")
ax.loglog(hs, err_ruido, "s--", color=EXA, lw=2, ms=8, label="con ruido 1e-3 m")
estilo(ax, "h [s]", "error maximo en xdot", "Convergencia de la diferencia centrada")
ax.legend(frameon=False)
fig.tight_layout(); fig.savefig("figuras/python_convergencia.png", dpi=150)

print("\nGraficas guardadas en Python/figuras/")

np.savetxt("resultados_python.csv",
           np.c_[t, x, y, vx, vy, v, theta, omega], delimiter=",",
           header="t,x,y,xdot,ydot,v,theta,omega", comments="", fmt="%.6f")
print("Resultados en resultados_python.csv")