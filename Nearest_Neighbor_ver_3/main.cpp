/* 
 * File:   main.cpp
 * Author: Arthur Choy
 * Created on November 22, 2022, 3:14 PM
 */

// VERSION 3
/* Implemented k-fold cross validation for all features
 * Next version I need to add a feature filter
 * NOTE: IF VERSION 4 or beyond IS MESSED UP, IT MOST LIKELY ORIGINATED HERE
 * as if I'll need to reference (or will remember to reference) the above message
 */

// Turns out I put cmath twice, whoops
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

int findNearestNeighbor(vector<double>, vector<vector<double>>);
double kFold(vector<vector<double>>, vector<int>);

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
    /*vector<double> test;
    cout << "Inputting: ";
    for(int i = 0; i < data[0].size(); i++) {
        double tempIn;
        cin >> tempIn;
        test.push_back(tempIn);
    }
    
    int testVal = findNearestNeighbor(test, data);
    cout << "Nearest Neighbor: " << testVal << endl;*/
    
    // Test k-fold cross validation with all features
    vector<int> temp;
    cout << "Calculated accuracy: " << kFold(data, temp)*100.0 << "%" << endl; 
    
    
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

// For the sake of simplicity (though to the detriment of time), K = n
double kFold(vector<vector<double>> data, vector<int> features) {
    vector<vector<double>> subData;
    double accuracyCount = 0;
    for(int i = 0; i < data.size(); i++) {
        // Create subdata vector, and extract K section (entryK)
        subData.assign(data.begin(), data.end());
        vector<double> entryK = subData[i];
        subData.erase(subData.begin()+i);
        
        // Find the nearest neighbor of the K section
        // If their classes match, then it was a successful classification
        int a = findNearestNeighbor(entryK, subData);
        if(entryK[0] == subData[a][0]) accuracyCount += 1;
    }
    
    // Return accurate classifications/total classifications
    return accuracyCount/static_cast<double>(data.size());
}