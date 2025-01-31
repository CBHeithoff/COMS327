#include <stdio.h> 
#include <time.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{ 
    srand(time(NULL));

    printf("%d\n", rand());
    printf("%d\n", rand());
    printf("%d\n", rand());
    printf("%d\n", rand());
    
    
    printf("%c\n", 'a' + (rand() % 26));
    printf("%c\n", 'a' + (rand() % 26));
    printf("%c\n", 'a' + (rand() % 26));
    printf("%c\n", 'a' + (rand() % 26));

    return 0;
}