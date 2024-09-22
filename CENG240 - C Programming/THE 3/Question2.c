#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int readline(char* buffer, unsigned bufferSize)
{
    unsigned nextIndex = 0;
    while (1)
    {
        if (nextIndex == bufferSize)
        {
            break;
        }
        char c;
        scanf("%c", &c);
        if (c == '\n')
        {
            buffer[nextIndex] = '\0';
            return strlen(buffer);
        }
        else
        {
            buffer[nextIndex] = c;
            nextIndex++;
        }
    }
}

int readText(char* buffer)
{
    unsigned nextIndex = 0;
    while (1)
    {
        char c;
        int ifeof = scanf("%c", &c);
        if(ifeof == EOF)
        {
           buffer[nextIndex] = '\0';
           break;
        }
        if(c == '\n')
        {
            continue;
        }
        buffer[nextIndex] = c;
        nextIndex++;
    }
    return strlen(buffer);
}

unsigned* sort(unsigned* sizes, unsigned count)
{
    unsigned *indexArray;

    indexArray = (unsigned*) malloc(count * sizeof(unsigned));

    unsigned sortedArray[count];
    for(int i = 0; i < count; i++)
    {
        sortedArray[i] = 0;
    }

    for(int i = 0; i < count; i++)
    {
        unsigned current = sizes[i];
        for(int j = 0; j < count; j++)
        {
            if(sortedArray[j] == 0)
            {
                sortedArray[j] = current;
                indexArray[j] = i;
                break;
            }
            else if(sortedArray[j] == current)
            {
                continue;
            }
            else if(sortedArray[j] > current)
            {
                memmove(&sortedArray[j+1], &sortedArray[j], sizeof(unsigned)* (i-j));
                sortedArray[j] = current;
                memmove(&indexArray[j+1], &indexArray[j], sizeof(unsigned)* (i-j));
                indexArray[j] = i;
                break;
            }
        }
    }
    return indexArray;
}

void printBetween(char* start, char* end)
{
    int len = end - start;
    for(int i = 0; i < len; i++)
    {
        printf("%c", start[i]);
    }
}

int main()
{

    char startDelimiter[12], endDelimiter[12];
    int startSize = readline(startDelimiter, 12);
    int endSize = readline(endDelimiter, 12);
    char* text;
    text = (char*) malloc(500*sizeof(char));
    int textSize = readText(text);
    text = (char*) realloc(text, (textSize + 1) * sizeof(char));
    char* current;
    current = text;
    char* tokens[100];
    unsigned tokenSizes[100];
    unsigned i = 0;
    while(current != NULL)
    {
        char* start = strstr(current, startDelimiter);
        if (start == NULL)
        {
            break;
        }
        char* end = strstr(current, endDelimiter);
        start = start + startSize;
        unsigned length = end - start;
        tokens[i] = (char *) malloc((length + 1) * sizeof(char));
        memcpy(tokens[i], start, length);
        tokens[i][length] = '\0';
        tokenSizes[i] = length;
        i++;
        current = end + endSize;
    }
    unsigned* indexArr = sort(tokenSizes, i);
    for(int j = 0; j < i; j++)
    {
        printf("%s\n", tokens[indexArr[j]]);
    }

    current = text;
    char* old = current;
    while(strstr(current, startDelimiter) != NULL)
    {
        current = strstr(current, startDelimiter);
        printBetween(old, current);
        old = current + startSize;
        current = strstr(current, endDelimiter);
        printBetween(old, current);
        old = current + endSize;
    }
    current += endSize;
    printf("%s\n", current);
    return 0;
}
