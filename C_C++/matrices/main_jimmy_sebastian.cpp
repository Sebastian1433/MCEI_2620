#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

using namespace Eigen;
using namespace std;

// ======================================================
// FUNCION PARA CALCULAR LA PSEUDOINVERSA DE MOORE-PENROSE
// ======================================================

MatrixXd pseudoInverse(const MatrixXd& A, double tol = 1e-10)
{
    JacobiSVD<MatrixXd> svd(A, ComputeThinU | ComputeThinV);

    VectorXd singular = svd.singularValues();
    VectorXd singular_inv = singular;

    for (int i = 0; i < singular.size(); i++)
    {
        if (singular(i) > tol)
            singular_inv(i) = 1.0 / singular(i);
        else
            singular_inv(i) = 0.0;
    }

    return svd.matrixV()
         * singular_inv.asDiagonal()
         * svd.matrixU().transpose();
}

// ======================================================
// PUNTO 3 - CONDICIONAMIENTO NUMERICO
// ======================================================

void analizarCondicionamiento()
{
    cout << "\n========================================\n";
    cout << "PUNTO 3 - CONDICIONAMIENTO NUMERICO\n";
    cout << "========================================\n\n";

    const int n = 5;

    MatrixXd H(n,n);

    // Crear matriz de Hilbert 5x5
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            H(i,j) = 1.0 / (i + j + 1.0);
        }
    }

    cout << "Matriz de Hilbert H:\n";
    cout << H << "\n\n";

    // Calcular numero de condicion mediante SVD
    JacobiSVD<MatrixXd> svd(H);

    VectorXd valoresSingulares = svd.singularValues();

    double condicion =
        valoresSingulares(0) /
        valoresSingulares(valoresSingulares.size() - 1);

    cout << "Numero de condicion:\n";
    cout << condicion << "\n\n";

    // Solucion real conocida
    VectorXd x_real = VectorXd::Ones(n);

    cout << "Solucion real:\n";
    cout << x_real << "\n\n";

    // Construir vector b
    VectorXd b = H * x_real;

    cout << "Vector b:\n";
    cout << b << "\n\n";

    // Resolver sistema original
    VectorXd x =
        H.colPivHouseholderQr().solve(b);

    cout << "Solucion sistema original:\n";
    cout << x << "\n\n";

    // Perturbacion pequena en b
    VectorXd b_pert = b;

    b_pert(0) += 1e-5;

    cout << "Vector b perturbado:\n";
    cout << b_pert << "\n\n";

    // Resolver sistema perturbado
    VectorXd x_pert =
        H.colPivHouseholderQr().solve(b_pert);

    cout << "Solucion con perturbacion:\n";
    cout << x_pert << "\n\n";

    // Calcular error relativo
    double error_rel =
        (x_pert - x).norm() / x.norm();

    cout << "Error relativo:\n";
    cout << error_rel << "\n";
}

// ======================================================
// PROGRAMA PRINCIPAL
// ======================================================

int main()
{
    cout << "========================================\n";
    cout << "PUNTO 1 - PSEUDOINVERSA DE MOORE-PENROSE\n";
    cout << "========================================\n\n";

    MatrixXd A(4,3);

    A << 1,  0,  2,
         2, -1,  5,
         0,  1, -1,
         1,  3, -1;

    cout << "Matriz A:\n";
    cout << A << "\n\n";

    MatrixXd Ap = pseudoInverse(A);

    cout << "Pseudoinversa A+:\n";
    cout << Ap << "\n\n";

    // Propiedad 1
    cout << "Propiedad 1: A*A+*A\n";
    cout << A * Ap * A << "\n\n";

    // Propiedad 2
    cout << "Propiedad 2: A+*A*A+\n";
    cout << Ap * A * Ap << "\n\n";

    // Propiedad 3
    cout << "Propiedad 3: (A*A+)^T\n";
    cout << (A * Ap).transpose() << "\n\n";

    cout << "A*A+:\n";
    cout << A * Ap << "\n\n";

    // Propiedad 4
    cout << "Propiedad 4: (A+*A)^T\n";
    cout << (Ap * A).transpose() << "\n\n";

    cout << "A+*A:\n";
    cout << Ap * A << "\n\n";

    // Ejecutar analisis de condicionamiento
    analizarCondicionamiento();

    return 0;
}