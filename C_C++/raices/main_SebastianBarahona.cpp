#include <iostream>
#include <iomanip>
#include <cmath>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>
#include <string>
#include <iomanip>

// f(x) = x^3 - 5x + 1
double f(double x, void *) {
  return x*x*x - 5.0*x + 1.0;
}

int main(int argc, char *argv[]) {
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  gsl_function F;
  F.function = &f;
  F.params = nullptr;
  double x_lo = 0.0;
  double x_hi = 1.0;

  std::string metodo = (argc > 1) ? argv[1] : "bisection";

  if (metodo == "bisection")      T = gsl_root_fsolver_bisection;
  else if (metodo == "falsepos")  T = gsl_root_fsolver_falsepos;
  else if (metodo == "brent")     T = gsl_root_fsolver_brent;
  else {
    std::cerr << "Metodo no valido: " << metodo << "\n";
    std::cerr << "Use: bisection | falsepos | brent\n";
    return 1;
  }

  std::cout << "Metodo: " << metodo << "\n";
  s = gsl_root_fsolver_alloc(T);
  gsl_root_fsolver_set(s, &F, x_lo, x_hi);
  std::cout << "iter\t" << "inf\t" << "sup\t" << "raíz\n";

  int status;
  int iter = 0;
  int max_iter = 100;
  double r;

  do {
    iter++;
    status = gsl_root_fsolver_iterate(s);
    r = gsl_root_fsolver_root(s);
    x_lo = gsl_root_fsolver_x_lower(s);
    x_hi = gsl_root_fsolver_x_upper(s);
    std::cout << iter << "\t" << x_lo << "\t" << x_hi << "\t" << r << "\n"; 
          status = gsl_root_test_interval( x_lo, x_hi, 0.0, 1e-8);
  } while(status == GSL_CONTINUE && iter < max_iter);

  std::cout << "\nMetodo          = " << metodo << "\n";
  std::cout << "Raiz encontrada = " << std::fixed << std::setprecision(10) << r << "\n";
  std::cout << "Iteraciones     = " << iter << "\n";
  std::cout << "f(raiz)         = " << std::scientific << f(r, nullptr) << "\n"; 
 return 0;
}

