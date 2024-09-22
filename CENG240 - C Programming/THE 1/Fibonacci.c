#include <stdio.h>

int main()
{
    int First, Second, coef1, coef2, number;
    char operation;
    int opcode =0;
    scanf(" %d %d %d %d %c %d",&First, &Second, &coef1, &coef2, &operation, &number);
    if (operation == '+')
    {
        opcode = 1;
    }
    int result=0, order=2;

    if(number == 1)
    {
        result = First;
    }
    else if(number == 2)
    {
        result = Second;

    }
    else
    {
        if(opcode)
        {
            while(order < number)
            {
                result = coef1*First + coef2*Second;
                First = Second;
                Second = result;
                order++;
            }
        }
        else
        {
            while(order < number)
            {
                result = coef1*First - coef2*Second;
                First = Second;
                Second = result;
                order++;
            }
        }
    }
    printf("%d",result);

    return 0;
}
