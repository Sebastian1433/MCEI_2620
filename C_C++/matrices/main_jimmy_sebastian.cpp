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
// FUNCION PARA CREAR MATRIZ DE HILBERT
// ======================================================

MatrixXd crearHilbert(int n)
{
    MatrixXd H(n, n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            H(i, j) = 1.0 / (i + j + 1.0);
        }
    }

    return H;
}


// ======================================================
// PUNTO 1 - PSEUDOINVERSA DE MOORE-PENROSE
// ======================================================

void punto1MoorePenrose()
{
    cout << "========================================\n";
    cout << "PUNTO 1 - PSEUDOINVERSA DE MOORE-PENROSE\n";
    cout << "========================================\n\n";

    MatrixXd A(4, 3);

    A << 1,  0,  2,
         2, -1,  5,
         0,  1, -1,
         1,  3, -1;

    cout << "Matriz A:\n";
    cout << A << "\n\n";

    MatrixXd Ap = pseudoInverse(A);

    cout << "Pseudoinversa A+:\n";
    cout << Ap << "\n\n";

    cout << "Propiedad 1: A*A+*A\n";
    cout << A * Ap * A << "\n\n";

    cout << "Propiedad 2: A+*A*A+\n";
    cout << Ap * A * Ap << "\n\n";

    cout << "Propiedad 3: (A*A+)^T\n";
    cout << (A * Ap).transpose() << "\n\n";

    cout << "A*A+:\n";
    cout << A * Ap << "\n\n";

    cout << "Propiedad 4: (A+*A)^T\n";
    cout << (Ap * A).transpose() << "\n\n";

    cout << "A+*A:\n";
    cout << Ap * A << "\n\n";
}


// ======================================================
// PUNTO 3 - CONDICIONAMIENTO NUMERICO
// ======================================================

void punto3Condicionamiento()
{
    cout << "\n========================================\n";
    cout << "PUNTO 3 - CONDICIONAMIENTO NUMERICO\n";
    cout << "========================================\n\n";

    const int n = 5;

    MatrixXd H = crearHilbert(n);

    cout << "Matriz de Hilbert H:\n";
    cout << H << "\n\n";

    // Numero de condicion mediante SVD
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

    // Resolver sistema original usando QR
    VectorXd x =
        H.colPivHouseholderQr().solve(b);

    cout << "Solucion sistema original:\n";
    cout << x << "\n\n";

    // Perturbacion pequena
    VectorXd b_pert = b;
    b_pert(0) += 1e-5;

    cout << "Vector b perturbado:\n";
    cout << b_pert << "\n\n";

    // Resolver sistema perturbado
    VectorXd x_pert =
        H.colPivHouseholderQr().solve(b_pert);

    cout << "Solucion con perturbacion:\n";
    cout << x_pert << "\n\n";

    // Error relativo
    double error_rel =
        (x_pert - x).norm() / x.norm();

    cout << "Error relativo:\n";
    cout << error_rel << "\n";
}


// ======================================================
// PUNTO 4 - INVERSA DIRECTA, QR Y SVD
// ======================================================

void punto4Inversa()
{
    cout << "\n========================================\n";
    cout << "PUNTO 4 - INVERSA DIRECTA, QR Y SVD\n";
    cout << "========================================\n\n";

    const int n = 5;

    MatrixXd A = crearHilbert(n);
    MatrixXd I = MatrixXd::Identity(n, n);

    cout << "Matriz A (Hilbert 5x5):\n";
    cout << A << "\n\n";

    // --------------------------------------------------
    // 1. Inversa directa
    // --------------------------------------------------

    MatrixXd Ainv_directa = A.inverse();

    double error_directa =
        (A * Ainv_directa - I).norm();

    cout << "Inversa directa:\n";
    cout << Ainv_directa << "\n\n";

    cout << "Error inversa directa ||A*A^-1 - I||:\n";
    cout << error_directa << "\n\n";


    // --------------------------------------------------
    // 2. Inversa usando QR
    // Resolver A*X = I
    // --------------------------------------------------

    HouseholderQR<MatrixXd> qr(A);

    MatrixXd Ainv_qr = qr.solve(I);

    double error_qr =
        (A * Ainv_qr - I).norm();

    cout << "Inversa mediante QR:\n";
    cout << Ainv_qr << "\n\n";

    cout << "Error QR ||A*A^-1 - I||:\n";
    cout << error_qr << "\n\n";


    // --------------------------------------------------
    // 3. Inversa usando SVD
    // A^-1 = V * Sigma^-1 * U^T
    // --------------------------------------------------

    JacobiSVD<MatrixXd> svd(
        A,
        ComputeFullU | ComputeFullV
    );

    VectorXd singular = svd.singularValues();
    VectorXd singular_inv = singular;

    for (int i = 0; i < singular.size(); i++)
    {
        singular_inv(i) = 1.0 / singular(i);
    }

    MatrixXd Ainv_svd =
        svd.matrixV()
        * singular_inv.asDiagonal()
        * svd.matrixU().transpose();

    double error_svd =
        (A * Ainv_svd - I).norm();

    cout << "Inversa mediante SVD:\n";
    cout << Ainv_svd << "\n\n";

    cout << "Error SVD ||A*A^-1 - I||:\n";
    cout << error_svd << "\n\n";


    // --------------------------------------------------
    // Comparacion final
    // --------------------------------------------------

    cout << "COMPARACION DE ERRORES\n";
    cout << "Directa: " << error_directa << "\n";
    cout << "QR:      " << error_qr << "\n";
    cout << "SVD:     " << error_svd << "\n";
}


// ======================================================
// PROGRAMA PRINCIPAL
// ======================================================

int main()
{
    punto1MoorePenrose();

    punto3Condicionamiento();

    punto4Inversa();

    return 0;
}