#include <fstream>

#include "FileRead.h"

// Reading matrix from file
std::vector<std::vector<double>> fileRead(const std::string nomeFile, int dim)
{
    // Open file for reading
    std::ifstream file(nomeFile);

    // Declare matrix
    std::vector<std::vector<double>> matrix;

    // Check dimension parameter. If zero or negative, throw an error
    if (dim <= 0)
    {
        throw std::invalid_argument("Matrix dimension cannot be zero or negative");
    }

    // Read matrix size from size
    int dimFile;
    file >> dimFile;

    //Check if the file has enough data for the requested matrix
    if (dimFile < dim)
    {
        //otherwise throw an error
        throw std::invalid_argument("Matrix file does not contain enough info");
    }

    // Initialise matrix as requested by the input parameter
    matrix.resize(dim);

    // Read matrix - if the parameter requires a smaller matrix than the one provided in the file
    // The matrix read is only a subportion of the whole file
    // Loop through all the rows in the destination matrix
    for (int i = 0; i < dim; i++)
    {
        int temp;
        int j;

        // Resize the matrix row, so that it is ready to accept values form file
        matrix[i].resize(dim);

        // Loop through each element in the row, reading the value from file
        for (j = 0; j < dim; j++)
        {
            file >> matrix[i][j];
        }

        // If the file has more data than what is required from the matrix,
        // skip the remaining values and move to the next row
        for (; j < dimFile; j++)
        {
            file >> temp;
        }
    }

    // File close
    file.close();

    // Return the matrix read from file
    return matrix;
}