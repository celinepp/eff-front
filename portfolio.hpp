#ifndef PORTFOLIO_HPP
#define PORTFOLIO_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>
#include "asset.hpp"

class Portfolio {
  public:
    int n;//the number of Assets
    Eigen::VectorXd weights;//a vector of weights
    std::vector<Asset> assets;//a list of Assets
    double volatility;//portfolio volatility
    double rateOfreturn;//rate of return
    Eigen::MatrixXd Correlation;//correlation matrix
  public:
    Portfolio(){}
    double unrestricted(double targetReturn);
    double restricted(double targetReturn);
    void calculateRateofReturn(){
      int sum = 0;
      for(int i=0;i<n;i++){
        sum += weights(i)*assets[i].averageReturn;
      }
      rateOfreturn = sum;
    }
    void calculateVolatility(){     
      double sum = 0;
      for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
          sum += weights(i)*weights(j)*Correlation(i,j)*assets[i].standardDeviation*assets[j].standardDeviation;
        }
      }
      volatility = std::sqrt(sum);
    }
    Eigen::MatrixXd calculateCovarianceMatrix(const Portfolio &P) {
    int n = P.n;
    Eigen::MatrixXd Cov(n, n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            Cov(i, j) = P.assets[i].standardDeviation *
                        P.Correlation(i, j) *
                        P.assets[j].standardDeviation;
        }
    }

    return Cov;
} 
    ~Portfolio(){}
};

#endif // PORTFOLIO_HPP
