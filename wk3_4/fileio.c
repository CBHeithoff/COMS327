#include <stdio.h>
#include <math.h> // For M_PI. If calling functions, must link with math library (-lm).

#include "endian.h"

typedef enum {
    operation_rt, //default 0
    operation_rb, //default 1, can "operation_rt = 10" for different values
    operation_wt, //default 2
    operation_wb  //default 3
} operation;

void usage(const char *s)
{
    // The runtime opens three files for us before main begins.
    // These are stdin, stdout, and stderr.
    fprintf(stderr, "%s [-rt|-rb|-wt|-wb]\n", s);
}

int main(int argc, char *argv[])
{
    operation op;
    int error;

    // no name
    struct {
        int i;
        int j;
        double d;
    } in, out; // only two instantances in and out
    FILE *f;
    int i;
    long long int d;

    // (void) in; // Suppress unused error from compile

    out.i = 1;
    out.j = 65537;
    out.d = M_PI;

    error = 1;
    // example command: ./fileio -wb
    // finding mode "-wb" on cammand
    if (argc == 2 && argv[1][0] == '-'){
        // reading
        if(argv[1][1] == 'r'){
            // reading text
            if (argv[1][2] == 't'){
                op = operation_rt;
                error = 0;
            }
            // reading binary
            else if(argv[1][2] == 'b'){
                op = operation_rb;
                error = 0;
            }
        }
        // writing
        else if(argv[1][1] == 'w'){
            // writing text
            if (argv[1][2] == 't'){
                op = operation_wt;
                error = 0;
            }
            // writing binary
            else if(argv[1][2] == 'b'){
                op = operation_wb;
                error = 0;
            }
        }
    }

    if(error){
        usage(argv[0]);
        return -1;
    }

    switch (op){
        case operation_rt:
            f = fopen("textfile", "r");
            fscanf(f, "%d\n%d\n%lf\n", &in.i, &in.j, &in.d);
            printf("%d\n%d\n%lf\n", in.i, in.j, in.d);
            break;
        case operation_wt:
            f = fopen("textfile", "w"); // can fail. should check
            fprintf(f, "%d\n%d\n%lf\n", out.i, out.j, out.d);
            break;
        case operation_rb:
            f = fopen("binaryfile", "rb");
            //fread(&in, sizeof (in), 1, f);

            // big endian conversion
            fread(&i, sizeof (i), 1, f);
            in.i = be32toh(i);
            fread(&i, sizeof (i), 1, f);
            in.j = be32toh(i);
            fread(&d, sizeof (d), 1, f);
            d = be64toh(d);
            in.d = (*(double *) &d);

            printf("%d\n%d\n%lf\n", in.i, in.j, in.d);
            break;
        case operation_wb:
            f = fopen("binaryfile", "wb"); // b tells Windwos to suppress \r
                                            // ignore in all other environments
            // little endian
            // fwrite(&out, sizeof (out), 1, f); // should return 1. Should check

            // converting to big endian
            i = htobe32(out.i);
            fwrite(&i, sizeof (i), 1, f);
            i = htobe32(out.j);
            fwrite(&i, sizeof (i), 1, f);
            d = htobe64(*(long long int *) &out.d);
            fwrite(&d, sizeof (d), 1, f);
            break;
    }

    fclose(f);

    return 0;
}

// htobe<number>()
// htobe8
// htobe16
// htobe32
// htobe64

// be<number>toh
// be8toh
// be16toh
// be32toh
// be64toh
