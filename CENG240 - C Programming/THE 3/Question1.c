#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* readline()
{
    char* line;
    char* start;
    line = (char*) malloc(100 * sizeof(char));
    start = line;
    scanf("%c", line);
    line++;
    while (1)
    {
        scanf("%c", line);
        if(*line == '\n')
        {
            break;
        }
        line++;
    }
    *line = '\0';
    return start;
}

int setfunction(char* line, char* variablePlace, int* valuePlace, int ifmalloc, char**variableList, int* valueList, unsigned variableCount)
{
    line = line + 4; // Jump the method
    char varname[100];
    sscanf(line, " %s", varname);
    int l = strlen(varname);
    line = line + l; // Jump the variable name
    int value;
    sscanf(line, " %d", &value);
    if (value > 99)
    {
        value = 99;
    }
    else if (value < 0)
    {
        value = 0;
    }
    for(int i = 0; i < variableCount; i++)
    {
        if(strcmp(varname, variableList[i]) == 0)
        {
            valueList[i] = value;
            return -1;
        }
    }
    if(ifmalloc == 1)
    {
        strcpy(variablePlace, varname);
        *valuePlace = value;
    }
    return l;
}

void addfunction(char* line, char**variableList, int* valueList, unsigned* variableCount)
{
    line = line + 4; // Jump the method
    char variable1[100];
    char variable2[100];
    sscanf(line, " %s", variable1);
    int l1 = strlen(variable1);
    line = line + l1 + 1;
    strcpy(variable2, line);
    int l2 = strlen(variable2); // I know the names and name's lengths
    int value1 = 0, value2 = 0;
    int m = 0; // To prevent unnecessary continuation and to know if variables are declared before.
    int index1, index2;
    for(int i = 0; i < *variableCount; i++)
    {
        if(strcmp(variable1, variableList[i]) == 0)
        {
            value1 = valueList[i];
            index1 = i;
            m += 10;
        }
        if(strcmp(variable2, variableList[i]) == 0)
        {
            value2 = valueList[i];
            index2 = i;
            m += 2;
        }
        if (m == 12)
        {
            break;
        }
    }

    if(m == 0) // Variables are not declared before
    {
        variableList[*variableCount] = (char*) malloc((l1+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable1);
        valueList[*variableCount] = value1;
        (*variableCount)++;
        variableList[*variableCount] = (char*) malloc((l2+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable2);
        valueList[*variableCount] = value2;
        (*variableCount)++;
    }

    else if(m == 10) // Only the first one is declared before;
    {
        valueList[index1] = value1 + 0;
        variableList[*variableCount] = (char*) malloc((l2+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable2);
        valueList[*variableCount] = value2;
        (*variableCount)++;
    }

    else if(m == 2) // Only the second one is declared before;
    {
        variableList[*variableCount] = (char*) malloc((l1+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable1);
        valueList[*variableCount] = 0 + value2;
        (*variableCount)++;
    }

    else if(m == 12) // Both of them declared before;
    {
           valueList[index1] = (value1 + value2 > 99) ? 99 : value1 + value2;
    }
}

void subfunction(char* line, char**variableList, int* valueList, unsigned* variableCount)
{
    line = line + 4; // Jump the method
    char variable1[100];
    char variable2[100];
    sscanf(line, " %s", variable1);
    int l1 = strlen(variable1);
    line = line + l1 + 1;
    strcpy(variable2, line);
    int l2 = strlen(variable2); // I know the names and name's lengths
    int value1 = 0, value2 = 0;
    int m = 0; // To prevent unnecessary continuation and to know if variables are declared before.
    int index1, index2;
    for(int i = 0; i < *variableCount; i++)
    {
        if(strcmp(variable1, variableList[i]) == 0)
        {
            value1 = valueList[i];
            index1 = i;
            m += 10;
        }
        if(strcmp(variable2, variableList[i]) == 0)
        {
            value2 = valueList[i];
            index2 = i;
            m += 2;
        }
        if (m == 12)
        {
            break;
        }
    }

    if(m == 0) // Variables are not declared before
    {
        variableList[*variableCount] = (char*) malloc((l1+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable1);
        valueList[*variableCount] = value1;
        (*variableCount)++;
        variableList[*variableCount] = (char*) malloc((l2+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable2);
        valueList[*variableCount] = value2;
        (*variableCount)++;
    }

    else if(m == 10) // Only the first one is declared before;
    {
        valueList[index1] = value1 + 0;
        variableList[*variableCount] = (char*) malloc((l2+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable2);
        valueList[*variableCount] = value2;
        (*variableCount)++;
    }

    else if(m == 2) // Only the second one is declared before;
    {
        variableList[*variableCount] = (char*) malloc((l1+1)* sizeof(char));
        strcpy(variableList[*variableCount], variable1);
        valueList[*variableCount] = 0; // Because it underflows or it is zero
        (*variableCount)++;
    }

    else if(m == 12) // Both of them declared before;
    {
           valueList[index1] = (value1 - value2 < 0) ? 0 : value1 - value2;
    }
}

void showfunction(char* line, char** variableList, int* valueList, unsigned variableCount)
{
    line = line + 5;
    char varname[100];
    sscanf(line, " %s", varname);
    for(int i = 0; i < variableCount; i++)
    {
        if(strcmp(varname, variableList[i]) == 0)
        {
            printf("%s = %d\n", varname, valueList[i]);
            return;
        }
    }
    printf("%s = %d\n", varname, 0);
}

int gobackfunction(char* line, int* isSmall, char** variableList, int* valueList, unsigned variableCount)
{
    int lineNumber = 0;
    line = line + 7;
    sscanf(line, " %d", &lineNumber);
    line = line + 2;
    char variable1[100], variable2[100];
    int value1 = 0, value2 = 0;
    sscanf(line, " %s", variable1);
    int l = strlen(variable1);
    line = line + l + 1;
    strcpy(variable2, line);
    int m = 0;
    for(int i = 0; i < variableCount; i++)
    {
        if(strcmp(variable1, variableList[i]) == 0)
        {
            value1 = valueList[i];
            m++;
        }
        if(strcmp(variable2, variableList[i]) == 0)
        {
            value2 = valueList[i];
            m++;
        }
        if (m == 2)
            break;
    }

    if (value1 < value2)
    {
        *isSmall = 1;
    }

    return lineNumber;
}

int main()
{
    char *Commandlist[100];
    char* variables[200];
    int values[200];
    unsigned variableindex = 0;
    for(int i = 0; 1; i++)
    {
        Commandlist[i] = (char*) malloc(100 * sizeof(char));
        Commandlist[i] = readline();
        if (!strcmp(Commandlist[i], "exit"))
        {
            break;
        }
    }
    for(int i = 0; 1; i++)
    {
        if (Commandlist[i][0] == 's' && Commandlist[i][1] == 'e' && Commandlist[i][2] == 't')   //Set Function
        {
            int length = setfunction(Commandlist[i], variables[variableindex], values + variableindex, 0, variables, values, variableindex);
            if (length == -1)
            {
                continue;
            }
            variables[variableindex] = (char*) malloc((length+1) * sizeof(char*));
            setfunction(Commandlist[i], variables[variableindex], values + variableindex, 1, variables, values, variableindex);
            variableindex++;
        }
        else if(Commandlist[i][0] == 'a' && Commandlist[i][1] == 'd' && Commandlist[i][2] == 'd')
        {
            addfunction(Commandlist[i], variables, values, &variableindex);
        }
        else if(Commandlist[i][0] == 's' && Commandlist[i][1] == 'u' && Commandlist[i][2] == 'b')
        {
            subfunction(Commandlist[i], variables, values, &variableindex);
        }
        else if(Commandlist[i][0] == 's' && Commandlist[i][1] == 'h' && Commandlist[i][2] == 'o' && Commandlist[i][3] == 'w')
        {
            showfunction(Commandlist[i], variables, values, variableindex);
        }
        else if(Commandlist[i][0] == 'g' && Commandlist[i][1] == 'o' && Commandlist[i][2] == 'b' && Commandlist[i][3] == 'a')
        {
            int isSmall = 0;
            int lineNumber = gobackfunction(Commandlist[i], &isSmall, variables, values, variableindex);
            if (isSmall)
                i = i - lineNumber - 1; // Because for loop increase the i while it is starting.
        }
        else if (!strcmp(Commandlist[i], "exit"))
        {
            break;
        }
    }
    return 0;
}
