#include <fstream>
#include <sstream>
#include <iostream>
#include "parse.hpp"

using std::string,std::vector, std::ifstream, std::cerr, std::stringstream;

// following method is from the vector docable
/**
 * @brief Split apart a line based upon delimiters in the string
 * 
 * @param line The string to be split
 * @param delim a string specifying the delimeter used to split apart the string
 * @return a vector of string objects
 */
std::vector<std::string> splitLine(std::string line, std::string delim) {
    std::vector<std::string> result;

    auto start = 0U;
    auto end = line.find(delim);
    while (end != std::string::npos) {
        result.push_back(line.substr(start, end - start));
        start = end + delim.length();
        end = line.find(delim, start);
    }
    std::string last = line.substr(start, end);
    if (last.length()) {
        result.push_back(last);
    }

    return result;
}

void parseCSV(const string& filename, 
              MatrixXd & correlation) {

  // Open input file
  ifstream infile(filename);
  if (!infile) {
    cerr << "Error opening file: " << filename << std::endl;
    exit(1);
  }
string line;
size_t numAssets = correlation.rows();
int rowsread = -1;

  // Read data rows
  while (getline(infile, line)) {
    rowsread ++;
    if (rowsread == numAssets){
        cerr<<"Too much data in file" <<"\n";
        exit(1);
    }
    vector<string> fields = splitLine(line,",");

    if (fields.size() != numAssets ) {
      cerr << "Invalid number of fields: " << line << "\n";
      exit(1);
    }

    for (size_t i = 0U; i < numAssets; i++) {
      try {
        double value = stod(fields[i]);
        correlation(rowsread,i) = value;
      }
      catch (const std::exception&) {
          cerr << "Invalid entry in the first data row: " << line << "\n";
          exit(1);
        }
      }
    
  }

  infile.close();
  if (rowsread != numAssets -1){
    cerr << "Too little data in the file" << "\n";
    exit(1);
  }
}
