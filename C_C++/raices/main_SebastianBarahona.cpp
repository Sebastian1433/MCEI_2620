// Taller de raices con GSL - Sebastian Barahona
// f(x) = x^3 - 5x + 1
//
// Uso: ./raices <metodo>
//   cerrados: bisection | falsepos | brent
//   abiertos: newton | secant | steffenson

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>

// f(x) = x^3 - 5x + 1
double f(double x, void *) {
  return x*x*x - 5.0*x + 1.0;
}

// f'(x) = 3x^2 - 5
double df(double x, void *) {
  return 3.0*x*x - 5.0;
}

// GSL exige f y f' empaquetadas juntas
void fdf(double x, void *p, double *y, double *dy) {
  *y  = f(x, p);
  *dy = df(x, p);
}

int main(int argc, char *argv[]) {

  std::string metodo = (argc > 1) ? argv[1] : "bisection";

  // Variables compartidas por los dos bloques
  int    iter = 0, max_iter = 100, status;
  double r = 0.0;

  // ============ METODOS CERRADOS ============
  if (metodo == "bisection" || metodo == "falsepos" || metodo == "brent") {

    const gsl_root_fsolver_type *T;
    if (metodo == "bisection")      T = gsl_root_fsolver_bisection;
    else if (metodo == "falsepos")  T = gsl_root_fsolver_falsepos;
    else                            T = gsl_root_fsolver_brent;

    gsl_function F;
    F.function = &f;
    F.params   = nullptr;

    double x_lo = 0.0;
    double x_hi = 1.0;

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
  else if (metodo == "newton" || metodo == "secant" || metodo == "steffenson") {

    const gsl_root_fdfsolver_type *T;
    if (metodo == "newton")       T = gsl_root_fdfsolver_newton;
    else if (metodo == "secant")  T = gsl_root_fdfsolver_secant;
    else                          T = gsl_root_fdfsolver_steffenson;

    gsl_function_fdf FDF;
    FDF.f      = &f;
    FDF.df     = &df;
    FDF.fdf    = &fdf;
    FDF.params = nullptr;

    double x0 = 0.5;

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

  return 0;
}
