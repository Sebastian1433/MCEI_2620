#include <iostream>
#include <Eigen/Dense>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace Eigen;

// ------------------------------------------------------------
// Eliminacion gaussiana con pivoteo parcial
// ------------------------------------------------------------
VectorXd eliminacionGaussiana(MatrixXd A, VectorXd b)
{
    int n = A.rows();

    for (int k = 0; k < n - 1; k++)
    {
        // Buscar pivote maximo
        int filaPivote = k;
        double maxValor = abs(A(k, k));

        for (int i = k + 1; i < n; i++)
        {
            if (abs(A(i, k)) > maxValor)
            {
                maxValor = abs(A(i, k));
                filaPivote = i;
            }
        }

        // Intercambio de filas
        if (filaPivote != k)
        {
            A.row(k).swap(A.row(filaPivote));
            swap(b(k), b(filaPivote));
        }

        // Eliminacion
        for (int i = k + 1; i < n; i++)
        {
            double factor = A(i, k) / A(k, k);

            for (int j = k; j < n; j++)
            {
                A(i, j) -= factor * A(k, j);
            }

            b(i) -= factor * b(k);
        }
    }

    // Sustitucion hacia atras
    VectorXd x(n);

    for (int i = n - 1; i >= 0; i--)
    {
        double suma = 0.0;

        for (int j = i + 1; j < n; j++)
        {
            suma += A(i, j) * x(j);
        }

        x(i) = (b(i) - suma) / A(i, i);
    }

    return x;
}

// ------------------------------------------------------------
// Programa principal
// ------------------------------------------------------------
int main()
{
    cout << fixed << setprecision(10);

    cout << "==========================================\n";
    cout << "PUNTO 2 - SISTEMA LINEAL 10x10\n";
    cout << "==========================================\n\n";

    MatrixXd A(10, 10);

    A <<
        2, 1, 0, 3, 2, 1, 0, 2, 1, 4,
        1, 3, 2, 0, 1, 4, 2, 1, 0, 2,
        0, 2, 4, 1, 3, 0, 1, 2, 4, 1,
        3, 0, 1, 5, 2, 1, 3, 0, 2, 1,
        2, 1, 3, 2, 6, 2, 1, 4, 0, 3,
        1, 4, 0, 1, 2, 5, 2, 1, 3, 0,
        0, 2, 1, 3, 1, 2, 4, 0, 2, 1,
        2, 1, 2, 0, 4, 1, 0, 5, 3, 2,
        1, 0, 4, 2, 0, 3, 2, 3, 6, 1,
        4, 2, 1, 1, 3, 0, 1, 2, 1, 5;

    // El enunciado no proporciona b.
    // Para poder comparar los metodos se asume:
    VectorXd b(10);
    b << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10;

    cout << "NOTA:\n";
    cout << "El enunciado no especifica el vector b.\n";
    cout << "Para completar el ejercicio se asume:\n\n";

    cout << "b =\n" << b << "\n\n";

    // --------------------------------------------------------
    // 1. Eliminacion Gaussiana
    // --------------------------------------------------------
    auto inicioGauss = chrono::high_resolution_clock::now();

    VectorXd xGauss = eliminacionGaussiana(A, b);

    auto finGauss = chrono::high_resolution_clock::now();

    double tiempoGauss =
        chrono::duration<double, micro>(finGauss - inicioGauss).count();

    // --------------------------------------------------------
    // 2. Factorizacion LU
    // --------------------------------------------------------
    auto inicioLU = chrono::high_resolution_clock::now();

    PartialPivLU<MatrixXd> lu(A);
    VectorXd xLU = lu.solve(b);

    auto finLU = chrono::high_resolution_clock::now();

    double tiempoLU =
        chrono::duration<double, micro>(finLU - inicioLU).count();

    // --------------------------------------------------------
    // 3. Factorizacion QR
    // --------------------------------------------------------
    auto inicioQR = chrono::high_resolution_clock::now();

    HouseholderQR<MatrixXd> qr(A);
    VectorXd xQR = qr.solve(b);

    auto finQR = chrono::high_resolution_clock::now();

    double tiempoQR =
        chrono::duration<double, micro>(finQR - inicioQR).count();

    // --------------------------------------------------------
    // Residuos
    // --------------------------------------------------------
    double residuoGauss = (A * xGauss - b).norm();
    double residuoLU = (A * xLU - b).norm();
    double residuoQR = (A * xQR - b).norm();

    // --------------------------------------------------------
    // Numero de condicion mediante SVD
    // --------------------------------------------------------
    JacobiSVD<MatrixXd> svd(A);

    VectorXd valoresSingulares = svd.singularValues();

    double condicion =
        valoresSingulares(0) /
        valoresSingulares(valoresSingulares.size() - 1);

    // --------------------------------------------------------
    // Resultados
    // --------------------------------------------------------
    cout << "------------------------------------------\n";
    cout << "SOLUCION - ELIMINACION GAUSSIANA\n";
    cout << "------------------------------------------\n";
    cout << xGauss << "\n\n";

    cout << "------------------------------------------\n";
    cout << "SOLUCION - FACTORIZACION LU\n";
    cout << "------------------------------------------\n";
    cout << xLU << "\n\n";

    cout << "------------------------------------------\n";
    cout << "SOLUCION - FACTORIZACION QR\n";
    cout << "------------------------------------------\n";
    cout << xQR << "\n\n";

    cout << "==========================================\n";
    cout << "COMPARACION NUMERICA\n";
    cout << "==========================================\n";

    cout << "Residuo Gauss = " << scientific << residuoGauss << "\n";
    cout << "Residuo LU    = " << scientific << residuoLU << "\n";
    cout << "Residuo QR    = " << scientific << residuoQR << "\n\n";

    cout << fixed;

    cout << "Tiempo Gauss = " << tiempoGauss << " microsegundos\n";
    cout << "Tiempo LU    = " << tiempoLU << " microsegundos\n";
    cout << "Tiempo QR    = " << tiempoQR << " microsegundos\n\n";

    cout << "Numero de condicion de A = "
         << condicion << "\n\n";

    cout << "Diferencia Gauss-LU = "
         << (xGauss - xLU).norm() << "\n";

    cout << "Diferencia Gauss-QR = "
         << (xGauss - xQR).norm() << "\n";

    cout << "Diferencia LU-QR = "
         << (xLU - xQR).norm() << "\n";

    cout << "\n==========================================\n";
    cout << "CONCLUSION\n";
    cout << "==========================================\n";

    cout << "Los tres metodos producen soluciones "
            "practicamente equivalentes.\n";

    cout << "Los residuos permiten evaluar la precision numerica "
            "de cada metodo.\n";

    cout << "LU es conveniente cuando se deben resolver varios "
            "sistemas con la misma matriz A.\n";

    cout << "QR suele ofrecer buena estabilidad numerica, aunque "
            "su costo computacional puede ser mayor.\n";

    return 0;
}