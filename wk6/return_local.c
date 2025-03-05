#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // make table of pointers that is size of dungeon to hold monster structs.
    // malloc unlimited amount of monsters.

    // ANSI Escape Codes
    // for color and other cursor control
    // not very portable



    // outputs nine 9s
    int i;//, j;
    // outputs 0-9
    int *j;
    int *a[10];

    for(i = 0; i < 10; i++){
        // outputs 0-9
        j = malloc(sizeof(*j));
        *j = i;
        a[i] = j;
        //outputs nine 9s
        // j = i;
        // a[i] = &j;
    }

    for(i = 0; i < 10; i++){
        printf("%d\n", *a[i]);
    }

    for(i = 0; i < 10; i++){
        free(a[i]);
    }

    


    return 0;
}