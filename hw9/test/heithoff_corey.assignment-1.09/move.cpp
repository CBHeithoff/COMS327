#include "move.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"
#include "event.h"
#include "io.h"
#include "npc.h"
#include "object.h"

void do_combat(dungeon *d, character *atk, character *def)
{
  int can_see_atk, can_see_def;
  const char *organs[] = {
    "liver",                   /*  0 */
    "pancreas",                /*  1 */
    "heart",                   /*  2 */
    "eye",                     /*  3 */
    "arm",                     /*  4 */
    "leg",                     /*  5 */
    "intestines",              /*  6 */
    "gall bladder",            /*  7 */
    "lungs",                   /*  8 */
    "hand",                    /*  9 */
    "foot",                    /* 10 */
    "spinal cord",             /* 11 */
    "pituitary gland",         /* 12 */
    "thyroid",                 /* 13 */
    "tongue",                  /* 14 */
    "bladder",                 /* 15 */
    "diaphram",                /* 16 */
    "stomach",                 /* 17 */
    "pharynx",                 /* 18 */
    "esophagus",               /* 19 */
    "trachea",                 /* 20 */
    "urethra",                 /* 21 */
    "spleen",                  /* 22 */
    "ganglia",                 /* 23 */
    "ear",                     /* 24 */
    "subcutaneous tissue"      /* 25 */
    "cerebellum",              /* 26 */ /* Brain parts begin here */
    "hippocampus",             /* 27 */
    "frontal lobe",            /* 28 */
    "brain",                   /* 29 */
  };
  int part;

  if (def->alive) {
    def->alive = 0;
    charpair(def->position) = NULL;
    
    if (def != d->PC) {
      d->num_monsters--;
    } else {
      if ((part = rand() % (sizeof (organs) / sizeof (organs[0]))) < 26) {
        io_queue_message("As %s%s eats your %s,", is_unique(atk) ? "" : "the ",
                         atk->name, organs[rand() % (sizeof (organs) /
                                                     sizeof (organs[0]))]);
        io_queue_message("   ...you wonder if there is an afterlife.");
        /* Queue an empty message, otherwise the game will not pause for *
         * player to see above.                                          */
        io_queue_message("");
      } else {
        io_queue_message("Your last thoughts fade away as "
                         "%s%s eats your %s...",
                         is_unique(atk) ? "" : "the ",
                         atk->name, organs[part]);
        io_queue_message("");
      }
      /* Queue an empty message, otherwise the game will not pause for *
       * player to see above.                                          */
      io_queue_message("");
    }
    atk->kills[kill_direct]++;
    atk->kills[kill_avenged] += (def->kills[kill_direct] +
                                  def->kills[kill_avenged]);
  }

  if (atk == d->PC) {
    io_queue_message("You smite %s%s!", is_unique(def) ? "" : "the ", def->name);
  }

  can_see_atk = can_see(d, character_get_pos(d->PC),
                        character_get_pos(atk), 1, 0);
  can_see_def = can_see(d, character_get_pos(d->PC),
                        character_get_pos(def), 1, 0);

  if (atk != d->PC && def != d->PC) {
    if (can_see_atk && !can_see_def) {
      io_queue_message("%s%s callously murders some poor, "
                       "defenseless creature.",
                       is_unique(atk) ? "" : "The ", atk->name);
    }
    if (can_see_def && !can_see_atk) {
      io_queue_message("Something kills %s%s.",
                       is_unique(def) ? "" : "the helpless ", def->name);
    }
    if (can_see_atk && can_see_def) {
      io_queue_message("You watch in abject horror as %s%s "
                       "gruesomely murders %s%s!",
                       is_unique(atk) ? "" : "the ", atk->name,
                       is_unique(def) ? "" : "the ", def->name);
    }
  }
}

void do_better_combat(dungeon *d, character *atk, character *def)
{
  const char *organs[] = {
    "liver",                   /*  0 */
    "pancreas",                /*  1 */
    "heart",                   /*  2 */
    "eye",                     /*  3 */
    "arm",                     /*  4 */
    "leg",                     /*  5 */
    "intestines",              /*  6 */
    "gall bladder",            /*  7 */
    "lungs",                   /*  8 */
    "hand",                    /*  9 */
    "foot",                    /* 10 */
    "spinal cord",             /* 11 */
    "pituitary gland",         /* 12 */
    "thyroid",                 /* 13 */
    "tongue",                  /* 14 */
    "bladder",                 /* 15 */
    "diaphram",                /* 16 */
    "stomach",                 /* 17 */
    "pharynx",                 /* 18 */
    "esophagus",               /* 19 */
    "trachea",                 /* 20 */
    "urethra",                 /* 21 */
    "spleen",                  /* 22 */
    "ganglia",                 /* 23 */
    "ear",                     /* 24 */
    "subcutaneous tissue"      /* 25 */
    "cerebellum",              /* 26 */ /* Brain parts begin here */
    "hippocampus",             /* 27 */
    "frontal lobe",            /* 28 */
    "brain",                   /* 29 */
  };
  int part;
  
  // damage dealing attack
  if (def->alive) {
    // npc vs npc displacement
    if (atk != d->PC && def != d->PC){
      int directions[8][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
      };
      int start_y = def->position[dim_y];
      int start_x = def->position[dim_x];
      int new_x, new_y;
      bool pos_found = false;

      // finding suitable position
      for (auto& dir : directions) {
        new_y = start_y + dir[0];
        new_x = start_x + dir[1];
        if(atk->position[dim_y] == new_y && atk->position[dim_x] == new_x){
          continue;
        }
        if (new_y >= 0 && new_y < DUNGEON_Y && new_x >= 0 && new_x < DUNGEON_X && 
            d->character_map[new_y][new_x] == NULL &&
            d->map[new_y][new_x] > ter_floor) {
          pos_found = true;
          break;
        }
      }
      // displacing
      if(pos_found){
        d->character_map[atk->position[dim_y]][atk->position[dim_x]] = NULL;
        def->position[dim_y] = new_y;
        def->position[dim_x] = new_x;
        d->character_map[new_y][new_x] = def;
        atk->position[dim_y] = start_y;
        atk->position[dim_x] = start_x;
        d->character_map[start_y][start_x] = atk;
      }
      // swapping
      else{
        def->position[dim_y] = atk->position[dim_y];
        def->position[dim_x] = atk->position[dim_x];
        d->character_map[atk->position[dim_y]][atk->position[dim_x]] = def;
        atk->position[dim_y] = start_y;
        atk->position[dim_x] = start_x;
        d->character_map[start_y][start_x] = atk;
      }
      return;
    }

    // PC is defending
    if (atk != d->PC && def == d->PC){
      int atk_damage = atk->damage->roll();
      d->PC->hp = d->PC->hp - atk_damage;
      
      if (d->PC->hp < 1){
        def->alive = 0;
        charpair(def->position) = NULL;
        
        if ((part = rand() % (sizeof (organs) / sizeof (organs[0]))) < 26) {
          io_queue_message("As %s%s eats your %s,", is_unique(atk) ? "" : "the ",
                           atk->name, organs[rand() % (sizeof (organs) /
                                                       sizeof (organs[0]))]);
          io_queue_message("   ...you wonder if there is an afterlife.");
          /* Queue an empty message, otherwise the game will not pause for *
           * player to see above.                                          */
          io_queue_message("");
        } else {
          io_queue_message("Your last thoughts fade away as "
                           "%s%s eats your %s...",
                           is_unique(atk) ? "" : "the ",
                           atk->name, organs[part]);
          io_queue_message("");
        }
        /* Queue an empty message, otherwise the game will not pause for *
         * player to see above.                                          */
        io_queue_message("");
        atk->kills[kill_direct]++;
        atk->kills[kill_avenged] += (def->kills[kill_direct] +
                                    def->kills[kill_avenged]);
      }
      else{
        // io_queue_message("You've been hit by %s%s! Your now have %d hp", is_unique(def) ? "" : "the ", def->name, d->PC->hp);
        // io_queue_message("");
      }
      //io_queue_message("");
    }
    // PC is attacking
    else if (atk == d->PC && def != d->PC){
      int atk_damage = d->PC->damage->roll();
      for(int i = 0; i < num_equipment; i++){
        if(d->PC->equipment[i] != NULL){
          atk_damage += d->PC->equipment[i]->roll_dice();
        }
      }
      def->hp = def->hp - atk_damage;

      //DEBUGGING
      // io_queue_message("%d attack damage! Monster hp = %d", atk_damage, def->hp);
      // io_queue_message("");

      if(def->hp < 1){
        // if monster with PICKUP ability, drop items at death
        if(has_characteristic(def, PICKUP_OBJ)){
          for(int i = 0; i < INVENTORY_SIZE; i++){
            if (def->inventory[i]) {
              int pos_y = def->position[dim_y];
              int pos_x = def->position[dim_x];
              if (d->objmap[pos_y][pos_x] == NULL){
                d->objmap[pos_y][pos_x] = def->inventory[i];
                def->inventory[i] = NULL;
              }
              else{
                def->inventory[i]->set_next(d->objmap[pos_y][pos_x]);
                d->objmap[pos_y][pos_x] = def->inventory[i];
                def->inventory[i] = NULL;
              }
            }
          }
        }

        def->alive = 0;
        charpair(def->position) = NULL;
        d->num_monsters--;
        io_queue_message("You smite %s%s!", is_unique(def) ? "" : "the ", def->name);
        io_queue_message("");
        atk->kills[kill_direct]++;
        atk->kills[kill_avenged] += (def->kills[kill_direct] +
                                    def->kills[kill_avenged]);
                                  
      }
      else{
        // io_queue_message("You attacked %s%s!", is_unique(def) ? "" : "the ", def->name);
        // io_queue_message("");
      }
      //io_queue_message("");


    }
  }

}

void move_character(dungeon *d, character *c, pair_t next)
{
  if (charpair(next) &&
      ((next[dim_y] != c->position[dim_y]) ||
       (next[dim_x] != c->position[dim_x]))) {
    do_better_combat(d, c, charpair(next));
  } else {
    /* No character in new position. */

    d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
    c->position[dim_y] = next[dim_y];
    c->position[dim_x] = next[dim_x];
    d->character_map[c->position[dim_y]][c->position[dim_x]] = c;
  }

  if (c == d->PC) {
    pc_reset_visibility(d->PC);
    pc_observe_terrain(d->PC, d);

    // automatic item pickup (can handle stacks of items)
    if(d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] != NULL){
      object *o = d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]];
      d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = NULL;
      int i = 0;
      while(o != NULL){
        if(!d->PC->inventory[i]){
          d->PC->inventory[i] = o;
          o = d->PC->inventory[i]->get_next();
        }
        i++;
        if(i >= INVENTORY_SIZE){
          d->objmap[d->PC->position[dim_y]][d->PC->position[dim_x]] = o;
          break;
        }
      }
      
    }
  }

  if(c != d->PC){
    // handling monsters with DESTROY ability
    if(has_characteristic(c, DESTROY_OBJ)){
      if(d->objmap[c->position[dim_y]][c->position[dim_x]]){
        d->objmap[c->position[dim_y]][c->position[dim_x]] = NULL;
      }
    }

    // handling monsters with PICKUP ability
    if(has_characteristic(c, PICKUP_OBJ)){
      if(d->objmap[c->position[dim_y]][c->position[dim_x]] != NULL){
        object *o = d->objmap[c->position[dim_y]][c->position[dim_x]];
        d->objmap[c->position[dim_y]][c->position[dim_x]] = NULL;
        int i = 0;
        while(o != NULL){
          if(!c->inventory[i]){
            c->inventory[i] = o;
            o = c->inventory[i]->get_next();
          }
          i++;
          if(i >= INVENTORY_SIZE){
            d->objmap[c->position[dim_y]][c->position[dim_x]] = o;
            break;
          }
        }
        
      }
    }
  }
}

void do_moves(dungeon *d)
{
  pair_t next;
  character *c;
  event *e;

  /* Remove the PC when it is PC turn.  Replace on next call.  This allows *
   * use to completely uninit the heap when generating a new level without *
   * worrying about deleting the PC.                                       */

  if (pc_is_alive(d)) {
    /* The PC always goes first one a tie, so we don't use new_event().  *
     * We generate one manually so that we can set the PC sequence       *
     * number to zero.                                                   */
    e = (event *) malloc(sizeof (*e));
    e->type = event_character_turn;
    /* Hack: New dungeons are marked.  Unmark and ensure PC goes at d->time, *
     * otherwise, monsters get a turn before the PC.                         */
    if (d->is_new) {
      d->is_new = 0;
      e->time = d->time;
    } else {
      e->time = d->time + (1000 / d->PC->speed);
    }
    e->sequence = 0;
    e->c = d->PC;
    heap_insert(&d->events, e);
  }

  while (pc_is_alive(d) &&
         (e = (event *) heap_remove_min(&d->events)) &&
         ((e->type != event_character_turn) || (e->c != d->PC))) {
    d->time = e->time;
    if (e->type == event_character_turn) {
      c = e->c;
    }
    if (!c->alive) {
      if (d->character_map[c->position[dim_y]][c->position[dim_x]] == c) {
        d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
      }
      if (c != d->PC) {
        event_delete(e);
      }
      continue;
    }

    npc_next_pos(d, (npc *) c, next);
    move_character(d, (npc *) c, next);

    heap_insert(&d->events, update_event(d, e, 1000 / c->speed));
  }

  io_display(d);
  if (pc_is_alive(d) && e->c == d->PC) {
    c = e->c;
    d->time = e->time;
    /* Kind of kludgey, but because the PC is never in the queue when   *
     * we are outside of this function, the PC event has to get deleted *
     * and recreated every time we leave and re-enter this function.    */
    e->c = NULL;
    event_delete(e);
    io_handle_input(d);
  }
}

void dir_nearest_wall(dungeon *d, character *c, pair_t dir)
{
  dir[dim_x] = dir[dim_y] = 0;

  if (c->position[dim_x] != 1 && c->position[dim_x] != DUNGEON_X - 2) {
    dir[dim_x] = (c->position[dim_x] > DUNGEON_X - c->position[dim_x] ? 1 : -1);
  }
  if (c->position[dim_y] != 1 && c->position[dim_y] != DUNGEON_Y - 2) {
    dir[dim_y] = (c->position[dim_y] > DUNGEON_Y - c->position[dim_y] ? 1 : -1);
  }
}

uint32_t against_wall(dungeon *d, character *c)
{
  return ((mapxy(c->position[dim_x] - 1,
                 c->position[dim_y]    ) == ter_wall_immutable) ||
          (mapxy(c->position[dim_x] + 1,
                 c->position[dim_y]    ) == ter_wall_immutable) ||
          (mapxy(c->position[dim_x]    ,
                 c->position[dim_y] - 1) == ter_wall_immutable) ||
          (mapxy(c->position[dim_x]    ,
                 c->position[dim_y] + 1) == ter_wall_immutable));
}

uint32_t in_corner(dungeon *d, character *c)
{
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(c->position[dim_x] - 1,
                          c->position[dim_y]    ) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x] + 1,
                          c->position[dim_y]    ) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x]    ,
                          c->position[dim_y] - 1) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x]    ,
                          c->position[dim_y] + 1) == ter_wall_immutable);

  return num_immutable > 1;
}

static void new_dungeon_level(dungeon *d, uint32_t dir)
{
  /* Eventually up and down will be independantly meaningful. *
   * For now, simply generate a new dungeon.                  */

  switch (dir) {
  case '<':
  case '>':
    new_dungeon(d);
    break;
  default:
    break;
  }
}


uint32_t move_pc(dungeon *d, uint32_t dir)
{
  pair_t next;
  uint32_t was_stairs = 0;
  const char *wallmsg[] = {
    "There's a wall in the way.",
    "BUMP!",
    "Ouch!",
    "You stub your toe.",
    "You can't go that way.",
    "You admire the engravings.",
    "Are you drunk?"
  };

  next[dim_y] = d->PC->position[dim_y];
  next[dim_x] = d->PC->position[dim_x];

  //DEBUGGING
  // io_queue_message("Your hp = %d. Your speed = %d", d->PC->hp, d->PC->speed);
  // io_queue_message("");

  switch (dir) {
  case 1:
  case 2:
  case 3:
    next[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    next[dim_y]--;
    break;
  }
  switch (dir) {
  case 1:
  case 4:
  case 7:
    next[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    next[dim_x]++;
    break;
  case '<':
    if (mappair(d->PC->position) == ter_stairs_up) {
      was_stairs = 1;
      new_dungeon_level(d, '<');
    }
    break;
  case '>':
    if (mappair(d->PC->position) == ter_stairs_down) {
      was_stairs = 1;
      new_dungeon_level(d, '>');
    }
    break;
  }

  if (was_stairs) {
    return 0;
  }

  if ((dir != '>') && (dir != '<') && (mappair(next) >= ter_floor)) {
    move_character(d, d->PC, next);
    dijkstra(d);
    dijkstra_tunnel(d);

    return 0;
  } else if (mappair(next) < ter_floor) {
    io_queue_message(wallmsg[rand() % (sizeof (wallmsg) /
                                       sizeof (wallmsg[0]))]);
    io_display(d);
  }

  return 1;
}
