#include <stdlib.h>
#include <iostream>

#include "utils.h"
#include "dungeon.h"
#include "object.h"
#include "descriptions.h"

void gen_objects(dungeon *d){
    uint32_t i;
    object *o;
    uint32_t room;
    pair_t p;

    uint32_t num_objects = 10; // default number of objects

    for (i = 0; i < num_objects; i++) {

        // avoid infinite loop by breaking if there are no eligible objects left
        bool all_artifact = true;
        for (size_t j = 0; j < d->artifact_objects.size(); ++j) {
            if (d->artifact_objects[j] != 1) {
                all_artifact = false;
                break;
            }
        }
        if(all_artifact){
          break;
        }

        // determining the object template for this object
        int obj_desc_id;
        do {
            obj_desc_id = rand_range(0, d->object_descriptions.size() - 1);
        } while (d->artifact_objects[obj_desc_id] || 
                (uint32_t)rand_range(0, 99) >= d->object_descriptions[obj_desc_id].get_rarity());

        // creating the object
        o = create_object(d->object_descriptions[obj_desc_id]);
        if(d->object_descriptions[obj_desc_id].get_artifact()){
            d->artifact_objects[obj_desc_id] = 1;
        }

        // determining the location of the object
        do {
            room = rand_range(1, d->num_rooms - 1);
            p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                                    (d->rooms[room].position[dim_y] +
                                    d->rooms[room].size[dim_y] - 1));
            p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                                    (d->rooms[room].position[dim_x] +
                                    d->rooms[room].size[dim_x] - 1));
        } while (d->object_map[p[dim_y]][p[dim_x]]);

        // determining object details
        o->obj_desc_id = obj_desc_id;
        d->object_map[p[dim_y]][p[dim_x]] = o;

    }

    // DEBUGGING
    // for(int y = 0; y < DUNGEON_Y; y++){
    //     for (int x = 0; x < DUNGEON_X; x++) {
    //       if (d->object_map[y][x]){
    //         std::cout << "Name: " << d->object_map[y][x]->name << "\n";
    //         std::cout << "Symbol: " << d->object_map[y][x]->symbol << "\n\n";
    //       }
    //     }
    // }

}

