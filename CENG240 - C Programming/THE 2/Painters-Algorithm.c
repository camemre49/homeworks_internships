#include <stdio.h>

int deptharray[50][50]; // To initialize with 0's. I will use in main function.

void printer(int rows, int columns, char array[rows][columns*2])
{
    columns = columns * 2;
    for(int i = rows - 1; i >= 0; i--) // to write upside down
    {
        for(int j = 0; j < columns; j++)
        {
            printf("%c", array[i][j]);
        }
        printf("\n");
    }
}

void statisticprinter(int row, int statarr[][4])
{
    for(int i = 0; i < row; i++)
    {
        printf("%d %d %d\n", statarr[i][0], statarr[i][1], statarr[i][2]);
    }
}

void circlemaker(int stats, int r, int x, int y, int depth, char letter, int rows, int columns, char array[rows][columns*2], int deptharr[rows][columns], int* onCanvas, int* totalpoint)
{
    x--;  // arrangement for 0 indexing
    y--;
    int bottom = y - r, up = y + r, leftmost = x - r, rightmost = x + r;
    int index = 2*leftmost;
    for(int i = bottom; i <= up; i++)
    {
        for(int j = leftmost; j <= rightmost; j++, index += 2)
        {
            if((x-j)*(x-j) + (y-i)*(y-i) <= r*r)
            {
                if(stats == 1)
                {
                    (*totalpoint)++;
                }
                if(i >= 0 && i < rows && j >= 0 && j < columns)
                {
                    if(stats == 1)
                        {
                            (*onCanvas)++;
                        }
                    if(deptharr[i][j] == 0 || deptharr[i][j] > depth)
                    {
                        array[i][index] = array[i][index + 1] = letter;
                        deptharr[i][j] = depth;
                    }
                }
            }
        }
        index = 2*leftmost;
    }
}

int LetterCounter(int r, int c, char letter, char canvasarr[r][c])
{
    int letterCount = 0;
    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            if(canvasarr[i][j] == letter)
            {
               letterCount++;
            }
        }
    }
    return letterCount/2;
}


int main()
{
    int columns, rows, circles, stats;
    scanf(" %d %d %d %d", &columns, &rows, &circles, &stats);
    // Canvas base
    int canvascolumns = columns*2;
    char canvas[rows][canvascolumns];
    for (int i = 0; i < rows; i++)
    {
        for(int j = 0; j < canvascolumns; j++)
        {
            canvas[i][j] = '-';
        }
    }

    int statistics[circles][4];

    char letter;
    char letterArray[circles];
    int xpoint, ypoint, radius, depth, PointsonCanvas = 0, totalpoints = 0;
    for(int i = 0; i < circles; i++)
    {
        scanf(" %c %d %d %d %d",&letter, &xpoint, &ypoint, &radius, &depth);
        letterArray[i] = letter;
        circlemaker(stats, radius, xpoint, ypoint, depth, letter, rows, columns, canvas, deptharray, &PointsonCanvas, &totalpoints);
        statistics[i][3] = PointsonCanvas;
        statistics[i][2] = totalpoints - PointsonCanvas;
        PointsonCanvas = 0;
        totalpoints = 0;
    }

    int shownPoints;
    for(int i = 0; i < circles; i++)
    {
        shownPoints = LetterCounter(rows, canvascolumns, letterArray[i], canvas);
        statistics[i][0] = shownPoints;
        statistics[i][1] = statistics[i][3] - shownPoints;
    }
    printer(rows, columns, canvas);
    if(stats == 1)
    {
        statisticprinter(circles, statistics);
    }
    return 0;
}
