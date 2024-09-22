#include <stdio.h>

int main()
{
    char current, old;
    int count=1;
    scanf(" %c", &current);
    old = current;
    while(1)
    {
        scanf("%c", &current);
        if(current== '\n')
        {
            break;
        }

        if(current == old)
        {
            count++;
        }
        else
        {
            printf("%d%c",count,old);
            count = 1;
            old = current;
        }
    }
    printf("%d%c",count, old);
    return 0;
}
