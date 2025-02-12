#include <stdio.h>
#include <ctype.h>

char *words[] = {
    "alex",
    "banana",
    "car",
    "door",
    "elephant",
    "froyo",
    "gold",
    "horse",
    "idiomatic",
    "jump",
    "kernel",
    "linux",
    "mouse",
    "no",
    "octopus",
    "parser",
    "quartz",
    "ready",
    "stalagmite",
    "torvalds",
    "unc",
    "wow",
    "x-ray",
    "yoga",
    "zowee"
};

int main(int argc, char *argv[])
{
    char c;

    c = argv[1][0];
    
    // printf("%c if for %s!\n", c, words[c - 97]);
    //printf("%c if for %s!\n", c, words[c - 'a']);
    printf("%c if for %s!\n", c, words[tolower(c) - 'a']);

    return 0;
}
