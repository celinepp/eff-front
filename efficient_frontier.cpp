#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include "portfolio.hpp"
#include "asset.hpp"
#include "parse.hpp"
#include <unistd.h>
#include <iomanip>
using namespace Eigen;
using std::vector, std::string;


// Function to read asset data from a file
std::vector<Asset> readAssetData(const std::string& filename) {
    std::vector<Asset> assets;
    std::ifstream file(filename);
    
    if(!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        exit(1);
    }
    
    std::string line;
    
    while(std::getline(file, line)) {
        vector<string> fields = splitLine(line, ",");
        if (fields.size() !=3){
            std::cerr << "Invalid asset data format" << std::endl;
            exit(1);   
        }
        Asset asset;
        asset.name = fields[0];
        try {
           asset.averageReturn =stod(fields[1]);
           asset.standardDeviation =stod(fields[2]);
        }
        catch (const std::exception&) {
            std::cerr << "Invalid entry in the first data row: " << line << "\n";
            exit(1);
      }
        
        assets.push_back(asset);
    }
    
    return assets;
}

// Function to read correlation matrix from a file
MatrixXd readCorrelationMatrix(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::vector <double> > correlationData;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<double> row;
        double value;
        while (iss >> value) {
            row.push_back(value);
        }

        if (row.empty() || iss.fail()) {
            std::cerr << "Error: Invalid data format in file " << filename << std::endl;
            exit(EXIT_FAILURE);
        }

        correlationData.push_back(row);
    }

    MatrixXd correlationMatrix(correlationData.size(), correlationData[0].size());
    for (size_t i = 0; i < correlationData.size(); ++i) {
        for (size_t j = 0; j < correlationData[i].size(); ++j) {
            correlationMatrix(i, j) = correlationData[i][j];
        }
    }

    return correlationMatrix;
}

double Portfolio::unrestricted(double targetReturn) {
    int n = this->n;

    // Construct matrix A
    MatrixXd A = MatrixXd::Zero(n + 2, n + 2);

    // Copy over data from the covariance matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A(i, j) = this->Correlation(i, j) * this->assets[i].standardDeviation * this->assets[j].standardDeviation;
        }
    }

    // Use loops to set 1's in the rows and columns
    for (int i = 0; i < n; i++) {
        A(n, i) = 1;
        A(n+1,i) = this->assets[i].averageReturn;
        A(i, n) = 1;
        A(i,n+1) = this->assets[i].averageReturn;
    }

    A(n, n) = 0;
    A(n + 1, n) = 0;

 

    // Set the vector b
    VectorXd b = VectorXd::Zero(n + 2);
    b(n) = 1;
    b(n + 1) = targetReturn;

    // Solve the equation Ax = b
    VectorXd X = A.colPivHouseholderQr().solve(b);

    this->weights = X.head(n);

    // Calculate and return portfolio volatility
    this->calculateVolatility();
    return this->volatility;
}

double Portfolio::restricted(double targetReturn) {
    int n = this->n;

    // Construct matrix A
    MatrixXd A = MatrixXd::Zero(n + 2, n + 2);

    // Copy over data from the covariance matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A(i, j) = this->Correlation(i, j) * this->assets[i].standardDeviation * this->assets[j].standardDeviation;
        }
    }

    // Use loops to set 1's in the rows and columns
    for (int i = 0; i < n; i++) {
        A(n, i) = 1;
        A(i, n) = 1;
    }

    A(n, n) = 0;
    A(n + 1, n) = 0;

    // Set the vector b
    VectorXd b = VectorXd::Zero(n + 2);
    b(n) = 1;
    b(n + 1) = targetReturn;

    // Solve the equation with constraints
    VectorXd X = A.fullPivHouseholderQr().solve(b);

    MatrixXd AA = A;

    // Loop until there are no new negative weights
    for (int i = 1;; i++) {
        int flag = 1;

        // Constraint matrix
        MatrixXd C;

        for (int j = 0; j < n; j++) {
            if (X(j) < 0) {
                // Use a small value instead of 0 to avoid numerical issues
                MatrixXd T = MatrixXd::Zero(n, 1);
                T(j, 0) = 1;
                MatrixXd temp = C;
                C.resize(n, C.cols() + 1);
                if (C.cols() == 1) {
                    C << T;
                } else {
                    C << temp, T;
                }
                flag = 0;
            }
        }

        if (flag == 1) {
            break;
        }

        // Add constraints to matrix A
        MatrixXd T = AA;
        AA.resize(T.rows() + C.cols(), n);
        AA << T, C.transpose();

        // Add constraints to vector b
        b.resize(T.rows() + C.cols());
        //b << b, MatrixXd::Zero(C.cols());

        // Solve the equation with constraints
        X = AA.fullPivHouseholderQr().solve(b);
    }

    // Extract weights from the solution
    this->weights = X.head(n);

    // Calculate and return portfolio volatility
    this->calculateVolatility();
    return this->volatility;
}

int main(int argc, char **argv) {
    // Parse command line arguments
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <asset_file> <correlation_matrix_file>" << std::endl;
        return EXIT_FAILURE;
    }
    // Check -r
    bool restrictedMode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-r") {
            restrictedMode = true;
            break;
        }
    }


    // Read asset data
    std::vector<Asset> assets = readAssetData(argv[1]);


    // Read correlation matrix
    MatrixXd correlationMatrix(assets.size(), assets.size());
    parseCSV(argv[2], correlationMatrix);




    
    Portfolio p;
    p.assets = assets;
    p.Correlation = correlationMatrix;
    p.n = assets.size();

    MatrixXd covMatrix = p.calculateCovarianceMatrix(p);


    std::cout << "ROR,volatility" << std::endl;
    std::cout.setf(std::ios::fixed);



    bool r = false;

    if (!r) {
        for (double l = 0.01; l <= 0.265; l += 0.01) {
            double targetReturn = l;
            std::cout << std::fixed << std::setprecision(1) << targetReturn * 100 << "%,";
            std::cout << std::fixed << std::setprecision(2) << p.unrestricted(targetReturn) * 100 << "%" << std::endl;
        }
    }

    // Restricted 
    if (r) {
        for (double l = 0.01; l <= 0.265; l += 0.01) {
            double targetReturn = l;
            std::cout << std::fixed << std::setprecision(1) << targetReturn * 100 << "%,";
            std::cout << std::fixed << std::setprecision(2) << p.restricted(targetReturn) * 100 << "%" << std::endl;
        }
    }

    std::cout.unsetf(std::ios::fixed);
    return EXIT_SUCCESS;
}