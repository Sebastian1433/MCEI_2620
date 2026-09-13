#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

using namespace Eigen;
using namespace std;

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

    return svd.matrixV() * singular_inv.asDiagonal() * svd.matrixU().transpose();
}

int main()
{
    MatrixXd A(4,3);

    A << 1,  0,  2,
         2, -1,  5,
         0,  1, -1,
         1,  3, -1;

    MatrixXd Ap = pseudoInverse(A);

    cout << "Matriz A:\n" << A << "\n\n";

    cout << "Pseudoinversa A+:\n" << Ap << "\n\n";

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
    cout << Ap * A << "\n";

    return 0;
}