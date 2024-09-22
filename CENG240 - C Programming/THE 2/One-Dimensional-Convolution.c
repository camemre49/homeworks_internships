#include <stdio.h>

int main()
{
    int first, second;
    scanf(" %d %d", &first, &second);
    int firstarr[first], secondarr[second];
    for(int i = 0; i < first; i++)
    {
        scanf(" %d", &firstarr[i]);
    }
    for(int j = 0; j < second; j++)
    {
        scanf(" %d", &secondarr[j]);
    }
    int out[first];
    int i = 0, counts = 0;
    while(i <= first - second)
    {
        int sum = 0;
        for(int j = 0; j < second; j++)
        {
            sum += firstarr[i + j] * secondarr[j];
        }
        out[i] = sum;
        counts++;
        i = i + 1;
    }

    for(int i = 0; i < counts; i++)
    {
        printf("%d ",out[i]);
    }
    return 0;
}
