#include <stdlib.h>
#include <iostream>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"

void pc::pc_delete(dungeon_t *d)
{
  if (d->pc) {
    free(d->pc);
  }
}

uint32_t pc::pc_is_alive(dungeon_t *d)
{
  return d->pc->alive;
}

void pc::place_pc(dungeon_t *d)
{
  d->pc->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));

  // initializing fog of war map
  int y, x;
  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      d->map_seen[y][x] = ter_wall_immutable;
    }
  }
  // fog of war light range
  for (y = d->pc->position[dim_y]-2; y < d->pc->position[dim_y]+3; y++) {
    for (x = d->pc->position[dim_x]-2; x < d->pc->position[dim_x]+3; x++) {
      if(y>=0 && y < DUNGEON_Y && x >=0 && x < DUNGEON_X){
        d->map_seen[y][x] = mapxy(x, y);
      }
    }
  }
}

void pc::config_pc(dungeon_t *d)
{
  d->pc = (character_t *) malloc(sizeof (pc_t));
  memset(d->pc, 0, sizeof (pc_t));
  d->pc->symbol = '@';

  place_pc(d);

  d->pc->speed = PC_SPEED;
  d->pc->alive = 1;
  d->pc->sequence_number = 0;
  d->pc->npc = NULL;
  d->pc->kills[kill_direct] = d->pc->kills[kill_avenged] = 0;

  d->character[d->pc->position[dim_y]][d->pc->position[dim_x]] = d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc::pc_next_pos(dungeon_t *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, d->pc)) {
    if (!count) {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(d->pc->position[dim_x] - 1, d->pc->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  } else if (charxy(d->pc->position[dim_x], d->pc->position[dim_y] - 1)) {
    dir[dim_y] = -1;
  } else if (charxy(d->pc->position[dim_x] + 1, d->pc->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  } else if (charxy(d->pc->position[dim_x] - 1, d->pc->position[dim_y])) {
    dir[dim_x] = -1;
  } else if (charxy(d->pc->position[dim_x] + 1, d->pc->position[dim_y])) {
    dir[dim_x] = 1;
  } else if (charxy(d->pc->position[dim_x] - 1, d->pc->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  } else if (charxy(d->pc->position[dim_x], d->pc->position[dim_y] + 1)) {
    dir[dim_y] = 1;
  } else if (charxy(d->pc->position[dim_x] + 1, d->pc->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  } else if (!have_seen_corner || count < 250) {
    /* Head to a corner and let most of the NPCs kill each other off */
    if (count) {
      count++;
    }
    if (!against_wall(d, d->pc) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir_nearest_wall(d, d->pc, dir);
    }
  }else {
    /* And after we've been there, let's head toward the center of the map. */
    if (!against_wall(d, d->pc) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir[dim_x] = ((d->pc->position[dim_x] > DUNGEON_X / 2) ? -1 : 1);
      dir[dim_y] = ((d->pc->position[dim_y] > DUNGEON_Y / 2) ? -1 : 1);
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(d->pc->position[dim_x] + dir[dim_x],
              d->pc->position[dim_y] + dir[dim_y]) &&
      ((charxy(d->pc->position[dim_x] + dir[dim_x] - 1,
               d->pc->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x] - 1,
                d->pc->position[dim_y] + dir[dim_y] - 1) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x] - 1,
               d->pc->position[dim_y] + dir[dim_y]) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x] - 1,
                d->pc->position[dim_y] + dir[dim_y]) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x] - 1,
               d->pc->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x] - 1,
                d->pc->position[dim_y] + dir[dim_y] + 1) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x],
               d->pc->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x],
                d->pc->position[dim_y] + dir[dim_y] - 1) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x],
               d->pc->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x],
                d->pc->position[dim_y] + dir[dim_y] + 1) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x] + 1,
               d->pc->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x] + 1,
                d->pc->position[dim_y] + dir[dim_y] - 1) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x] + 1,
               d->pc->position[dim_y] + dir[dim_y]) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x] + 1,
                d->pc->position[dim_y] + dir[dim_y]) != d->pc)) ||
       (charxy(d->pc->position[dim_x] + dir[dim_x] + 1,
               d->pc->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->pc->position[dim_x] + dir[dim_x] + 1,
                d->pc->position[dim_y] + dir[dim_y] + 1) != d->pc)))) {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t pc::pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->pc->position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc->position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->pc->position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc->position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}
