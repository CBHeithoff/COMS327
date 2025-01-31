#include <stdio.h> 
#include <ctype.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{ 
    int i, j;

	if (argc == 1){
        printf("Hello World!\n");
    } else {
        for (i = 1; i < argc; i++){
            if(isdigit(argv[i][0])){
                printf("%d\n", atoi(argv[i]));
            } else{
                printf("Hello ");
                for (j = 0; argv[i][j]; j++){
                    printf("%c", argv[i][j]);
                }
                printf("!\n");
            }
        }
    }
    

	return 0;
} 