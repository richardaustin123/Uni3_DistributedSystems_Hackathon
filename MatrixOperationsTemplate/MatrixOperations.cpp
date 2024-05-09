#include <vector>
#include <thread>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <condition_variable>

#include "MatrixOperations.h"
#include "FileWrite.h"
#include "ThreadPool.hpp"

std::mutex mtx;
std::condition_variable cv;
std::condition_variable cv2;
bool ready = false;
bool ready2 = false;
ThreadPool pool( std::thread::hardware_concurrency());

void matrixOperationsInit(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix)
{

    int dim = srcMatrix->size();

    std::vector<std::vector<double>> op1Matrix(dim);
    std::vector<std::vector<double>> op2Matrix(dim);
    std::vector<std::vector<double>> op3Matrix(dim);

    dstMatrix->resize(dim);

    for (int i = 0; i < dim; i++)
    {
        op1Matrix[i].resize(dim);
        op2Matrix[i].resize(dim);
        op3Matrix[i].resize(dim);
        dstMatrix->at(i).resize(dim);
    }

    // 3 threads, each for one operation, run in parallel to speed up the process
    std::thread t1(operation1, srcMatrix, &op1Matrix);
    std::thread t2(operation2, &op1Matrix, &op2Matrix);
    std::thread t3(operation3, &op2Matrix, &op3Matrix);
    
    // joins after the threads for parallel operations 
    // join waits for the threads to finish before continuing
    t1.join();
    t2.join();
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

// operation 1
// Flip the matrix rows to columns
void operation1(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>>* dstMatrix) {
    int matrixSize = srcMatrix->size();

    // Loop through rows and columns of the source matrix so we can flip them
    for (int row = 0; row < matrixSize; row++) {
        // Set ready cv to false so we can wait for the operation to finish
        ready = false;
        // Lock the mutex to prevent matrix from being accessed by the other threads ()
        std::unique_lock<std::mutex> lock(mtx);
        // add the operation to the thread pool
        // (I did a similar thing in a practice at the weekend which was taught from https://codereview.stackexchange.com/questions/229560/implementation-of-a-thread-pool-in-c)
        pool.enqueue(([ = ]() {
            // loop through the columns of the matrix to flip them
            for (int col = 0; col < matrixSize; col++) {
                // simply change the row and columns
                dstMatrix->at(row).at(col) = srcMatrix->at(col).at(row);
            }
        }));
    }

    // set ready to true so op2 knows to start as the flip is complete
    ready = true;
    // notify op2 thread (lab 6)
    cv.notify_all();
}

// operation 2
// Add the current element to all neighbours around it
void operation2(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix)
{
    // set matrix size (same as dim but easier to read) 
    int matrixSize = srcMatrix->size();

    // loop through rows 
    for (int row = 0; row < matrixSize; row++) {
        // second condition variable false until operation complete
        ready2 = false;
        // lock the mutex stopping other threads from accessung
        std::unique_lock<std::mutex> lock(mtx);
        // wait for the op1 to finish by checking the ready condition
        while (!ready) {
            cv.wait(lock);
        }
        // Add the operation to the thread pool to run in parallel
        pool.enqueue(([ = ]() {
            // loop through the columns of the matrix to add the current element to all neighbours around it
            for (int col = 0; col < matrixSize; col++) {
                    // neighboursAdded i sum current element and all neighbours around it
                    double neighboursAdded = 0;
                    // Loop through the neighbours around the current element to add them
                    for (int neighbourRow = row - 1; neighbourRow <= row + 1; neighbourRow++) {
                        for (int neighbourCol = col - 1; neighbourCol <= col + 1; neighbourCol++) {
                            // if the neighbour is within the matrix bounds (not top, bottom, left, right) add it to the sum
                            if (neighbourRow >= 0 && neighbourRow < matrixSize && neighbourCol >= 0 && neighbourCol < matrixSize) {
                                // add the neighbour to the neighboursAdded sum
                                neighboursAdded += srcMatrix->at(neighbourRow).at(neighbourCol);
                            }
                        }
                    }
                    // set the current element in new matrix to the sum of the current element and all neighbours
                    dstMatrix->at(row).at(col) = neighboursAdded;
            }
        }));
        // set ready2 to true so op3 knows to start
        ready2 = true;
        // Notify the treads that the operation is complete
        cv2.notify_all();
    }
}

// operation 3
// Multiply the matrix rows by columns
void operation3(std::vector<std::vector<double>> * srcMatrix, std::vector<std::vector<double>> * dstMatrix) {
    // set matrix size again 
    int matrixSize = srcMatrix->size();
    // loop through the rows of the matrix so that we can multiply the rows by the columns
    for (int row = 0; row < matrixSize; row++) {
        // set lock to prevent other threads from accessing the matrix
        std::unique_lock<std::mutex> lock(mtx);
        // wait for the operation 2 to finish before starting
        while (!ready2) {
            cv2.wait(lock);
        }

        // add the operation to the thread pool to run in parallel
        pool.enqueue([=]() {
            // Loop through the columns of the matrix to multiply the rows by the columns
            for (int col = 0; col < matrixSize; col++) {
                for (int cell = 0; cell < matrixSize; cell++) {
                    // Multiply the row by the column and add it to the destination matrix
                    dstMatrix->at(row).at(col) += srcMatrix->at(row).at(cell) * srcMatrix->at(cell).at(col);
                }
            }
        });
    }
}




