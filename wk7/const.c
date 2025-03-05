#include <stdio.h>

int main(int argc, char *argv[])
{
    // const doesn't allow variable value to be changed.
    // has to be initialized when it is created.
    const int i = 0;

    printf("%d\n", i);

    //i = 1; // error
    //(int) i = 1; // error
    *((int *) &i) = 1; // works to change i even though it is const

    printf("%d\n", i);

    return 0;
}