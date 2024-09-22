#include "the2.h"

// do not add extra libraries here
/*
    arr       : array to be sorted, in order to get points this array should contain be in sorted state before returning
    ascending : true for ascending, false for descending 
    size      : number of elements in the array
*/

void reverseArray(std::string* arr, int size) {
    for (int i = 0, j = size - 1; i < j; i++, j--) {
        std::swap(arr[i], arr[j]);
    }
}


int radix_string_sort(std::string* arr, int size, bool ascending){
    
    int maxLength = 0;
    for (int i = 0; i < size; i++) {
        int len = arr[i].length();
        if (len > maxLength) {
            maxLength = len;
        }
    }
    
    std::string helper[size];
    std::string bucket[size];
    int bucketIndex;
    int iterationCount = 0;

    for (int i = maxLength - 1; i >= 0; i--){
        iterationCount++;
        
        int * counts = new int[26];
        for (int i = 0; i < 26; ++i) {
            counts[i] = 0;
        }

        bucketIndex = 0;
        for(int j = 0; j < size; j++){
            iterationCount++;
            std::string word = arr[j];
            int len = word.length();
            if (i >= len){
                bucket[bucketIndex] = word;
                bucketIndex++;
                continue;
            }
            counts[(int)(word[i]) - 65] += 1;
        }

        for(int k = 0; k < 25; k++){
            iterationCount++;
            counts[k+1] = counts[k + 1] + counts[k];
        }

        for(int j = size - 1; j >= 0; j--){
            iterationCount++;
            std::string word = arr[j];
            int len = word.length();
            if (i >= len){
                continue;
            }
            int index = (int)(word[i]) - 65;
            helper[counts[index] - 1 + bucketIndex] = arr[j];
            counts[index]--;
        }

        for(int k = 0; k < bucketIndex; k++){
            iterationCount++;
            arr[k] = bucket[k];
        }

        for(int k = bucketIndex; k < size; k++){
            iterationCount++;
            arr[k] = helper[k];
        }
    }
    
    if(!ascending){
        reverseArray(arr, size);
    }
    
    return iterationCount;
}
