/* 
 * File:   main.cpp
 * Author: Arthur Choy
 * Created on November 22, 2022, 3:14 PM
 */

// VERSION 6
/*
 * so the searches from v5 aren't producing the correct result and my only guess
 * is that i didnt normalize the data.
 */

// System Libraries
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <ctime>
using namespace std;

// Function Prototypes
int findNearestNeighbor(vector<double>, vector<vector<double>>, vector<int>);
double kFold(vector<vector<double>>, vector<int>);
pair<vector<int>, double> fowardSelection(vector<vector<double>>);
pair<vector<int>, double> backwardsElimination(vector<vector<double>>);

// Helper Functions
void reportFeatures(vector<int>);
vector<vector<double>> zNormalize(vector<vector<double>>);
double mean(vector<double>);
double standardDev(vector<double>, double);

// Global variables (for testing output only)
bool outputOnce = false;

/*
 * 
 */
int main(int argc, char** argv) {
    vector<vector<double>> data;    // Data vector
    
    // Input file objects
    string fileName;
    ifstream file;
    
    // File validation
    do {
        cout << "Please enter the file name (do not include file extension): ";
        cin >> fileName;
        fileName += ".txt";

        //fileName = "CS170_Small_Data__96.txt";
        
        // Open file
        file.open(fileName);
        if(!file) cout << "Could not find file " << fileName << "." << endl;
    } while(!file); // If it doesn't open, loop
    
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
    
    data = zNormalize(data);
    
    cout << "The dataset has " << data[0].size()-1 << " features (not including class attribute), with " << data.size() << " instances." << endl;
    
    // Test k-fold cross validation with all features
    vector<int> temp;
    for(int i = 1; i < data[0].size(); i++) temp.push_back(i);
    cout << "With all features using \"Leaving-one-out\" evaluation, accuracy is: " << kFold(data, temp)*100.0 << "%" << endl;
    
    int algo;
    cout << "1 - Forward Selection" << endl
         << "2 - Backward Elimination" << endl
         << "Please enter the number corresponding to the algorithm you want to run: ";
    cin >> algo;
    
    int start = time(0);
    if(algo == 1) {
        cout << "Performing Forwards Selection Search" << endl;

        pair<vector<int>, double> result = fowardSelection(data);

        cout << "Finished search! The best feature set was ";
        reportFeatures(result.first);
        cout << ", which has an accuracy of " << result.second*100.0 << "%" << endl;
    }
    else if(algo == 2) {
        cout << endl << "Performing Backwards Elimination Search" << endl;

        pair<vector<int>, double> result = backwardsElimination(data);

        cout << "Finished search! The best feature set was ";
        reportFeatures(result.first);
        cout << ", which has an accuracy of " << result.second*100.0 << "%" << endl;
    }
    else cout << "That is not a valid algorithm selection!" << endl;
    int stop = time(0);
    
    cout << "Time: " << stop-start << " seconds" << endl;
    
    return 0;
}

/*
 * =====================================
 *         ALGORITHM FUNCTIONS
 * =====================================
 */
int findNearestNeighbor(vector<double> entry, vector<vector<double>> data, vector<int> features) {
    int nearest; // Return value
    //https://stackoverflow.com/questions/5834635/how-do-i-get-double-max
    double nearestDist = numeric_limits<double>::max(); // Distance of closest checked element
    
    double tempSum; // Counter to hold values for distance calculation
    
    // For each entry in data, determine its distance from entry
    for(int i = 0; i < data.size(); i++) {
        tempSum = 0;
        for(int j = 1; j < data[i].size(); j++) {
            //if(!outputOnce) cout << "Feature find in progress: " << j << " found? " << findFeature(features, j) << endl;
            if(find(features.begin(), features.end(), j) != features.end()) {
                tempSum += pow(entry[j] - data[i][j], 2);
            }
        }
        tempSum = sqrt(tempSum);
        
        // If it's closest so far, update
        if(tempSum < nearestDist) {
            nearest = i;
            nearestDist = tempSum;
        }
        outputOnce = true;
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
        int a = findNearestNeighbor(entryK, subData, features);
        if(entryK[0] == subData[a][0]) accuracyCount += 1;
    }
    
    // Return accurate classifications/total classifications
    return accuracyCount/static_cast<double>(data.size());
}

pair<vector<int>, double> fowardSelection(vector<vector<double>> data) {
    cout << "Beginning Forward Selection Search" << endl << endl;
    vector<int> unAddedFeatures, bestSet, bestSetInTier, bestSetInNext;
    double bestAccuracy, bestAccuracyInNext;
    int featureAdded;
    // Initialize unadded features (all features)
    for(int i = 1; i < data[0].size(); i++) unAddedFeatures.push_back(i);
    
    // Set up initial state in search
    vector<int> fillerVec;
    bestAccuracy = kFold(data, fillerVec);
    
    // For all features
    for(int i = 1; i < data[0].size(); i++) {
        // Reset best accuracy
        bestAccuracyInNext = 0;
        // Search through all possible features to add
        for(int j = 0; j < unAddedFeatures.size(); j++) {
            vector<int> tempFeatures; // Vector to hold one possible feature to add to last tier's best
            // Copy last tier's best
            tempFeatures.assign(bestSetInTier.begin(), bestSetInTier.end());
            // Add unadded feature j
            tempFeatures.push_back(unAddedFeatures[j]);
            
            // Calculate accuracy
            double tempAccuracy = kFold(data, tempFeatures);
            
            /*
             * OUTPUT INDIVIDUAL RESULTS
             */
            cout << "\tUsing feature(s) ";
            reportFeatures(tempFeatures);
            cout << " accuracy is " << tempAccuracy*100.0 << "%" << endl;
            
            // If accuracy is best so far in working tier, update
            if(tempAccuracy > bestAccuracyInNext) {
                bestAccuracyInNext = tempAccuracy;
                bestSetInNext.assign(tempFeatures.begin(), tempFeatures.end());
                featureAdded = unAddedFeatures[j];
            }
        }
        // Remove added feature from unadded set
        vector<int>::iterator tempIt = find(unAddedFeatures.begin(), unAddedFeatures.end(), featureAdded);
        unAddedFeatures.erase(tempIt);
        
        // Update best accuracy/set, else warn user of accuracy decrease
        if(bestAccuracyInNext > bestAccuracy) {
            bestAccuracy = bestAccuracyInNext;
            bestSet.assign(bestSetInNext.begin(), bestSetInNext.end());
        }
        else cout << endl << "Accuracy has decreased! Continuing search for local maxima.";
        
        // Report best set in tier
        cout << endl << "Feature set ";
        reportFeatures(bestSetInNext);
        cout << " was best, accuracy is " << bestAccuracyInNext*100.0 << "%" << endl << endl;
        
        bestSetInTier = bestSetInNext;
    }
    
    // Return result
    return pair<vector<int>, double>(bestSet, bestAccuracy);
}

pair<vector<int>, double> backwardsElimination(vector<vector<double>> data) {
    cout << "Beginning Backwards Elimination Search" << endl << endl;
    vector<int> addedFeatures, bestSet, bestSetInTier, bestSetInNext;
    double bestAccuracy, bestAccuracyInNext;
    int featureRemoved;
    // Initialize added features (all features)
    for(int i = 1; i < data[0].size(); i++) addedFeatures.push_back(i);
    
    // Set up initial state in search
    bestAccuracy = kFold(data, addedFeatures);
    bestSetInTier.assign(addedFeatures.begin(), addedFeatures.end());
    
    // For all features
    for(int i = 1; i < data[0].size(); i++) {
        // Reset best accuracy
        bestAccuracyInNext = 0;
        // Search through all possible features to remove
        // Special case if testing empty set
        if(addedFeatures.size() == 1) {
            vector<int> tempFeatures;
            double tempAccuracy = kFold(data, tempFeatures);
            
            cout << "\tUsing no features, accuracy is " << tempAccuracy*100.0 << "%" << endl;
            
            if(tempAccuracy > bestAccuracyInNext) {
                bestAccuracyInNext = tempAccuracy;
                bestSetInNext.clear();
            }
            
            addedFeatures.clear();
        }
        else {
            for(int j = 0; j < addedFeatures.size(); j++) {
                vector<int> tempFeatures; // Vector to hold one possible feature to remove from last tier's best
                // Copy last tier's best
                tempFeatures.assign(bestSetInTier.begin(), bestSetInTier.end());
                // Remove added feature j
                tempFeatures.erase(find(tempFeatures.begin(), tempFeatures.end(), addedFeatures[j]));

                // Calculate accuracy
                double tempAccuracy = kFold(data, tempFeatures);

                /*
                 * OUTPUT INDIVIDUAL RESULTS
                 */
                cout << "\tUsing feature(s) ";
                reportFeatures(tempFeatures);
                cout << " accuracy is " << tempAccuracy*100.0 << "%" << endl;

                // If accuracy is best so far in working tier, update
                if(tempAccuracy > bestAccuracyInNext) {
                    bestAccuracyInNext = tempAccuracy;
                    bestSetInNext.assign(tempFeatures.begin(), tempFeatures.end());
                    featureRemoved = addedFeatures[j];
                }
            }
            // Remove removed feature from added set
            vector<int>::iterator tempIt = find(addedFeatures.begin(), addedFeatures.end(), featureRemoved);
            addedFeatures.erase(tempIt);
        }
        
        // Update best accuracy/set, else warn user of accuracy decrease
        if(bestAccuracyInNext > bestAccuracy) {
            bestAccuracy = bestAccuracyInNext;
            if(bestSetInNext.empty()) bestSet.clear();
            else bestSet.assign(bestSetInNext.begin(), bestSetInNext.end());
        }
        else cout << endl << "Accuracy has decreased! Continuing search for local maxima.";
        
        // Special case testing for empty set
        if(!bestSetInNext.empty()) {
            // Report best set in tier
            cout << endl << "Feature set ";
            reportFeatures(bestSetInNext);
            cout << " was best, accuracy is " << bestAccuracyInNext*100.0 << "%" << endl << endl;
        }
        else cout << endl << "Empty feature set was best, accuracy is " << bestAccuracyInNext*100.0 << "%" << endl << endl;
        
        bestSetInTier = bestSetInNext;
    }
    
    // Return result
    return pair<vector<int>, double>(bestSet, bestAccuracy);
}

void reportFeatures(vector<int> features) {
    sort(features.begin(), features.end());
    cout << "{ ";
    for(int i = 0; i < features.size()-1; i++) cout << features[i] << ", ";
    cout << features[features.size()-1] << " }";
    return;
}

vector<vector<double>> zNormalize(vector<vector<double>> data) {
    vector<vector<double>> dataCopy;
    dataCopy.assign(data.begin(), data.end());
    
    for(int j = 1; j < dataCopy[0].size(); j++) {
        vector<double> featureData;
        for(int i = 0; i < data.size(); i++) {
            featureData.push_back(dataCopy[i][j]);
        }
        double featureMean = mean(featureData);
        double featureStd = standardDev(featureData, featureMean);
        
        for(int i = 0; i < dataCopy.size(); i++) {
            dataCopy[i][j] = (dataCopy[i][j] - featureMean)/featureStd;
        }
    }
    
    return dataCopy;
}

double mean(vector<double> data) {
    double temp = 0;
    for(int i = 0; i < data.size(); i++) temp += data[i];
    return temp/static_cast<double>(data.size());
}

double standardDev(vector<double> data, double mean) {
    double temp = 0;
    
    for(int i = 0; i < data.size(); i++) temp += pow(data[i]-mean, 2);
    
    return sqrt(temp/static_cast<double>(data.size()));
}