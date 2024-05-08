#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

// Template class allows for many type of values in teh source matrix.
// Any type of value supported by std::ofstream is allowed, including int and double
// In general this is a method to avoid conding the same function multiple times where
// the only change is the type of a parameter
template <class type>
void fileWrite(const std::string filename, const std::vector<std::vector<type>>* srcMatrix)
{
    // Open file to write
    std::ofstream file(filename);

    // Write matrix size
    file << srcMatrix->size() << std::endl;

    // Write matrix
    // looping through all the rows
    for (int i = 0; i < srcMatrix->size(); i++)
    {
        // Looping through all the elemtns in the row
        for (int j = 0; j < srcMatrix->at(i).size(); j++)
        {
            // Printing the value to the file with a fixed width of two digits, so the matrix shows nicely in the text file
            file << std::setw(2) << srcMatrix->at(i).at(j) << " ";
        }
        // At the end of each row insert a new line
        file << std::endl;
    }

    // Close the file
    file.close();
}

