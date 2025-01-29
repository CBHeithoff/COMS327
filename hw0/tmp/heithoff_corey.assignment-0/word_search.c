#include <stdio.h> 
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 20
char table[MAX][MAX];
int size;



int populate_table(char newWord[])
{
    // initialization
    int i, j, r, c, startDir, startCol, startRow, dir, row, col, result;
    
    startDir = rand() % 4;
    startCol = rand() % size;
    startRow = rand() % size;

    // checking all column locations
    for(c = 0; c < size; c++){
        col = (startCol + c) % size;

        // checking all row locations
        for(r = 0; r < size; r++){
            row = (startRow + r) % size;

            // checking all four direction positions
            for(j = 0; j < 4; j+=1){
                // initialization
                result = 0;
                dir = (startDir + j) % 4;

                // for debugging
                // printf("\n");
                // for (i = 0; i < MAX; i++){
                //     for (k = 0; k < MAX; k++){
                //         if (table[i][k] == '\0'){
                //             printf("%2c ", '-');
                //         } else{
                //             printf("%2c ", table[i][k]);
                //         }
                //     }
                //     printf("\n");
                // }

                // diagonal up
                if(dir == 0){
                    if(size < (col + strlen(newWord)) || (row + 1 < strlen(newWord))){
                        continue;
                    }
                    for (i = 0; i < newWord[i]; i++){
                        if(table[row - i][i + col] == '\0'){
                            result = 1;
                        }else{
                            result = 0;
                            break;
                        }
                    } 
                    // if placement is successful, insert into real table
                    if (result == 1){
                        for (i = 0; i < newWord[i]; i++){
                            table[row - i][i + col] = newWord[i];
                        }
                        return 1;
                    }
                } 
                // rightward
                else if(dir == 1){
                    if(size < (col + strlen(newWord))){
                        continue;
                    }
                    for (i = 0; i < newWord[i]; i++){
                        if(table[row][i + col] == '\0'){
                            result = 1;
                        }else{
                            result = 0;
                            break;
                        }
                    }
                    // if placement is successful, insert into real table
                    if (result == 1){
                        for (i = 0; i < newWord[i]; i++){
                            table[row][i + col] = newWord[i];
                        }
                        return 1;
                    }
                }
                // diagonal down 
                else if(dir == 2){
                    if(size < (col + strlen(newWord)) || (size < (row + strlen(newWord)))){
                        continue;
                    }
                    for (i = 0; i < newWord[i]; i++){
                        if(table[i + row][i + col] == '\0'){
                            result = 1;
                        }else{
                            result = 0;
                            break;
                        }
                    }  
                    // if placement is successful, insert into real table
                    if (result == 1){
                        for (i = 0; i < newWord[i]; i++){
                            table[i + row][i + col] = newWord[i];
                        }
                        return 1;
                    }     
                } 
                // downward 
                else if(dir == 3){
                    if(size < (row + strlen(newWord))){
                        continue;
                    }
                    for (i = 0; i < newWord[i]; i++){
                        if(table[i + row][col] == '\0'){
                            result = 1;
                        }else{
                            result = 0;
                            break;
                        }
                    }
                    // if placement is successful, insert into real table
                    if (result == 1){
                        for (i = 0; i < newWord[i]; i++){
                            table[i + row][col] = newWord[i];
                        }
                        return 1;
                    }     
                }
            }
        }
    }
    // if no position/location is avaliable return '0' meaning error
    return 0;
}


int main (int argc, char *argv[])
{ 
    // initialization
    int i, j, result;

    size = atoi(argv[1]);
    
    srand(time(NULL));  

    // inserting words
    for (i = 2; i < argc; i++){
        result = populate_table(argv[i]);
        if (result == 0){
            printf("\nError: %s cannot be fit\n", argv[i]);
            return 0;
        }
    }

    // filling in remaining spaces and printing puzzle 
    printf("\n");
    for (j = 0; j < size; j++){
        for (i = 0; i < size; i++){
            if (table[j][i] == '\0'){
                table[j][i] = 'a' + (rand() % 26);
            }
            printf("%2c ", table[j][i]);
        }
        printf("\n");
    }

    // printing word list
    printf("\nWord List:\n");
    for (i = 2; i < argc; i++){
        printf("%s\n", argv[i]);
    }
    printf("\n");

    return 0;
}