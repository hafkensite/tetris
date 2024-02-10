#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

#define byte char
#define MAX_Y 32
#define MAX_X 8

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

const int MAX_SHAPE = 5;
const int shapes[][8] = {
    {0, 0, 1, 0, 1, 1, 0, 1}, // Square
    {0, 0, 0, 1, 0, 2, 1, 1}, // T
    {0, 0, 0, 1, 0, 2, 0, 3}, // Line
    {0, 0, 0, 1, 0, 2, 1, 2}, // L
    {0, 0, 0, 1, 1, 1, 1, 2}  // Z
};

byte buffer[MAX_Y];

void printBuffer()
{
    clear();
    for (int y = 0; y < MAX_Y; y++)
    {
        for (int x = 0; x < MAX_X; x++)
        {
            char c = bitRead(buffer[y], x) ? '#' : '.';
            // printf("%c", c);
            addch(c);
        }
        addch('\n');
        // printf("\n");
    }
}

void toggleShape(int shape, int x, int y)
{
    for (int i = 0; i < 4; i++)
    {
        int dx = shapes[shape][i * 2] + x;
        int dy = shapes[shape][i * 2 + 1] + y;
        bitWrite(buffer[dy], dx, !bitRead(buffer[dy], dx));
    }
}

int fitShape(int shape, int x, int y)
{
    for (int i = 0; i < 4; i++)
    {
        int dx = shapes[shape][i * 2] + x;
        int dy = shapes[shape][i * 2 + 1] + y;
        if (dx < 0 || dx >= MAX_X)
        {
            return 0;
        }
        if (dy < 0 || dy >= MAX_Y)
        {
            return 0;
        }
        if (bitRead(buffer[dy], dx))
        {
            return 0;
        }
    }
    return 1;
}

int main(int argv, char *args[])
{
    time_t t;
    srand((unsigned)time(&t));

    initscr();
    clear();
    noecho();
    cbreak();
    timeout(200);

newShape:

    // TODO remove full lines

    int y = -1;
    int x = 4;
    int shapeNum = rand() % MAX_SHAPE;

    for (;;)
    {
        int c = getch();

        switch (c)
        {
        case 'a':
            if (fitShape(shapeNum, x - 1, y))
            {
                x--;
            }
            break;
        case 'd':
            if (fitShape(shapeNum, x + 1, y))
            {
                x++;
            }
            break;
        case 's':
            // TODO rotate
        }

        if (fitShape(shapeNum, x, y + 1))
        {
            y++;
            toggleShape(shapeNum, x, y);
        }
        else
        {
            if (y <= 0)
            {
                return -1;
            }
            // Bonk
            toggleShape(shapeNum, x, y);
            goto newShape;
        }
        printBuffer();
        // usleep(50000);
        toggleShape(shapeNum, x, y);
    }
    return 0;
}
