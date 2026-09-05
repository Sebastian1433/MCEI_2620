// Taller de raices con GSL - Sebastian Barahona
// f1(x) = x^3 - 5x + 1        f2(x) = e^(-x) - x
//
// Dos formas de usarlo:
//
//   1) Interactivo: ./raices           -> el programa pregunta
//   2) Por argumentos: ./raices <metodo> [a b | x0] [f2]
//
//        cerrados: bisection | falsepos | brent    -> intervalo [a,b]
//        abiertos: newton | secant | steffenson    -> valor inicial x0
//        f2: opcional, cambia a la funcion e^(-x) - x
//
//      ./raices brent -3 -2
//      ./raices newton 2.5
//      ./raices brent 0 1 f2

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <cstdlib>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>

// ---------------- Funciones ----------------

// f1(x) = x^3 - 5x + 1
double f1(double x, void *)  { return x*x*x - 5.0*x + 1.0; }
double df1(double x, void *) { return 3.0*x*x - 5.0; }

// f2(x) = e^(-x) - x
double f2(double x, void *)  { return std::exp(-x) - x; }
double df2(double x, void *) { return -std::exp(-x) - 1.0; }

// Selector: decide cual de las dos se usa
bool usar_f2 = false;

double f(double x, void *p)  { return usar_f2 ? f2(x, p)  : f1(x, p);  }
double df(double x, void *p) { return usar_f2 ? df2(x, p) : df1(x, p); }

// GSL exige f y f' empaquetadas juntas
void fdf(double x, void *p, double *y, double *dy) {
  *y  = f(x, p);
  *dy = df(x, p);
}

// Devuelve true si el metodo pertenece a la familia cerrada
bool es_cerrado(const std::string &m) {
  return m == "bisection" || m == "falsepos" || m == "brent";
}
bool es_abierto(const std::string &m) {
  return m == "newton" || m == "secant" || m == "steffenson";
}

int main(int argc, char *argv[]) {

  std::string metodo;
  double x_lo = 0.0, x_hi = 1.0, x0 = 0.5;

  // ============ MODO INTERACTIVO ============
  // Se activa cuando no se pasan argumentos.
  if (argc == 1) {

    std::cout << "\n===== BUSQUEDA DE RAICES CON GSL =====\n\n";

    // --- Funcion ---
    std::cout << "Funcion:\n";
    std::cout << "  1) f(x) = x^3 - 5x + 1\n";
    std::cout << "  2) f(x) = e^(-x) - x\n";
    std::cout << "Opcion: ";
    int op_f;
    std::cin >> op_f;
    usar_f2 = (op_f == 2);

    // --- Metodo ---
    std::cout << "\nMetodo:\n";
    std::cout << "  Cerrados (piden intervalo [a,b]):\n";
    std::cout << "    1) Biseccion\n";
    std::cout << "    2) False Position\n";
    std::cout << "    3) Brent\n";
    std::cout << "  Abiertos (piden valor inicial x0):\n";
    std::cout << "    4) Newton\n";
    std::cout << "    5) Secante\n";
    std::cout << "    6) Steffenson\n";
    std::cout << "Opcion: ";
    int op_m;
    std::cin >> op_m;

    switch (op_m) {
      case 1: metodo = "bisection";  break;
      case 2: metodo = "falsepos";   break;
      case 3: metodo = "brent";      break;
      case 4: metodo = "newton";     break;
      case 5: metodo = "secant";     break;
      case 6: metodo = "steffenson"; break;
      default:
        std::cerr << "Opcion de metodo no valida.\n";
        return 1;
    }

    // --- Valores iniciales: depende de la familia ---
    if (es_cerrado(metodo)) {
      std::cout << "\nLimite inferior del intervalo: ";
      std::cin >> x_lo;
      std::cout << "Limite superior del intervalo: ";
      std::cin >> x_hi;
    } else {
      std::cout << "\nValor inicial x0: ";
      std::cin >> x0;
    }

    std::cout << "\n";
  }

  // ============ MODO POR ARGUMENTOS ============
  else {
    metodo = argv[1];

    for (int i = 2; i < argc; i++)
      if (std::string(argv[i]) == "f2") usar_f2 = true;

    if (es_cerrado(metodo)) {
      if (argc > 2) x_lo = std::atof(argv[2]);
      if (argc > 3) x_hi = std::atof(argv[3]);
    } else {
      if (argc > 2) x0 = std::atof(argv[2]);
    }
  }

  // Variables compartidas por los dos bloques
  int    iter = 0, max_iter = 100, status;
  double r = 0.0;

  std::cout << "Funcion: " << (usar_f2 ? "f(x) = e^(-x) - x" : "f(x) = x^3 - 5x + 1") << "\n\n";

  // ============ METODOS CERRADOS ============
  if (es_cerrado(metodo)) {

    const gsl_root_fsolver_type *T;
    if (metodo == "bisection")      T = gsl_root_fsolver_bisection;
    else if (metodo == "falsepos")  T = gsl_root_fsolver_falsepos;
    else                            T = gsl_root_fsolver_brent;

    gsl_function F;
    F.function = &f;
    F.params   = nullptr;

    gsl_root_fsolver *s = gsl_root_fsolver_alloc(T);
    gsl_root_fsolver_set(s, &F, x_lo, x_hi);

    std::cout << "iter\t" << "inf\t\t" << "sup\t\t" << "raiz\n";

    do {
      iter++;
      status = gsl_root_fsolver_iterate(s);
      r    = gsl_root_fsolver_root(s);
      x_lo = gsl_root_fsolver_x_lower(s);
      x_hi = gsl_root_fsolver_x_upper(s);
      std::cout << iter << "\t" << std::fixed << std::setprecision(8)
                << x_lo << "\t" << x_hi << "\t" << r << "\n";
      status = gsl_root_test_interval(x_lo, x_hi, 0.0, 1e-8);
    } while (status == GSL_CONTINUE && iter < max_iter);

    gsl_root_fsolver_free(s);
  }

  // ============ METODOS ABIERTOS ============
  else if (es_abierto(metodo)) {

    const gsl_root_fdfsolver_type *T;
    if (metodo == "newton")       T = gsl_root_fdfsolver_newton;
    else if (metodo == "secant")  T = gsl_root_fdfsolver_secant;
    else                          T = gsl_root_fdfsolver_steffenson;

    gsl_function_fdf FDF;
    FDF.f      = &f;
    FDF.df     = &df;
    FDF.fdf    = &fdf;
    FDF.params = nullptr;

    gsl_root_fdfsolver *s = gsl_root_fdfsolver_alloc(T);
    gsl_root_fdfsolver_set(s, &FDF, x0);

    std::cout << "iter\t" << "x_anterior\t" << "x_nuevo\n";

    double x = x0, x_prev;

    do {
      iter++;
      x_prev = x;
      status = gsl_root_fdfsolver_iterate(s);
      x = gsl_root_fdfsolver_root(s);
      std::cout << iter << "\t" << std::fixed << std::setprecision(8)
                << x_prev << "\t" << x << "\n";
      status = gsl_root_test_delta(x, x_prev, 0.0, 1e-8);
    } while (status == GSL_CONTINUE && iter < max_iter);

    r = x;
    gsl_root_fdfsolver_free(s);
  }

  else {
    std::cerr << "Metodo no valido: " << metodo << "\n";
    std::cerr << "Use: bisection | falsepos | brent | newton | secant | steffenson\n";
    return 1;
  }

  // ============ RESULTADO ============
  std::cout << "\nMetodo          = " << metodo << "\n";
  std::cout << "Raiz encontrada = " << std::fixed << std::setprecision(10) << r << "\n";
  std::cout << "Iteraciones     = " << iter << "\n";
  std::cout << "f(raiz)         = " << std::scientific << f(r, nullptr) << "\n";

  if (status != GSL_SUCCESS)
    std::cout << "ADVERTENCIA: no convergio en " << max_iter << " iteraciones.\n";

  return 0;
}
