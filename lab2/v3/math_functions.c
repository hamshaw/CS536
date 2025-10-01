#include "math_functions.h"

//function for finding max of array values
int maximum(int values[], int n) {
    int max = values[0];

    for (int i = 1; i < n; i++) {
        if (values[i] > max && values[i] != 0) {
            max = values[i];
        }
    }
    return max;
}

//function for finding min of array values
int minimum(int values[], int n) {
    int min = values[0];

    for (int i = 1; i < n; i++) {
        if (values[i] < min && values[i] !=0) {
            min = values[i];
        }
    }
    return min;
}

//function for finding average of array values
int mean(int values[], int n){
        int average = 0;
        for (int i=0; i<n; i++){
                average += values[i];
        }
        return average/n;
}

//function for finding standard deviation of array values
int stdv(int values[], int n){
        int average = mean(values, n);
        int s = 0;
        for (int i=0; i<n; i++){
                s += (average - values[i])*(average - values[i]);
        }
        return sqrt(s/n);
}
