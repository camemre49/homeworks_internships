void swapShorts(unsigned short& a, unsigned short& b) {
    unsigned short temp = a;
    a = b;
    b = temp;
}

int partitioningLomuto(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size){
    if(median_of_3 && size > 2){
        unsigned short middle = arr[(int)((size - 1) / 2)];
        int medianIndex;
        if ((arr[0] < arr[size - 1] && arr[0] > middle) || (middle > arr[0] && arr[size - 1] < arr[0])) {
            medianIndex = 0;
        } else if ((arr[0] < middle && middle < arr[size - 1]) || (arr[size - 1] < middle && middle < arr[0])) {
            medianIndex = (size - 1) / 2;
        } else {
            medianIndex = size - 1;
        }
        if(size -1 != medianIndex){
            swapShorts(arr[medianIndex], arr[size - 1]);
            swap++;
            double dist = 0;
            dist = (size - 1) - medianIndex;
            avg_dist += dist;
            if(max_dist < dist){
                max_dist = dist;
            }
        }
    }
    int pivot = arr[size - 1];
    int greaterThanPivotIndex = -1;
    for (int j = 0; j < size - 1; j++){
        if(arr[j] >= pivot){
            greaterThanPivotIndex += 1;
            swapShorts(arr[greaterThanPivotIndex], arr[j]);
            (swap)++;
            double dist = 0;
            dist = j - greaterThanPivotIndex;
            avg_dist += dist;
            if(max_dist < dist){
                max_dist = dist;
            }

        }
    }

    swapShorts(arr[greaterThanPivotIndex + 1], arr[size - 1]);
    (swap)++;
    double dist = 0;
    dist = (size - 1) - (greaterThanPivotIndex + 1);
    avg_dist += dist;
    if(max_dist < dist){
        max_dist = dist;
    }


    return greaterThanPivotIndex + 1;
}

int lomuto(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size) {
    int recursiveCalls = 1;

    if (size > 1){
        int pivotIndex = partitioningLomuto(arr, swap, avg_dist, max_dist, hoare, median_of_3, size);
        recursiveCalls += lomuto(arr, swap, avg_dist, max_dist, hoare, median_of_3, pivotIndex);
        recursiveCalls += lomuto(arr + pivotIndex + 1, swap, avg_dist, max_dist, hoare, median_of_3, size - pivotIndex - 1);
    }

    return recursiveCalls;

}


int partitioningHoare(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size){
    if(median_of_3 && size > 2){
        unsigned short middle = arr[(int)((size - 1) / 2)];
        int medianIndex;
        if ((arr[0] < arr[size - 1] && arr[0] > middle) || (middle > arr[0] && arr[size - 1] < arr[0])) {
            medianIndex = 0;
        } else if ((arr[size - 1] > arr[0] && middle > arr[size - 1]) || (middle < arr[size - 1] && arr[size - 1] < arr[0])) {
            medianIndex = size - 1;
        } else {
            medianIndex = (size - 1) / 2;
        }
        if((int)((size - 1) / 2) != medianIndex){
            swapShorts(arr[medianIndex], arr[(int)((size - 1) / 2)]);
            swap++;
            double dist = 0;
            dist = ((size - 1) / 2) - medianIndex;
            if (dist < 0){
                dist = -dist;
            }
            avg_dist += dist;
            if(max_dist < dist){
                max_dist = dist;
            }
        }
    }
    unsigned short pivot = arr[(int)((size - 1) / 2)];
    int leftIndex = -1;
    int rightIndex = size;

    while(true){

        do {
          rightIndex--;

        } while (rightIndex >= 0 && arr[rightIndex] < pivot);


        do {
          leftIndex++;

        } while (leftIndex < size && pivot < arr[leftIndex]);


        if(rightIndex > leftIndex){
            swapShorts(arr[leftIndex], arr[rightIndex]);
            swap++;
            double dist = 0;
            dist = rightIndex - leftIndex;
            avg_dist += dist;
            if(max_dist < dist){
                max_dist = dist;
            }
        }
        else{
           return rightIndex;
        }
    }
}

int hoareSort(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size) {
    int recursiveCalls = 1;

    if (size > 1){
        int pivotIndex = partitioningHoare(arr, swap, avg_dist, max_dist, hoare, median_of_3, size);
        recursiveCalls += hoareSort(arr, swap, avg_dist, max_dist, hoare, median_of_3, pivotIndex + 1);
        recursiveCalls += hoareSort(arr + pivotIndex + 1, swap, avg_dist, max_dist, hoare, median_of_3, size - pivotIndex - 1);
    }

    return recursiveCalls;
}

int quickSort(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size){

    int recursiveCalls = 0;


    if (!hoare) {
        if (size > 1){
            recursiveCalls = lomuto(arr, swap, avg_dist, max_dist, hoare, median_of_3, size);
        }
    }
    else{
        if (size > 1){
            recursiveCalls = hoareSort(arr, swap, avg_dist, max_dist, hoare, median_of_3, size);
        }
    }

    avg_dist = avg_dist / (double) swap;
    return recursiveCalls;
}
