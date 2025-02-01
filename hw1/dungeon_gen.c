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
                d->dungeon[i][j].visible = ' ';
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
            d->rooms[r].connected = 0;
            r+=1;
        }
        // r+=1;
        // printf("next room: %d\n", r);
    }

    d->rooms_num = r;
    
    return 0;
}

int dungeon_corridor_gen(struct dungeon *d)
{
    int i, j, k, R1, R2, oneCont, lastCont, R1x, R1y, R2x, R2y, curveOff;//, Rdisx, Rdisy;
    double closedis, dis;
    R1 = 0;
    R2 = 1;
    oneCont = 0;
    lastCont = 0;

    for(k = 0; k < d->rooms_num; k++){
        
        if(lastCont == 0){
            closedis = 100.0;
            for(i = 0; i < d->rooms_num; i++){
                for(j = 0; j < d->rooms_num; j++){
                    if(i == j || (d->rooms[i].connected == 1 && d->rooms[j].connected == 1) || (!(d->rooms[i].connected == 1) && !(d->rooms[j].connected == 1) && oneCont == 1)){
                        continue;
                    }
                    // Euclidean distance calculation
                    double x1 = (d->rooms[i].x_left + d->rooms[i].x_right) / 2.0;
                    double y1 = (d->rooms[i].y_high + d->rooms[i].y_low) / 2.0;
                    double x2 = (d->rooms[j].x_left + d->rooms[j].x_right) / 2.0;
                    double y2 = (d->rooms[j].y_high + d->rooms[j].y_low) / 2.0;
                    dis = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

                    if (dis < closedis){
                        R1 = i;
                        R2 = j;
                        closedis = dis;
                    }
                }
            }
        }else{
            R1 = rand() % d->rooms_num;
            R2 = rand() % d->rooms_num;
            while(R1 == R2){
                R2 = rand() % d->rooms_num;
            }
            closedis = 100.0;
        }

        printf("R1  x_left: %d and y_high: %d and length: %d and height: %d and connected: %d\n", d->rooms[R1].x_left, d->rooms[R1].y_high, d->rooms[R1].length, d->rooms[R1].height, d->rooms[R1].connected);
        printf("R2  x_left: %d and y_high: %d and length: %d and height: %d and connected: %d\n", d->rooms[R2].x_left, d->rooms[R2].y_high, d->rooms[R2].length, d->rooms[R2].height, d->rooms[R2].connected);
        printf("Distance: %d\n", (int)closedis);

        if(d->rooms[R1].length > 4){
            R1x = (rand() % (d->rooms[R1].length - 4)) + d->rooms[R1].x_left + 2;
        }else{
            R1x = (rand() % (d->rooms[R1].length - 2)) + d->rooms[R1].x_left + 1;
        }
        if(d->rooms[R2].length > 4){
            R2x = (rand() % (d->rooms[R2].length - 4)) + d->rooms[R2].x_left + 2;
        }else{
            R2x = (rand() % (d->rooms[R2].length - 2)) + d->rooms[R2].x_left + 1;
        }
        if(d->rooms[R1].height > 4){
            R1y = (rand() % (d->rooms[R1].height - 4)) + d->rooms[R1].y_high + 2;
        }else{
            R1y = (rand() % (d->rooms[R1].height - 2)) + d->rooms[R1].y_high + 1;
        }
        if(d->rooms[R2].height > 4){
            R2y = (rand() % (d->rooms[R2].height - 4)) + d->rooms[R2].y_high + 2;
        }else{
            R2y = (rand() % (d->rooms[R2].height - 2)) + d->rooms[R2].y_high + 1;
        }

        // if(R2x >= R1x){
        //     for(i = R1x; i <= R2x; i++){
        //         if(d->dungeon[R1y][i].type != 2){
        //             d->dungeon[R1y][i].type = 3;
        //             d->dungeon[R1y][i].visible = '#';
        //             d->dungeon[R1y][i].hardness = 0;               
        //         }
        //     }
        //     if(R2y >= R1y){
        //         for(i = R1y; i <= R2y; i++){
        //             if(d->dungeon[i][R2x].type != 2){
        //                 d->dungeon[i][R2x].type = 3;
        //                 d->dungeon[i][R2x].visible = '#';
        //                 d->dungeon[i][R2x].hardness = 0;               
        //             }
        //         }
        //     }else{
        //         for(i = R2y; i <= R1y; i++){
        //             if(d->dungeon[i][R2x].type != 2){
        //                 d->dungeon[i][R2x].type = 3;
        //                 d->dungeon[i][R2x].visible = '#';
        //                 d->dungeon[i][R2x].hardness = 0;               
        //             }
        //         }
        //     }
        // }else{
        //     for(i = R2x; i <= R1x; i++){
        //         if(d->dungeon[R2y][i].type != 2){
        //             d->dungeon[R2y][i].type = 3;
        //             d->dungeon[R2y][i].visible = '#';
        //             d->dungeon[R2y][i].hardness = 0;               
        //         }
        //     }
        //     if(R2y >= R1y){
        //         for(i = R1y; i <= R2y; i++){
        //             if(d->dungeon[i][R1x].type != 2){
        //                 d->dungeon[i][R1x].type = 3;
        //                 d->dungeon[i][R1x].visible = '#';
        //                 d->dungeon[i][R1x].hardness = 0;               
        //             }
        //         }
        //     }else{
        //         for(i = R2y; i <= R1y; i++){
        //             if(d->dungeon[i][R1x].type != 2){
        //                 d->dungeon[i][R1x].type = 3;
        //                 d->dungeon[i][R1x].visible = '#';
        //                 d->dungeon[i][R1x].hardness = 0;               
        //             }
        //         }
        //     }
        // }

        ///////////////////////////////////////////

        if(R2x >= R1x){
            // for(i = R2x; i >= R1x; i--){
            //     if(d->dungeon[R1y][i].type != 2){
            //         d->dungeon[R1y][i].type = 3;
            //         d->dungeon[R1y][i].visible = '#';
            //         d->dungeon[R1y][i].hardness = 0;               
            //     }
            // }

            // R2 is below and right of R1
            if(R2y >= R1y){
                curveOff = 0;
                for(i = R2x; i >= R1x; i--){
                    if(d->dungeon[R1y - curveOff][i].type != 2){
                        if(i != R2x && lastCont == 1 && (d->dungeon[R1y - curveOff][i].type == 3 || d->dungeon[R1y - (curveOff + 1)][i].type == 3)){
                            break;
                        }
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R1y - (curveOff + 1) > d->rooms[R1].y_high) && i != R2x && d->dungeon[R1y - (curveOff + 1)][i].type != 2){
                            d->dungeon[R1y - curveOff][i].type = 3;
                            d->dungeon[R1y - curveOff][i].visible = '#';
                            d->dungeon[R1y - curveOff][i].hardness = 0;
                            curveOff += 1;
                        }
                        d->dungeon[R1y - curveOff][i].type = 3;
                        d->dungeon[R1y - curveOff][i].visible = '#';
                        d->dungeon[R1y - curveOff][i].hardness = 0;               
                    }
                }
                curveOff = 0;
                for(i = R1y; i <= R2y; i++){
                    if(d->dungeon[i][R2x + curveOff].type != 2){
                        if(i != R1y && lastCont == 1 && (d->dungeon[i][R2x + curveOff].type == 3 || d->dungeon[i][R2x + (curveOff + 1)].type == 3)){
                            break;
                        }
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R2x + (curveOff + 1) < d->rooms[R2].x_right) && i != R1y && d->dungeon[i][R2x + (curveOff + 1)].type != 2){
                            d->dungeon[i][R2x + curveOff].type = 3;
                            d->dungeon[i][R2x + curveOff].visible = '#';
                            d->dungeon[i][R2x + curveOff].hardness = 0; 
                            curveOff += 1;
                        }
                        d->dungeon[i][R2x + curveOff].type = 3;
                        d->dungeon[i][R2x + curveOff].visible = '#';
                        d->dungeon[i][R2x + curveOff].hardness = 0;               
                    }
                }
            // R2 is above and right of R1
            }else{
                curveOff = 0;
                for(i = R2x; i >= R1x; i--){
                    if(d->dungeon[R1y + curveOff][i].type != 2){
                        if(i != R2x && lastCont == 1 && (d->dungeon[R1y + curveOff][i].type == 3 || d->dungeon[R1y + (curveOff + 1)][i].type == 3)){
                            break;
                        }
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R1y + (curveOff + 1) < d->rooms[R1].y_low) && i != R2x && d->dungeon[R1y + (curveOff + 1)][i].type != 2){
                            d->dungeon[R1y + curveOff][i].type = 3;
                            d->dungeon[R1y + curveOff][i].visible = '#';
                            d->dungeon[R1y + curveOff][i].hardness = 0;
                            curveOff += 1;
                        }
                        d->dungeon[R1y + curveOff][i].type = 3;
                        d->dungeon[R1y + curveOff][i].visible = '#';
                        d->dungeon[R1y + curveOff][i].hardness = 0;               
                    }
                }
                curveOff = 0;
                for(i = R1y; i >= R2y; i--){
                    if(d->dungeon[i][R2x + curveOff].type != 2){
                        if(i != R1y && lastCont == 1 && (d->dungeon[i][R2x + curveOff].type == 3 || d->dungeon[i][R2x + (curveOff + 1)].type == 3)){
                            break;
                        }
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R2x + (curveOff + 1) < d->rooms[R2].x_right) && i != R1y && d->dungeon[i][R2x + (curveOff + 1)].type != 2){
                            d->dungeon[i][R2x + curveOff].type = 3;
                            d->dungeon[i][R2x + curveOff].visible = '#';
                            d->dungeon[i][R2x + curveOff].hardness = 0; 
                            curveOff += 1;
                        }
                        d->dungeon[i][R2x + curveOff].type = 3;
                        d->dungeon[i][R2x + curveOff].visible = '#';
                        d->dungeon[i][R2x + curveOff].hardness = 0;               
                    }
                }
            }
        }else{
            // for(i = R2x; i <= R1x; i++){
            //     if(d->dungeon[R2y][i].type != 2){
            //         d->dungeon[R2y][i].type = 3;
            //         d->dungeon[R2y][i].visible = '#';
            //         d->dungeon[R2y][i].hardness = 0;               
            //     }
            // }

            // R2 is below and left of R1
            if(R2y >= R1y){
                curveOff = 0;
                for(i = R1x; i >= R2x; i--){
                    if(d->dungeon[R2y + curveOff][i].type != 2){
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R2y + (curveOff + 1) < d->rooms[R2].y_low) && i != R1x && d->dungeon[R2y + (curveOff + 1)][i].type != 2){
                            d->dungeon[R2y + curveOff][i].type = 3;
                            d->dungeon[R2y + curveOff][i].visible = '#';
                            d->dungeon[R2y + curveOff][i].hardness = 0;  
                            curveOff += 1;
                        }
                        d->dungeon[R2y + curveOff][i].type = 3;
                        d->dungeon[R2y + curveOff][i].visible = '#';
                        d->dungeon[R2y + curveOff][i].hardness = 0;               
                    }
                }
                curveOff = 0;
                for(i = R2y; i >= R1y; i--){
                    if(d->dungeon[i][R1x + curveOff].type != 2){
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R1x + (curveOff + 1) < d->rooms[R1].x_right) && i != R2y && d->dungeon[i][R1x + (curveOff + 1)].type != 2){
                            d->dungeon[i][R1x + curveOff].type = 3;
                            d->dungeon[i][R1x + curveOff].visible = '#';
                            d->dungeon[i][R1x + curveOff].hardness = 0; 
                            curveOff += 1;
                        }
                        d->dungeon[i][R1x + curveOff].type = 3;
                        d->dungeon[i][R1x + curveOff].visible = '#';
                        d->dungeon[i][R1x + curveOff].hardness = 0;               
                    }
                }
                // R2 is above and left of R1
            }else{
                curveOff = 0;
                for(i = R1x; i >= R2x; i--){
                    if(d->dungeon[R2y - curveOff][i].type != 2){
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R2y - (curveOff + 1) > d->rooms[R2].y_high) && i != R1x && d->dungeon[R2y - (curveOff + 1)][i].type != 2){
                            d->dungeon[R2y - curveOff][i].type = 3;
                            d->dungeon[R2y - curveOff][i].visible = '#';
                            d->dungeon[R2y - curveOff][i].hardness = 0;  
                            curveOff += 1;
                        }
                        d->dungeon[R2y - curveOff][i].type = 3;
                        d->dungeon[R2y - curveOff][i].visible = '#';
                        d->dungeon[R2y - curveOff][i].hardness = 0;               
                    }
                }
                curveOff = 0;
                for(i = R2y; i <= R1y; i++){
                    if(d->dungeon[i][R1x + curveOff].type != 2){
                        // 1 in 3 chance of corridor curving
                        // as long as curve doesn't make the corridor miss the target room
                        if(rand() % 3 == 0 && (R1x + (curveOff + 1) < d->rooms[R1].x_right) && i != R2y && d->dungeon[i][R1x + (curveOff + 1)].type != 2){
                            d->dungeon[i][R1x + curveOff].type = 3;
                            d->dungeon[i][R1x + curveOff].visible = '#';
                            d->dungeon[i][R1x + curveOff].hardness = 0; 
                            curveOff += 1;
                        }
                        d->dungeon[i][R1x + curveOff].type = 3;
                        d->dungeon[i][R1x + curveOff].visible = '#';
                        d->dungeon[i][R1x + curveOff].hardness = 0;               
                    }
                }
            }
        }

        oneCont = 1;
        d->rooms[R1].connected = 1;
        d->rooms[R2].connected = 1;
        if(k == d->rooms_num - 2){
            lastCont = 1;
        }
        
    }
    

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