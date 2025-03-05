#include <ncurses.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i;
    char a[4] = "\\|/-";

    initscr();

    for (i = 0; i < 320; i++){
        usleep(125000);
        mvaddch(23, i / 4, a[i % 4]);
        refresh();
    }

    endwin();

    return 0;

    // manually get out of curses crash by typing reset then press enter 

    // init function
    
        // raw for mult-bit inputs
        // noecho() to disable printing out input
        // curs_set(0)
        // keypad(stdscr, TRUE) to disable cursor blinking?
        // color stuff

    

    // man nccurses
    // less /usr/include/ncurser.h
        
        // to look at key names

    

}