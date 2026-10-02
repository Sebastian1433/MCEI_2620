// =====================================================================
//  Taller de diferenciacion numerica - PARTE 3: C/C++ con GSL
//  Estimacion de velocidad lineal y angular de un robot diferencial.
//
//  Procedimiento:  (x,y,t) -> (xdot,ydot) -> (v,theta) -> omega
//
//  El taller pide ademas explorar gsl_deriv_central y explicar por que
//  los datos tabulados exigen una estrategia distinta: esa comparacion
//  esta en la seccion 5 de este programa.
// =====================================================================
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

// ---------------------------------------------------------------
// 1. Lectura del CSV
// ---------------------------------------------------------------
struct Datos {
    std::vector<double> t, x, y;
};

Datos leerCSV(const std::string &ruta) {
    std::ifstream f(ruta);
    if (!f) {
        std::cerr << "ERROR: no se pudo abrir " << ruta << "\n";
        std::cerr << "Ejecute el programa desde la carpeta que contiene el CSV.\n";
        std::exit(1);
    }
    Datos d;
    std::string linea;
    std::getline(f, linea);                 // descarta el encabezado
    while (std::getline(f, linea)) {
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        std::string c;
        double v[3];
        for (int k = 0; k < 3; ++k) { std::getline(ss, c, ','); v[k] = std::stod(c); }
        d.t.push_back(v[0]); d.x.push_back(v[1]); d.y.push_back(v[2]);
    }
    return d;
}

// ---------------------------------------------------------------
// 2. Diferencias centradas sobre datos tabulados
//
//    Interior:  f'(t_i) ~ (f_{i+1} - f_{i-1}) / (t_{i+1} - t_{i-1})   O(h^2)
//    Extremos:  formulas unilaterales de tres puntos                  O(h^2)
//
//    Se usan las de tres puntos y no las de dos para no degradar el
//    orden en los bordes: con dos puntos el error alli seria O(h) y,
//    al volver a diferenciar theta, contaminaria tambien al vecino.
// ---------------------------------------------------------------
std::vector<double> derivadaCentral(const std::vector<double> &f,
                                    const std::vector<double> &t) {
    const int n = f.size();
    std::vector<double> d(n);
    for (int i = 1; i < n - 1; ++i)
        d[i] = (f[i+1] - f[i-1]) / (t[i+1] - t[i-1]);

    const double h = t[1] - t[0];
    d[0]     = (-3.0*f[0] + 4.0*f[1] - f[2]) / (2.0*h);
    d[n-1]   = ( 3.0*f[n-1] - 4.0*f[n-2] + f[n-3]) / (2.0*h);
    return d;
}

// ---------------------------------------------------------------
// 3. Desenvolvimiento angular (equivalente a unwrap)
//
//    atan2 devuelve valores en (-pi, pi]. Al cruzar esa frontera
//    aparece un salto de 2*pi que la diferenciacion interpreta como
//    una velocidad angular enorme. Se acumulan multiplos de 2*pi
//    para que theta quede continua antes de derivarla.
// ---------------------------------------------------------------
std::vector<double> desenvolver(const std::vector<double> &th) {
    std::vector<double> u(th.size());
    u[0] = th[0];
    double correccion = 0.0;
    for (size_t i = 1; i < th.size(); ++i) {
        double d = th[i] - th[i-1];
        if (d >  M_PI) correccion -= 2.0*M_PI;
        if (d < -M_PI) correccion += 2.0*M_PI;
        u[i] = th[i] + correccion;
    }
    return u;
}

// ---------------------------------------------------------------
// 4. Funciones analiticas, para gsl_deriv_central y para el error
// ---------------------------------------------------------------
double fx(double t, void *) { return 0.08*t*t + 0.40*std::sin(0.45*t); }
double fy(double t, void *) { return 0.50*t   + 0.30*(1.0 - std::cos(0.45*t)); }

double dfx(double t) { return 0.16*t + 0.18*std::cos(0.45*t); }
double dfy(double t) { return 0.50   + 0.135*std::sin(0.45*t); }
double ddfx(double t){ return 0.16   - 0.081*std::sin(0.45*t); }
double ddfy(double t){ return 0.06075*std::cos(0.45*t); }

double maxAbs(const std::vector<double> &a, const std::vector<double> &b,
              int i0, int i1) {
    double m = 0.0;
    for (int i = i0; i <= i1; ++i) m = std::max(m, std::fabs(a[i] - b[i]));
    return m;
}

int main(int argc, char *argv[]) {
    std::string ruta = (argc > 1) ? argv[1] : "trayectoria_robot.csv";
    Datos d = leerCSV(ruta);
    const int n = d.t.size();
    const double h = d.t[1] - d.t[0];

    std::cout << "==================================================\n";
    std::cout << "PARTE 3 - C/C++ CON GSL\n";
    std::cout << "==================================================\n\n";
    std::cout << "Muestras cargadas: " << n << "\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Paso temporal h  : " << h << " s\n\n";

    // ---------- Derivadas sobre los datos tabulados ----------
    std::vector<double> xdot = derivadaCentral(d.x, d.t);
    std::vector<double> ydot = derivadaCentral(d.y, d.t);

    std::vector<double> v(n), th(n);
    for (int i = 0; i < n; ++i) {
        v[i]  = std::sqrt(xdot[i]*xdot[i] + ydot[i]*ydot[i]);
        th[i] = std::atan2(ydot[i], xdot[i]);
    }
    std::vector<double> theta = desenvolver(th);
    std::vector<double> omega = derivadaCentral(theta, d.t);

    // ---------- Referencia analitica ----------
    std::vector<double> v_e(n), th_e(n), om_e(n), xdot_e(n);
    for (int i = 0; i < n; ++i) {
        double t = d.t[i];
        double a = dfx(t), b = dfy(t), c = ddfx(t), e = ddfy(t);
        xdot_e[i] = a;
        v_e[i]    = std::sqrt(a*a + b*b);
        th_e[i]   = std::atan2(b, a);
        om_e[i]   = (a*e - b*c) / (a*a + b*b);
    }
    th_e = desenvolver(th_e);

    std::cout << std::scientific << std::setprecision(3);
    std::cout << "Errores maximos frente a la solucion analitica:\n";
    std::cout << "  xdot   interior = " << maxAbs(xdot, xdot_e, 1, n-2)
              << "    global = " << maxAbs(xdot, xdot_e, 0, n-1) << "\n";
    std::cout << "  v      interior = " << maxAbs(v, v_e, 1, n-2)
              << "    global = " << maxAbs(v, v_e, 0, n-1) << "\n";
    std::cout << "  theta  interior = " << maxAbs(theta, th_e, 1, n-2)
              << "    global = " << maxAbs(theta, th_e, 0, n-1) << "\n";
    std::cout << "  omega  interior = " << maxAbs(omega, om_e, 1, n-2)
              << "    global = " << maxAbs(omega, om_e, 0, n-1) << "\n";

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nRangos obtenidos:\n";
    double vmin=v[0], vmax=v[0], omin=omega[0], omax=omega[0];
    for (int i = 0; i < n; ++i) {
        vmin = std::min(vmin, v[i]); vmax = std::max(vmax, v[i]);
        omin = std::min(omin, omega[i]); omax = std::max(omax, omega[i]);
    }
    std::cout << "  v     en [" << vmin << ", " << vmax << "] m/s\n";
    std::cout << "  omega en [" << omin << ", " << omax << "] rad/s\n";

    // ---------- 5. gsl_deriv_central sobre la funcion evaluable ----------
    std::cout << "\n--------------------------------------------------\n";
    std::cout << "gsl_deriv_central SOBRE LA FUNCION EVALUABLE\n";
    std::cout << "--------------------------------------------------\n";

    gsl_function F;
    F.function = &fx;
    F.params   = nullptr;

    std::cout << std::scientific << std::setprecision(3);
    std::cout << "\n" << std::setw(8) << "t"
              << std::setw(16) << "gsl_deriv"
              << std::setw(14) << "err_gsl"
              << std::setw(16) << "err_tabulado" << "\n";

    double err_gsl_max = 0.0, err_tab_max = 0.0;
    for (int i : {10, 20, 30, 40}) {
        double t = d.t[i], res, abserr;
        gsl_deriv_central(&F, t, 1e-5, &res, &abserr);
        double eg = std::fabs(res - dfx(t));
        double et = std::fabs(xdot[i] - dfx(t));
        err_gsl_max = std::max(err_gsl_max, eg);
        err_tab_max = std::max(err_tab_max, et);
        std::cout << std::setw(8) << std::fixed << std::setprecision(1) << t
                  << std::scientific << std::setprecision(6) << std::setw(16) << res
                  << std::setprecision(3) << std::setw(14) << eg
                  << std::setw(16) << et << "\n";
    }

    std::cout << "\nPor que los datos tabulados exigen otra estrategia:\n";
    std::cout << "  gsl_deriv_central puede EVALUAR f en cualquier t, de modo que\n";
    std::cout << "  escoge su propio paso y hasta estima el error. Con 51 muestras\n";
    std::cout << "  fijas no existe f(t) fuera de la malla: el paso lo impone el\n";
    std::cout << "  muestreo (h = " << std::fixed << std::setprecision(2) << h
              << " s) y no se puede refinar.\n";
    std::cout << "  Por eso aqui las diferencias se aplican directamente sobre los\n";
    std::cout << "  arreglos, y GSL queda para otras operaciones numericas.\n";
    std::cout << std::scientific << std::setprecision(3);
    std::cout << "  Error de gsl_deriv_central (h libre)   : " << err_gsl_max << "\n";
    std::cout << "  Error de la diferencia tabulada (h fijo): " << err_tab_max << "\n";

    // ---------- 6. Exportar ----------
    std::ofstream out("resultados_cpp.csv");
    out << "t,x,y,xdot,ydot,v,theta,omega\n";
    out << std::fixed << std::setprecision(6);
    for (int i = 0; i < n; ++i)
        out << d.t[i] << "," << d.x[i] << "," << d.y[i] << ","
            << xdot[i] << "," << ydot[i] << "," << v[i] << ","
            << theta[i] << "," << omega[i] << "\n";
    out.close();
    std::cout << "\nResultados en resultados_cpp.csv\n";
    return 0;
}