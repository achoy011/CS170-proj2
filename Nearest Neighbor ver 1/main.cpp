/* 
 * File:   main.cpp
 * Author: Arthur Choy
 * Created on November 22, 2022, 3:14 PM
 */

// Version 1 - Added File I/O

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    vector<vector<double>> data;
    
    string fileName;
    ifstream file;
    cout << "Please enter the file name (do not include file extension): ";
    cin >> fileName;
    fileName += ".txt";
    
    file.open(fileName);
    
    string input;
    //https://discuss.codechef.com/t/input-multiple-integers-in-c-line-in-single-line-with-whitespace/21355/2
    while(getline(file, input)) {
        vector<double> tempVec;
        double tempNum;
        istringstream ss(input);
        while(ss >> tempNum) tempVec.push_back(tempNum);
        data.push_back(tempVec);
    }
    
    file.close();
    
    for(int entry = 0; entry < data.size(); entry++) {
        for(int ele = 0; ele < data[entry].size(); ele++) {
            cout << data[entry][ele] << " ";
        }
        cout << endl;
    }
    
    return 0;
}

