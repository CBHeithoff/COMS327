#include <stdio.h>
#include <math.h> // For M_PI. If calling functions, must link with math library (-lm).

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

    (void) in; // Suppress unused error from compile

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
            break;
        case operation_wb:
            break;
    }

    fclose(f);

    return 0;
}