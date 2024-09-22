#include <stdio.h>

int main()
{
    int DerivativeOrder, degree;
    float coefficient, x;
    scanf(" %f %d %d",&x, &DerivativeOrder, &degree);

    float result=0.;

    while(degree >= 0)
    {
        scanf(" %f", &coefficient);
        int exp = degree;
        for(int i=0; i < DerivativeOrder;i++)
        {
            if(coefficient == 0)
            {
                break;
            }
            coefficient *= exp;
            exp--;
        }
        for(int i=0; i<exp;i++)
            {
                coefficient *= x;
            }
            result += coefficient;

        degree--;
    }
    printf("%.3f", result);
    return 0;
}
