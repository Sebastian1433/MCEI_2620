#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

using namespace std;
using namespace Eigen;

int main()
{
    cout << "==========================================" << endl;
    cout << "PUNTO 5 - MINIMOS CUADRADOS" << endl;
    cout << "==========================================" << endl;

    // El ejecutable se ejecutara desde la carpeta build,
    // por eso el archivo CSV esta un nivel arriba.
    ifstream archivo("../datos_convertidor_realista.csv");

    if (!archivo.is_open())
    {
        cerr << "Error: no se pudo abrir el archivo CSV." << endl;
        return 1;
    }

    string linea;

    // Saltar encabezado
    getline(archivo, linea);

    vector<double> voltaje;
    vector<double> corriente;
    vector<double> temperatura;
    vector<double> potencia;

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string campo;
        vector<double> fila;

        while (getline(ss, campo, ','))
        {
            fila.push_back(stod(campo));
        }

        if (fila.size() == 4)
        {
            voltaje.push_back(fila[0]);
            corriente.push_back(fila[1]);
            temperatura.push_back(fila[2]);
            potencia.push_back(fila[3]);
        }
    }

    archivo.close();

    int n = potencia.size();

    cout << "Numero de muestras cargadas: " << n << endl;

    // Matriz de diseno X
    MatrixXd X(n, 4);

    // Vector de potencia medida
    VectorXd y(n);

    for (int i = 0; i < n; i++)
    {
        X(i, 0) = 1.0;
        X(i, 1) = voltaje[i];
        X(i, 2) = corriente[i];
        X(i, 3) = temperatura[i];

        y(i) = potencia[i];
    }

    // =====================================================
    // SOLUCION POR QR
    // =====================================================

    VectorXd beta_qr = X.colPivHouseholderQr().solve(y);

    cout << endl;
    cout << "Coeficientes calculados mediante QR:" << endl;

    cout << "beta0 = " << beta_qr(0) << endl;
    cout << "beta1 (Voltaje) = " << beta_qr(1) << endl;
    cout << "beta2 (Corriente) = " << beta_qr(2) << endl;
    cout << "beta3 (Temperatura) = " << beta_qr(3) << endl;

    // =====================================================
    // PREDICCIONES Y RESIDUOS
    // =====================================================

    VectorXd y_pred = X * beta_qr;
    VectorXd residuos = y - y_pred;

    double mse = residuos.squaredNorm() / n;
    double norma_residuos = residuos.norm();
    double max_residuo = residuos.cwiseAbs().maxCoeff();

    cout << endl;
    cout << "Resultados del ajuste:" << endl;
    
    cout << "\nVector residual r = y - X*beta:\n";
    cout << residuos << endl;
    
    cout << "Norma de residuos = "
         << norma_residuos << endl;

    cout << "MSE = "
         << mse << endl;

    cout << "Maximo residuo absoluto = "
         << max_residuo << endl;

    // =====================================================
    // NUMERO DE CONDICION DE X
    // =====================================================

    JacobiSVD<MatrixXd> svd(X);

    VectorXd valores_singulares = svd.singularValues();

    double condicion =
        valores_singulares(0) /
        valores_singulares(valores_singulares.size() - 1);

    cout << "Numero de condicion de X = "
         << condicion << endl;

    // =====================================================
    // ECUACIONES NORMALES
    // beta = (X^T X)^-1 X^T y
    // =====================================================

    MatrixXd XtX = X.transpose() * X;
    VectorXd Xty = X.transpose() * y;

    VectorXd beta_normal =
        XtX.ldlt().solve(Xty);

    cout << endl;
    cout << "Coeficientes mediante ecuaciones normales:" << endl;

    cout << "beta0 = " << beta_normal(0) << endl;
    cout << "beta1 = " << beta_normal(1) << endl;
    cout << "beta2 = " << beta_normal(2) << endl;
    cout << "beta3 = " << beta_normal(3) << endl;

    // Comparacion entre ambos metodos
    double diferencia =
        (beta_qr - beta_normal).norm();

    cout << endl;
    cout << "Diferencia entre QR y ecuaciones normales = "
         << diferencia << endl;

    cout << endl;
    cout << "Modelo obtenido:" << endl;

    cout << "P = "
         << beta_qr(0)
         << " + (" << beta_qr(1) << ") V"
         << " + (" << beta_qr(2) << ") I"
         << " + (" << beta_qr(3) << ") T"
         << endl;

    return 0;
}