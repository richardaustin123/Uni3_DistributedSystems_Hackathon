#include <vector>
#include <stdexcept>
#include <thread>
#include <iostream>
#include <iomanip>
#include <condition_variable>

#include "MatrixOperations.h"
#include "FileWrite.h"

bool rowsFliped = false;
std::mutex mtx;
std::condition_variable cv;
int rowsProcessed = 0;

void printStartMatrix(std::vector<std::vector<double>> * srcMatrix);

void matrixOperationsInit(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix)
{
    int dim = srcMatrix->size();

    std::vector<std::vector<double>> op1Matrix(dim);
    std::vector<std::vector<double>> op2Matrix(dim);
    std::vector<std::vector<double>> op3Matrix(dim);

    int cpuCount = std::thread::hardware_concurrency();

    dstMatrix->resize(dim);

    for (int i = 0; i < dim; i++)
    {
        op1Matrix[i].resize(dim);
        op2Matrix[i].resize(dim);
        op3Matrix[i].resize(dim);
        dstMatrix->at(i).resize(dim);
    }

    // Print starting matrix 
    // printStartMatrix(srcMatrix);

    std::thread op1Thread(operation1, srcMatrix, &op1Matrix);
    // Start operation2 in a separate thread
    std::thread op2Thread(operation2, &op1Matrix, &op2Matrix);
    op1Thread.join();
    op2Thread.join();
    std::thread t3(operation3, &op2Matrix, &op3Matrix);
    t3.join();


    for (int i = 0; i < dim; i++)
    {
        dstMatrix->at(i).resize(dim);
        for (int j = 0; j < dim; j++)
        {
            dstMatrix->at(i).at(j) = op3Matrix.at(i).at(j);
        }
    }

    fileWrite("srcMatrix.txt", srcMatrix);
    fileWrite("op1Matrix.txt", &op1Matrix);
    fileWrite("op2Matrix.txt", &op2Matrix);
    fileWrite("op3Matrix.txt", &op3Matrix);
    fileWrite("dstMatrix.txt", dstMatrix);
}

// operation1 flip the rows to columns
// operation1 flip the rows to columns
void operation1(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix)
{
    // Loop through rows and columns of the source matrix and flip them 
    for (int row = 0; row < srcMatrix->size(); row++) {
        for (int col = 0; col < srcMatrix->at(row).size(); col++) {
            // dest matrix is now the source matrix flipped on the diagonal
            dstMatrix->at(col).at(row) = srcMatrix->at(row).at(col);
        }
        // Increment the number of processed rows
        rowsProcessed++;

        // Notify the condition variable if at least two rows have been processed
        if (rowsProcessed >= 2) {
            cv.notify_all();
        }
    }
}

// operation2 add the current element to all neighbours around it
// I renamed i and j to row and col so i can think and follow easier
void operation2(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix)
{
    std::unique_lock<std::mutex> lock(mtx);
    
    // Wait until at least two rows have been processed
    cv.wait(lock, []{ return rowsProcessed >= 2; });
    lock.unlock();

    // Loop through the source matrix and add the current element to all neighbours around it
    for (int col = 0; col < srcMatrix->size(); col++) {
        for (int row = 0; row < srcMatrix->size(); row++) {
            // Get the current element
            double currentElement = srcMatrix->at(row).at(col);

            // Iterate over the neighbours in row and column to add the current element to them
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    // Calculate the neighbour's position to add the current element to (the next element in the matrix)
                    int neighbourRow = row + i;
                    int neighbourCol = col + j;

                    // Check if the neighbour's is not outside of the matrix so that we dont get an out of bounds error
                    if (neighbourRow >= 0 && neighbourRow < srcMatrix->size() && neighbourCol >= 0 && neighbourCol < srcMatrix->size()) {
                        // Add the current element to the neighbour
                        dstMatrix->at(neighbourRow).at(neighbourCol) += currentElement;
                    }
                }
            }
        }
    }
}

// operation3 multiply the matrix by itself, row by column
// kept this to i j k in line with the powerpoint presentation
void operation3(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix)
{
    // Resize the destination matrix to the same size as the source matrix 
    dstMatrix->resize(srcMatrix->size(), std::vector<double>(srcMatrix->size(), 0));

    // Multiply the matrix by itself. Get a row, then go column by column
    for (int i = 0; i < srcMatrix->size(); i++) {           // row
        for (int j = 0; j < srcMatrix->size(); j++) {       // column
            for (int k = 0; k < srcMatrix->size(); k++) {   // inner loop
                // Multiply the row by the column and add it to the destination matrix
                dstMatrix->at(i).at(j) += srcMatrix->at(i).at(k) * srcMatrix->at(k).at(j); 
            }
        }
    }
}

void printStartMatrix(std::vector<std::vector<double>> * srcMatrix) {
    std::cout << "Starting matrix: " << std::endl;
    for (int i = 0; i < srcMatrix->size(); i++) {
        for (int j = 0; j < srcMatrix->at(i).size(); j++) {
            std::cout << srcMatrix->at(i).at(j);
        }
        std::cout << std::endl;
    }
}