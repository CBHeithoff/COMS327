#include "dungeon_gen.h"

int dungeon_init(struct dungeon *d)
{
    int i, j;

    d->rooms_num = 0;

    for(i = 0; i < MAX_y; i++){
        for(j = 0; j < MAX_x; j++){
            //printf("i and j: %d %d\n", i, j);
            if((i == 0) || (i == MAX_y - 1) || (j == 0) || (j == MAX_x -1)){
                d->dungeon[i][j].type = 0;
                d->dungeon[i][j].visible = 'W';
                d->dungeon[i][j].hardness = 100;
            }else{
                d->dungeon[i][j].type = 1;
                d->dungeon[i][j].visible = 'r';
                d->dungeon[i][j].hardness = 50;
            }
            //printf("type and hardness: %d %d\n", d->dungeon[i][j].type, d->dungeon[i][j].hardness);
        }
    }

    return 0;
}

int dungeon_rooms_gen(struct dungeon *d)
{
    int r, i, j, flag, extra_rooms;
    r = 0;
    extra_rooms = rand() % 4;

    while(r < (6 + extra_rooms)){
        flag = 0;
        d->rooms[r].x_left = rand() % MAX_x;
        d->rooms[r].y_high = rand() % MAX_y;
        d->rooms[r].length = (rand() % (MAX_room_length - MIN_room_length + 1)) + MIN_room_length;
        d->rooms[r].height = (rand() % (MAX_room_height - MIN_room_height + 1)) + MIN_room_height;
        d->rooms[r].x_right = d->rooms[r].x_left + d->rooms[r].length - 1;
        d->rooms[r].y_low = d->rooms[r].y_high + d->rooms[r].height - 1;

        printf("x_left: %d and y_high: %d and length: %d and height: %d\n", d->rooms[r].x_left, d->rooms[r].y_high, d->rooms[r].length, d->rooms[r].height);

        for(i = d->rooms[r].y_high; i <= d->rooms[r].y_low; i++){
            for(j = d->rooms[r].x_left; j <= d->rooms[r].x_right; j++){
                if(d->dungeon[i][j].type == 0 || d->dungeon[i][j].type == 2){
                    //printf("try: %d\n", try);
                    flag = 1;
                    break;
                }
                else if(d->dungeon[i-1][j-1].type == 2 || d->dungeon[i+1][j+1].type == 2 || d->dungeon[i+1][j-1].type == 2 || d->dungeon[i-1][j+1].type == 2){
                    //printf("try: %d\n", try);
                    flag = 1;
                    break;
                }
            }
            if(flag){
                break;
            }
        }
        if(!flag){
            printf("room: %d\n", r);     
            for(i = d->rooms[r].y_high; i <= d->rooms[r].y_low; i++){
                for(j = d->rooms[r].x_left; j <= d->rooms[r].x_right; j++){  
                    d->dungeon[i][j].type = 2;
                    d->dungeon[i][j].visible = '.';
                    d->dungeon[i][j].hardness = 0; 
                }
            }
            r+=1;
        }
        // r+=1;
        // printf("next room: %d\n", r);
    }

    d->rooms_num = r;
    
    return 0;
}

int dungeon_print(struct dungeon *d)
{
    int i, j;

    for(i = 0; i < MAX_y; i++){
        for(j = 0; j < MAX_x; j++){
            printf("%c", d->dungeon[i][j].visible);
        }
        printf("\n");
    }

    return 0;
}