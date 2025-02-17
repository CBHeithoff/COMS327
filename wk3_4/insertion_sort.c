#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void insertion_sort_int(int *a, int n)
{
    int i, j, t;

    for(i = 1; i < n; i++){
        for(t = a[i], j = i - 1; j > -1 && a[j] > t; j--){
            a[j + 1] = a[j];
        }
        a[j + 1] = t;
    }
}

/*
Reading declarations in C: When reading a declaration in C, 
we begin with the name and go right when we can and left 
when we must.

compare_int is a pointer to a function maybe
int (*compare_int)(void *v1, void *v2)
*/

// insertion sort parameter a[j] > t wont work
// need a comparator and pass it in
// but cannot pass in ints, compile doesn't know there ints, pass void pointers instead
//int compare_int(int i, int j)
int compare_int(void *v1, void *v2)
{
    return *((int *) v1) - *((int *) v2);
}

int compare_int_reverse(void *v1, void *v2)
{
    return *((int *) v2) - *((int *) v1);
}



// void pointer to point to anything
// but how will it index into a without knowing the size
// so pass in the size
void insertion_sort_generic(void *a, int n, int size,
                            int (*compare)(void *, void *))
{
    int i, j;
    void *t;
    // a + (i * size) won't work for address math
    // c with index one byte at a time though, we use that
    char *c; // Use a char array because it is byte addressable

    c = a;

    t = malloc(size);

    for(i = 1; i < n; i++){
        //for(t = a[i], j = i - 1; j > -1 && a[j] > t; j--){
        //for(t = c + (i * size), j = i - 1; j > -1 && a[j] > t; j--){
        for (memcpy(t, c + (i * size), size), j = i - 1; 
            j > -1 && compare(c + (j * size), t) > 0; j--){
            //a[j + 1] = a[j];
            memcpy(c + ((j + 1) * size), c + (j * size), size);
        }
        //a[j + 1] = t;
        memcpy(c + ((j + 1) * size), t, size);
    }

    free(t);
}

int a[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };



int compare_chars(void *v1, void *v2)
{
    return *((char *) v1) - *((char *) v2);
}



char *words[] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
    "ten",
};


int compare_strings(void *v1, void *v2)
{
    // need to dereference
    //return strcmp(v1, v2);
    return strcmp(*(char **) v1, *(char **) v2);
}


int main(int argc, char *argv[])
{
    int i;
    char s[] = "the quick brown fox jumps over the lazy dog.";
    
    //insertion_sort_generic(a, 11, sizeof (*a), compare_int);
    insertion_sort_generic(a, sizeof (a) / sizeof (a[0]), 
                            sizeof (*a), compare_int);

    //for (i = 0; i < 11; i++){
    for (i = 0; i < sizeof (a) / sizeof (*a); i++){
        printf("%d ", a[i]);
    }
    printf("\n");


    insertion_sort_generic(a, sizeof (a) / sizeof (a[0]), 
                            sizeof (*a), compare_int_reverse);

    //for (i = 0; i < 11; i++){
    for (i = 0; i < sizeof (a) / sizeof (*a); i++){
        printf("%d ", a[i]);
    }
    printf("\n");



    printf("%s\n", s);
    insertion_sort_generic(s, strlen(s), 1, compare_chars);
    printf("%s\n", s);


    // size of words / one element of words
    // strcmp passes const but expecting void pointers, use case
    //insertion_sort_generic(words, sizeof(words) / sizeof (*words), sizeof (*words), strcmp);
    //insertion_sort_generic(words, sizeof(words) / sizeof (*words), 
    //                        sizeof (*words), 
    //                        (int (*)(void *, void *))strcmp);
    // strcmp not going to work
    insertion_sort_generic(words, sizeof(words) / sizeof (*words), 
                            sizeof (*words), 
                            compare_strings);

    for(i = 0; i < sizeof (words) / sizeof (*words); i++){
        printf("%s\n", words[i]);
    }


    return 0;
}