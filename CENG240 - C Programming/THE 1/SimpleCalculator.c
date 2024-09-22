#include <stdio.h>

int main()
{
    double FirstNumber, SecondNumber;
    char operation;
    scanf(" %lf",&FirstNumber);
    scanf(" %c",&operation);
    scanf(" %lf", &SecondNumber);

    if(operation=='+')
        printf("%.3lf", FirstNumber + SecondNumber);
    else if(operation == '-')
        printf("%.3lf", FirstNumber - SecondNumber);
    else if(operation == '/')
        printf("%.3lf", FirstNumber / SecondNumber);
    else if(operation == '*')
        printf("%.3lf", FirstNumber * SecondNumber);
    return 0;
}