#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define MAX_x 80
#define MAX_y 21
#define MAX_rooms 10
#define MIN_room_length 4
#define MAX_room_length 15
#define MIN_room_height 3
#define MAX_room_height 9



struct dungeon_cell {
/*
types:  0 = dungeon edge (immutable rock)
        1 = rock
        2 = room
        3 = corridor
        4 = stair_up
        5 = stair_down
*/
    int type;
    char visible;
    int hardness;
};

struct room {
    int length;
    int height;
    int x_left;
    int x_right;
    int y_high;
    int y_low;
    int connected; // 0 is no, 1 is yes
};

struct dungeon {
    struct dungeon_cell dungeon[MAX_y][MAX_x];
    struct room rooms[MAX_rooms];
    int rooms_num;
};

int dungeon_init(struct dungeon *d);
int dungeon_print(struct dungeon *d);
int dungeon_rooms_gen(struct dungeon *d);
int dungeon_corridor_gen(struct dungeon *d);


#endif