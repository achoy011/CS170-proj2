/* 
 * File:   main.cpp
 * Author: Arthur Choy
 * Created on November 22, 2022, 3:14 PM
 */

// VERSION 2
/*
 * Added nearest neighbor algorithm (and tested it somewhat as well)
 * Added test input and output for nearest neighbor algorithm
 * Removed need to put in file name for now, until I finalize everything
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
using namespace std;

int findNearestNeighbor(vector<double>, vector<vector<double>>);

/*
 * 
 */
int main(int argc, char** argv) {
    vector<vector<double>> data;    // Data vector
    
    // Input file objects
    string fileName;
    ifstream file;
    //cout << "Please enter the file name (do not include file extension): ";
    //cin >> fileName;
    //fileName += ".txt";
    
    // Open file (I should have file validation but I don't feel like it at the moment)
    fileName = "CS170_Small_Data__1.txt";
    file.open(fileName);
    
    // Input string to get from file
    string input;
    //https://discuss.codechef.com/t/input-multiple-integers-in-c-line-in-single-line-with-whitespace/21355/2
    while(getline(file, input)) {
        vector<double> tempVec;
        double tempNum;
        istringstream ss(input);
        while(ss >> tempNum) tempVec.push_back(tempNum);
        data.push_back(tempVec);
    }
    
    // Close file once all data has been read
    file.close();
    
    // Test output data
    /*for(int entry = 0; entry < data.size(); entry++) {
        for(int ele = 0; ele < data[entry].size(); ele++) {
            cout << data[entry][ele] << " ";
        }
        cout << endl;
    }*/
    
    // Get test vector to test nearest neighbor algorithm
    vector<double> test;
    cout << "Inputting: ";
    for(int i = 0; i < data[0].size(); i++) {
        double tempIn;
        cin >> tempIn;
        test.push_back(tempIn);
    }
    
    int testVal = findNearestNeighbor(test, data);
    cout << "Nearest Neighbor: " << testVal << endl;
    
    
    return 0;
}

int findNearestNeighbor(vector<double> entry, vector<vector<double>> data) {
    int nearest; // Return value
    //https://stackoverflow.com/questions/5834635/how-do-i-get-double-max
    double nearestDist = numeric_limits<double>::max(); // Distance of closest checked element
    
    double tempSum; // Counter to hold values for distance calculation
    
    // For each entry in data, determine its distance from entry
    for(int i = 0; i < data.size(); i++) {
        tempSum = 0;
        for(int j = 1; j < data[i].size(); j++) {
            tempSum += pow(entry[j] - data[i][j], 2);
        }
        tempSum = sqrt(tempSum);
        
        // If it's closest so far, update
        if(tempSum < nearestDist) {
            nearest = i;
            nearestDist = tempSum;
        }
    }
    
    return nearest; // Return calculated nearest neighbor
}

