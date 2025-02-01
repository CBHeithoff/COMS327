#include "dungeon_gen.h"

//gcc dungeon_gen.c dungeon_gen_main.c -Wall -Werror -o dungeon_gen_main


int main(int argc, char *argv[])
{
    struct dungeon d;

    srand(time(NULL));  

    if(dungeon_init(&d)){
        return 0;
    }

    //dungeon_print(&d);

    dungeon_rooms_gen(&d);

    dungeon_print(&d);

    dungeon_corridor_gen(&d);

    dungeon_print(&d);

    return 0;
}