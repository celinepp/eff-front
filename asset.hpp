#ifndef ASSET_HPP
#define ASSET_HPP

#include <vector>
#include <string>

// Define a struct to represent an asset
struct Asset {
    std::string name;
    double averageReturn;
    double standardDeviation;
    std::vector<double> returns;  // Add a vector to store returns over time
};

// Function to calculate rate of return
double calculateRateOfReturn(double Pn, double Pn1);

// Function to calculate average return
double calculateAverageReturn(const std::vector<double>& returns);

// Function to calculate volatility
double calculateVolatility(const std::vector<double>& returns);

// Function to calculate covariance
double calculateCovariance(const std::vector<double>& returnsA, const std::vector<double>& returnsB);

// Function to calculate correlation
double calculateCorrelation(const std::vector<double>& returnsA, const std::vector<double>& returnsB);

#endif // ASSET_HPP
