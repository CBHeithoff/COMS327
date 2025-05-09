#include <stdio.h>
#include <math.h>

// PPM for development. large size of files, but use to work with 

//gcc images.c -o images -lm
//./images
//xv image.ppm
//xv grey.pgm

#define Y_SIZE 1024
#define X_SIZE 1019

// humans really only see RBG and G twice as well as RB
#define R 0
#define G 1
#define B 2

unsigned char image[Y_SIZE][X_SIZE][3];

unsigned char grey[Y_SIZE][X_SIZE]; // ilumious matrix
unsigned char grey_out[Y_SIZE][X_SIZE];

unsigned char blur[Y_SIZE][X_SIZE];
unsigned char sharp[Y_SIZE][X_SIZE];

short edge_x[Y_SIZE][X_SIZE];
short edge_y[Y_SIZE][X_SIZE];

void write_ppm(char *filename, char image[Y_SIZE][X_SIZE][3])
{
    FILE *f;
    
    f = fopen(filename, "wb");

    fprintf(f, "P6\n%d\n%d\n255\n", X_SIZE, Y_SIZE);

    fwrite(image, sizeof(char), Y_SIZE * X_SIZE * 3, f);

    fclose(f);
}

void write_pgm(char *filename, char image[Y_SIZE][X_SIZE])
{
    FILE *f;
    
    f = fopen(filename, "wb");

    fprintf(f, "P5\n%d\n%d\n255\n", X_SIZE, Y_SIZE);

    fwrite(image, sizeof(char), Y_SIZE * X_SIZE, f);

    fclose(f);
}

void read_ascii_ppm(char *filename, unsigned char image[Y_SIZE][X_SIZE][3])
{
    char line[80];
    FILE *f;
    int x, y;

    f = fopen(filename, "r");

    fgets(line, 80, f);
    fgets(line, 80, f);
    fgets(line, 80, f);
    fgets(line, 80, f);

    for(y = 0; y < Y_SIZE; y++){
        for(x = 0; x < X_SIZE; x++){
            fscanf(f, "%hhu %hhu %hhu", &image[y][x][R], 
                                        &image[y][x][G], 
                                        &image[y][x][B]);
        }
    }
}

int main(int argc, char *argv[])
{
    int x, y;
    // comb-solution matrices

    // specific linear algebra stuff for edge detection
    short sobel_x[3][3] = {{ -1, -2, -1},
                           {  0,  0,  0},
                           {  1,  2,  1}};

    short sobel_y[3][3] = {{ -1,  0,  1},
                           { -2,  0,  2},
                           { -1,  0,  1}};

    // blur photo
    // also brightens the image by factor of 16, so divide by 16 after average to retain energy
    short gaussian[3][3] = {{ 1, 2, 1},
                            { 2, 4, 2},
                            { 1, 2, 1}};

    short sharpen[3][3] = {{  0, -1,  0},
                           { -1,  5, -1},
                           {  0, -1,  0}};

    short tmp;

    // creating multi-color image

    // for(y = 0; y < Y_SIZE; y++){
    //     for(x = 0; x < X_SIZE; x++){
    //         image[y][x][R] = (y / (double) Y_SIZE) * 255;
    //         image[y][x][G] = (1.0 - (y / (double) Y_SIZE)) * 255;
    //         image[y][x][B] = (x / (double) X_SIZE) * 255;
    //     }
    // }

    // write_ppm("image.ppm", image);


    read_ascii_ppm("motorcycle.ppm", image);

    // edge detection

    for(y = 0; y < Y_SIZE; y++){
        for(x = 0; x < X_SIZE; x++){
            grey[y][x] = (0.299 * image[y][x][R] +
                          0.587 * image[y][x][G] +
                          0.114 * image[y][x][B]);
        }
    }

    // skip image border edge
    for(y = 1; y < Y_SIZE - 1; y++){
        for(x = 1; x < X_SIZE - 1; x++){
            grey[y][x] = (0.299 * image[y][x][R] +
                          0.587 * image[y][x][G] +
                          0.114 * image[y][x][B]);
        }
    }

    for(y = 1; y < Y_SIZE - 1; y++){
        for(x = 1; x < X_SIZE - 1; x++){
            edge_x[y][x] = (grey[y - 1][x - 1] * sobel_x[1 - 1][1 - 1] +
                            grey[y - 1][x    ] * sobel_x[1 - 1][1    ] +
                            grey[y - 1][x + 1] * sobel_x[1 - 1][1 + 1] +
                            grey[y    ][x - 1] * sobel_x[1    ][1 - 1] +
                            grey[y    ][x    ] * sobel_x[1    ][1    ] +
                            grey[y    ][x + 1] * sobel_x[1    ][1 + 1] +
                            grey[y + 1][x - 1] * sobel_x[1 + 1][1 - 1] +
                            grey[y + 1][x    ] * sobel_x[1 + 1][1    ] +
                            grey[y + 1][x + 1] * sobel_x[1 + 1][1 + 1]);

            edge_y[y][x] = (grey[y - 1][x - 1] * sobel_y[1 - 1][1 - 1] +
                            grey[y - 1][x    ] * sobel_y[1 - 1][1    ] +
                            grey[y - 1][x + 1] * sobel_y[1 - 1][1 + 1] +
                            grey[y    ][x - 1] * sobel_y[1    ][1 - 1] +
                            grey[y    ][x    ] * sobel_y[1    ][1    ] +
                            grey[y    ][x + 1] * sobel_y[1    ][1 + 1] +
                            grey[y + 1][x - 1] * sobel_y[1 + 1][1 - 1] +
                            grey[y + 1][x    ] * sobel_y[1 + 1][1    ] +
                            grey[y + 1][x + 1] * sobel_y[1 + 1][1 + 1]);

            tmp = sqrt(edge_x[y][x] * edge_x[y][x] + 
                       edge_y[y][x] * edge_y[y][x]);
            if(tmp > 255) tmp = 255;
            grey_out[y][x] = tmp;

            tmp = (grey[y - 1][x - 1] * gaussian[1 - 1][1 - 1] +
                   grey[y - 1][x    ] * gaussian[1 - 1][1    ] +
                   grey[y - 1][x + 1] * gaussian[1 - 1][1 + 1] +
                   grey[y    ][x - 1] * gaussian[1    ][1 - 1] +
                   grey[y    ][x    ] * gaussian[1    ][1    ] +
                   grey[y    ][x + 1] * gaussian[1    ][1 + 1] +
                   grey[y + 1][x - 1] * gaussian[1 + 1][1 - 1] +
                   grey[y + 1][x    ] * gaussian[1 + 1][1    ] +
                   grey[y + 1][x + 1] * gaussian[1 + 1][1 + 1]);
            tmp /= 16; // divide by the sum of the terms in the kernel in
                       // order to maintain energy
            if (tmp > 255) tmp = 255; // Clamp overflow to white
            blur[y][x] = tmp;

            tmp = (grey[y - 1][x - 1] * sharpen[1 - 1][1 - 1] +
                grey[y - 1][x    ] * sharpen[1 - 1][1    ] +
                grey[y - 1][x + 1] * sharpen[1 - 1][1 + 1] +
                grey[y    ][x - 1] * sharpen[1    ][1 - 1] +
                grey[y    ][x    ] * sharpen[1    ][1    ] +
                grey[y    ][x + 1] * sharpen[1    ][1 + 1] +
                grey[y + 1][x - 1] * sharpen[1 + 1][1 - 1] +
                grey[y + 1][x    ] * sharpen[1 + 1][1    ] +
                grey[y + 1][x + 1] * sharpen[1 + 1][1 + 1]);
            if (tmp > 255) tmp = 255; // Clamp overflow to white
            if (tmp < 0) tmp = 0; // Clamp underflow to black
            sharp[y][x] = tmp;
        }
    }

    //write_pgm("grey.pgm", grey);
    //write_pgm("grey.pgm", grey_out);

    write_pgm("blur.pgm", blur);
    write_pgm("sharp.pgm", sharpen);

    return 0;
}