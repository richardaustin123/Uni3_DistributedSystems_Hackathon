#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>

#include "RandomMatrixGenerator.h"
#include "FileRead.h"
#include "MatrixOperations.h"
#include "FileWrite.h"

// Define a matrix size parameter
// NOTE: This may be changed
#define DIM 200

int main()
{
    // Declare matrix size parameter
    int dim = DIM;

    // Declare initial and final matrices to store data before and after computations
    std::vector<std::vector<double>> srcMatrix(dim);
    std::vector<std::vector<double>> finalMatrix(dim);

    // Declare a vector to store time intervals from measurements
    std::vector<double> timeIntervals;

    srcMatrix = randomMatrixGenerator(dim); //generate random matrix
    //srcMatrix = fileRead("srcMatrix.txt", dim); //read matrix from file

    // Print out matrix size for user knowledge
    // std::cout << "Matrix size: " << srcMatrix.size() << std::endl;

    // Repeat the operations 10 times to get an averaged execution time
    for (int i = 0; i < 10; i++)
    {
        //print the iteration count
        // std::cout << "Iteration: " << i + 1 << std::endl;

        // Start the chronometer to measure execution time
        std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

        // Perform the operations
        matrixOperationsInit(&srcMatrix, &finalMatrix);

        // Stop the chronometer at the end of the operations to capture the execution time
        std::chrono::time_point<std::chrono::high_resolution_clock> stop = std::chrono::high_resolution_clock::now();

        // The execution time is computed as final time minus start time
        double interval = (std::chrono::duration<double, std::milli>(stop - start)).count();

        // The time interval is queued in a vector to allow for the computation of an average
        timeIntervals.push_back(interval);
    }

    matrixOperationsInit(&srcMatrix, &finalMatrix);

    // Compute the average of the time intervals measured earlier

    // Set a cumulative variable to 0 to sum all the intervals
    double sum = 0;

    // Loop through all the intervals in the vector and sum them in the cumulative variable above
    for (int i = 0; i < timeIntervals.size(); i++)
    {
        // std::cout << timeIntervals[i] << " ms" << std::endl;
        sum += timeIntervals[i];
    }

    // Print the average value of all the intervales
    std::cout << "Average: " << sum / timeIntervals.size() << " ms" << std::endl;

    // Terminate execution without errors
    return 0;
}
