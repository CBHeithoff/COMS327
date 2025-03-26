#include <stdlib.h>
#include <ncurses.h>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"

void pc_delete(pc_t *pc)
{
  if (pc) {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  memset(&d->pc, 0, sizeof (d->pc));
  d->pc.symbol = '@';

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = calloc(1, sizeof (*d->pc.pc));
  d->pc.npc = NULL;
  d->pc.kills[kill_direct] = d->pc.kills[kill_avenged] = 0;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;
  static int target_room = -1;
  static int target_is_valid = 0;

  if (target_is_valid &&
      (d->pc.position[dim_x] == d->rooms[target_room].position[dim_x]) &&
      (d->pc.position[dim_y] == d->rooms[target_room].position[dim_y])) {
    target_is_valid = 0;
  }
  
  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, &d->pc)) {
    if (!count) {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  } else if (charxy(d->pc.position[dim_x], d->pc.position[dim_y] - 1)) {
    dir[dim_y] = -1;
  } else if (charxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  } else if (charxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y])) {
    dir[dim_x] = -1;
  } else if (charxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y])) {
    dir[dim_x] = 1;
  } else if (charxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  } else if (charxy(d->pc.position[dim_x], d->pc.position[dim_y] + 1)) {
    dir[dim_y] = 1;
  } else if (charxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  } else if (!have_seen_corner || count < 250) {
    /* Head to a corner and let most of the NPCs kill each other off */
    if (count) {
      count++;
    }
    if (!against_wall(d, &d->pc) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir_nearest_wall(d, &d->pc, dir);
    }
  } else {
    /* And after we've been there, let's cycle through the rooms, *
     * one-by-one, until the game ends                            */
    if (target_room == -1) {
      target_room = 0;
      target_is_valid = 1;
    }
    if (!target_is_valid) {
      target_is_valid = 1;
      target_room = (target_room + 1) % d->num_rooms;
    }
    /* When against the dungeon border, always head toward the target; *
     * otherwise, head toward the target with 1/3 probability.         */
    if (against_wall(d, &d->pc) || rand_under(1, 3)) {
      dir[dim_x] = ((d->pc.position[dim_x] >
		     d->rooms[target_room].position[dim_x]) ? -1 : 1);
      dir[dim_y] = ((d->pc.position[dim_y] >
		     d->rooms[target_room].position[dim_y]) ? -1 : 1);
    } else {
      /* Else we'll choose a random direction */
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(d->pc.position[dim_x] + dir[dim_x],
              d->pc.position[dim_y] + dir[dim_y]) &&
      ((charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
               d->pc.position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
                d->pc.position[dim_y] + dir[dim_y] - 1) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
               d->pc.position[dim_y] + dir[dim_y]) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
                d->pc.position[dim_y] + dir[dim_y]) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
               d->pc.position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
                d->pc.position[dim_y] + dir[dim_y] + 1) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x],
               d->pc.position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x],
                d->pc.position[dim_y] + dir[dim_y] - 1) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x],
               d->pc.position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x],
                d->pc.position[dim_y] + dir[dim_y] + 1) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
               d->pc.position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
                d->pc.position[dim_y] + dir[dim_y] - 1) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
               d->pc.position[dim_y] + dir[dim_y]) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
                d->pc.position[dim_y] + dir[dim_y]) != &d->pc)) ||
       (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
               d->pc.position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
                d->pc.position[dim_y] + dir[dim_y] + 1) != &d->pc)))) {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t display_monster_list(dungeon_t *d) {
  int ch;
  int scroll_offset = 0;

  while (1) {
    clear();
    mvprintw(0, 0, "Monster List (ESC to exit)");  

    scroll_monster_list(d, &scroll_offset); 

    ch = getch();  

    switch (ch) {
      case KEY_UP:
        // Scrolling up while staying in bounds
        if (scroll_offset > 0) {
            scroll_offset--;
        }
        break;
      case KEY_DOWN:
        // Scrolling down while staying in bounds
        if (scroll_offset < d->num_monsters - DUNGEON_Y) {  
            scroll_offset++;
        }
        break;
      case 27:  // ESC
        clear();
        render_dungeon_ncurses(d);
        refresh();
        return 0; 
      case 'Q':
        clear();
        render_dungeon_ncurses(d);
        refresh();
        return 1;
      default:
        break;  
    }
  }

  return 0;
}

void scroll_monster_list(dungeon_t *d, int *scroll_offset) {
  int index = 0;   
  int start = *scroll_offset;
  int end = (start + DUNGEON_Y < d->num_monsters) ? start + DUNGEON_Y : d->num_monsters;

  // iterating through monsters
  for (int y = 0; y < DUNGEON_Y; y++) {
    for (int x = 0; x < DUNGEON_X; x++) {
      character_t *monster = d->character[y][x];
      if (monster != NULL && monster != &d->pc && monster->alive) {
        if (index >= start && index < end) {
          int x_dist = x - d->pc.position[dim_x];
          int y_dist = y - d->pc.position[dim_y];

          const char *x_dir = (x_dist > 0) ? "east" : "west"; 
          const char *y_dir = (y_dist > 0) ? "south" : "north";

          mvprintw(index - start + 2, 0, "%c, %d %s and %d %s",
                    monster->symbol, abs(y_dist), y_dir, abs(x_dist), x_dir);
        }
        index++;
      }
    }
  }

  refresh();




}

uint32_t pc_next_pos_input(dungeon_t *d, pair_t dir)
{
  dir[dim_y] = dir[dim_x] = 0;

  int ch, exit_flag;
  exit_flag = 0;

  // waits until a valid key is pressed
  while (1) {  
    ch = getch();

    move(0, 0);
    clrtoeol();  // Clears first line 
    refresh();

    switch (ch) {
      case '7': case 'y': dir[dim_y] = -1; dir[dim_x] = -1; return 0;   // Upper left
      case '8': case 'k': dir[dim_y] = -1; return 0;                    // Up
      case '9': case 'u': dir[dim_y] = -1; dir[dim_x] = 1; return 0;    // Upper right
      case '6': case 'l': dir[dim_x] = 1; return 0;                     // Right
      case '3': case 'n': dir[dim_y] = 1; dir[dim_x] = 1; return 0;     // Lower right
      case '2': case 'j': dir[dim_y] = 1; return 0;                     // Down
      case '1': case 'b': dir[dim_y] = 1; dir[dim_x] = -1; return 0;    // Lower left
      case '4': case 'h': dir[dim_x] = -1; return 0;                    // Left
      case '>': 
        mvprintw(0, 0, "Traveled down stairs!");
        refresh();
        return 1;  // Attempt to go down stairs
      case '<': 
        mvprintw(0, 0, "Traveled up stairs!");
        refresh();
        return 1;  // Attempt to go up stairs
      case '5': case ' ': case '.': return 0;  // Rest for a turn
      case 'm': 
        // Show monster list
        exit_flag = display_monster_list(d); 
        if (exit_flag){
          return 255; // Quit game
        }
        break;  
      case 'Q': return 255;  // Quit game
      default: 
        mvprintw(0, 0, "Invalid input!");
        refresh();
        break;  // Keep waiting for valid input
    }
  }

}

uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->pc.position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc.position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->pc.position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc.position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}
