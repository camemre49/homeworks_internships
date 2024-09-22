#include <stdio.h>

float arithmetic(int n, int sumArray[n][n])
{
    float arithmetic = 0;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            arithmetic += sumArray[i][j];
        }
    }
    return (arithmetic / (n*n));
}



void Array_Maker(int r, int c, int s, float arr[2][4], float value, int *order)
{
    arr[*order][0] = r;
    arr[*order][1] = c;
    arr[*order][2] = s;
    arr[*order][3] = value;
    *(order) = 1:
}




float Sum_Array(int row, int column, int size, int arrsize, int arr[arrsize][arrsize]) // Takes a (size)X(size) array from the whole array. And finds its value.
{
    int tempArray[size][size];
    int i = row;
    int j = column;
    if (row + size <= arrsize && column + size <= arrsize)
    {
        for(int count = 0; count < size; count++) // Exludes the array from the original array
        {
            for(int count2 = 0; count2 < size; count2++)
            {
                tempArray[count][count2] = arr[i+count][j+count2];
            }
        }
        float arithm = arithmetic(size, tempArray);
        return arithm;
    }
    return -1;
}





int main()
{
    int minimumSize, arraySize;
    scanf("%d %d",&arraySize, &minimumSize);

    if (arraySize < minimumSize)
    {
        return 0;
    }

    int numbers[arraySize][arraySize];
    for(int i = 0; i < arraySize; i++)
    {
        for(int j = 0; j < arraySize; j++)
        {
            scanf("%d", &numbers[i][j]);
        }
    }

    if (arraySize == minimumSize)
    {
        printf("%d %d %d\n",1, 1, arraySize);
        return 0;
    }

    float whole = arithmetic(arraySize, numbers);
    float arithmeticArray[2][4]; // Holds [0]: x, [1]: y, [2]: size, [3]: value of the largest matrix.
    int order = 0;
    Array_Maker(0, 0, arraySize, arithmeticArray, whole, &order);
    for(int i = minimumSize; i < arraySize; i++)
    {
        for(int r = 0; r + i <= arraySize; r++)
        {
            for(int c = 0; c+i <= arraySize; c++)  // Taking the possible indexes for a minimumSize array.
            {
                float value = Sum_Array(r, c, i, arraySize, numbers);
                if(value == arithmeticArray[0][3])
                {
                    order = 1;
                    Array_Maker(r, c, i, arithmeticArray, value, &order); // Current value in arithmeticarr[1]
                    if (arithmeticArray[1][2] > arithmeticArray[0][2])  // Implements the tie-braker rules.
                    {
                        order = 0; // If current value should be the answer, it goes to the first index of the arithmetic array. At the last part of the function order = 1 again.
                        Array_Maker(r, c, i, arithmeticArray, value, &order);

                    }
                    else if (arithmeticArray[1][2] == arithmeticArray[0][2])
                    {
                        if (arithmeticArray[1][1] < arithmeticArray[0][1])
                        {
                            order = 0;
                            Array_Maker(r, c, i, arithmeticArray, value, &order);
                        }
                    }

                }
                else if(value > arithmeticArray[0][3])
                {
                    order = 0;
                    Array_Maker(r, c, i, arithmeticArray, value, &order);
                }
            }
        }
    }
    int r,c,s;
    r = arithmeticArray[0][0];
    c = arithmeticArray[0][1];
    s = arithmeticArray[0][2];
    printf("%d %d %d", (int) r+1, (int) c+1, (int) s);
    return 0;
}

