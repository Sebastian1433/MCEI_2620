#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

using namespace Eigen;
using namespace std;

int main()
{
    cout << "========================================\n";
    cout << "PUNTO 2 - SISTEMA LINEAL 10x10\n";
    cout << "========================================\n\n";

    MatrixXd A(10,10);

    A <<
    2,1,0,3,2,1,0,2,1,4,
    1,3,2,0,1,4,2,1,0,2,
    0,2,4,1,3,0,1,2,4,1,
    3,0,1,5,2,1,3,0,2,1,
    2,1,3,2,6,2,1,4,0,3,
    1,4,0,1,2,5,2,1,3,0,
    0,2,1,3,1,2,4,0,2,1,
    2,1,2,0,4,1,0,5,3,2,
    1,0,4,2,0,3,2,3,6,1,
    4,2,1,1,3,0,1,2,1,5;

    cout << "Matriz A:\n";
    cout << A << "\n\n";

    cout << "NOTA:\n";
    cout << "El enunciado no especifica el vector b.\n";
    cout << "Por tanto, no se calcula una solucion x definitiva.\n";
    cout << "Se dejan preparadas las factorizaciones LU y QR.\n\n";

    // -----------------------------
    // FACTORIZACION LU
    // -----------------------------

    PartialPivLU<MatrixXd> lu(A);

    MatrixXd LU = lu.matrixLU();

    MatrixXd L = MatrixXd::Identity(10,10);
    MatrixXd U = MatrixXd::Zero(10,10);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            if(i > j)
                L(i,j) = LU(i,j);
            else
                U(i,j) = LU(i,j);
        }
    }

    cout << "Matriz L:\n";
    cout << L << "\n\n";

    cout << "Matriz U:\n";
    cout << U << "\n\n";

    MatrixXd P = lu.permutationP();

    double errorLU = (P*A - L*U).norm();

    cout << "Error LU ||P*A - L*U||:\n";
    cout << errorLU << "\n\n";

    // -----------------------------
    // FACTORIZACION QR
    // -----------------------------

    HouseholderQR<MatrixXd> qr(A);

    MatrixXd Q = qr.householderQ();

    MatrixXd R =
        qr.matrixQR().triangularView<Upper>();

    cout << "Matriz Q:\n";
    cout << Q << "\n\n";

    cout << "Matriz R:\n";
    cout << R << "\n\n";

    double errorQR = (A - Q*R).norm();

    cout << "Error QR ||A - Q*R||:\n";
    cout << errorQR << "\n\n";

    // -----------------------------
    // NUMERO DE CONDICION
    // -----------------------------

    JacobiSVD<MatrixXd> svd(A);

    VectorXd s = svd.singularValues();

    double condicion =
        s(0) / s(s.size()-1);

    cout << "Numero de condicion de A:\n";
    cout << condicion << "\n\n";

    cout << "PENDIENTE:\n";
    cout << "Cuando el profesor suministre el vector b,\n";
    cout << "se calcularan las soluciones mediante:\n";
    cout << "- Eliminacion Gaussiana\n";
    cout << "- LU\n";
    cout << "- QR\n";

    return 0;
}