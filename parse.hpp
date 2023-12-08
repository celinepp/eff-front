// parse.hpp
#ifndef PARSE_HPP
#define PARSE_HPP

#include <string>
#include <vector>
#include <Eigen/Dense>

using Eigen::MatrixXd;

// Parse CSV file into asset names and time series data
void parseCSV(const std::string& filename, 
              MatrixXd & correlation);
std::vector<std::string> splitLine(std::string line, std::string delim);

#endif              
