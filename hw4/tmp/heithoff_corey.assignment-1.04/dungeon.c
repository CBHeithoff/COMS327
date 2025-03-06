#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "heap.h"
#include "dungeon.h"
#include "utils.h"
#include "path.h"

#define DUMP_HARDNESS_IMAGES 0

typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

/*
static uint32_t in_room(dungeon_t *d, int16_t y, int16_t x)
{
  int i;

  for (i = 0; i < d->num_rooms; i++) {
    if ((x >= d->rooms[i].position[dim_x]) &&
        (x < (d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x])) &&
        (y >= d->rooms[i].position[dim_y]) &&
        (y < (d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y]))) {
      return 1;
    }
  }

  return 0;
}
*/

static uint32_t adjacent_to_room(dungeon_t *d, int16_t y, int16_t x)
{
  return (mapxy(x - 1, y) == ter_floor_room ||
          mapxy(x + 1, y) == ter_floor_room ||
          mapxy(x, y - 1) == ter_floor_room ||
          mapxy(x, y + 1) == ter_floor_room);
}

static uint32_t is_open_space(dungeon_t *d, int16_t y, int16_t x)
{
  return !hardnessxy(x, y);
}

static int32_t corridor_path_cmp(const void *key, const void *with) {
  return ((corridor_path_t *) key)->cost - ((corridor_path_t *) with)->cost;
}

static void dijkstra_corridor(dungeon_t *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room) {
          mapxy(x, y) = ter_floor_hall;
          hardnessxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

/* This is a cut-and-paste of the above.  The code is modified to  *
 * calculate paths based on inverse hardnesses so that we get a    *
 * high probability of creating at least one cycle in the dungeon. */
static void dijkstra_corridor_inv(dungeon_t *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room) {
          mapxy(x, y) = ter_floor_hall;
          hardnessxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

#define hardnesspair_inv(p) (is_open_space(d, p[dim_y], p[dim_x]) ? 127 :     \
                             (adjacent_to_room(d, p[dim_y], p[dim_x]) ? 191 : \
                              (255 - hardnesspair(p))))

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

/* Chooses a random point inside each room and connects them with a *
 * corridor.  Random internal points prevent corridors from exiting *
 * rooms in predictable locations.                                  */
static int connect_two_rooms(dungeon_t *d, room_t *r1, room_t *r2)
{
  pair_t e1, e2;

  e1[dim_y] = rand_range(r1->position[dim_y],
                         r1->position[dim_y] + r1->size[dim_y] - 1);
  e1[dim_x] = rand_range(r1->position[dim_x],
                         r1->position[dim_x] + r1->size[dim_x] - 1);
  e2[dim_y] = rand_range(r2->position[dim_y],
                         r2->position[dim_y] + r2->size[dim_y] - 1);
  e2[dim_x] = rand_range(r2->position[dim_x],
                         r2->position[dim_x] + r2->size[dim_x] - 1);

  /*  return connect_two_points_recursive(d, e1, e2);*/
  dijkstra_corridor(d, e1, e2);

  return 0;
}

static int create_cycle(dungeon_t *d)
{
  /* Find the (approximately) farthest two rooms, then connect *
   * them by the shortest path using inverted hardnesses.      */

  int32_t max, tmp, i, j, p, q;
  pair_t e1, e2;

  for (i = max = 0; i < d->num_rooms - 1; i++) {
    for (j = i + 1; j < d->num_rooms; j++) {
      tmp = (((d->rooms[i].position[dim_x] - d->rooms[j].position[dim_x])  *
              (d->rooms[i].position[dim_x] - d->rooms[j].position[dim_x])) +
             ((d->rooms[i].position[dim_y] - d->rooms[j].position[dim_y])  *
              (d->rooms[i].position[dim_y] - d->rooms[j].position[dim_y])));
      if (tmp > max) {
        max = tmp;
        p = i;
        q = j;
      }
    }
  }

  /* Can't simply call connect_two_rooms() because it doesn't *
   * use inverse hardnesses, so duplicate it here.            */
  e1[dim_y] = rand_range(d->rooms[p].position[dim_y],
                         (d->rooms[p].position[dim_y] +
                          d->rooms[p].size[dim_y] - 1));
  e1[dim_x] = rand_range(d->rooms[p].position[dim_x],
                         (d->rooms[p].position[dim_x] +
                          d->rooms[p].size[dim_x] - 1));
  e2[dim_y] = rand_range(d->rooms[q].position[dim_y],
                         (d->rooms[q].position[dim_y] +
                          d->rooms[q].size[dim_y] - 1));
  e2[dim_x] = rand_range(d->rooms[q].position[dim_x],
                         (d->rooms[q].position[dim_x] +
                          d->rooms[q].size[dim_x] - 1));

  dijkstra_corridor_inv(d, e1, e2);

  return 0;
}

static int connect_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = 1; i < d->num_rooms; i++) {
    connect_two_rooms(d, d->rooms + i - 1, d->rooms + i);
  }

  create_cycle(d);

  return 0;
}

int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

static int smooth_hardness(dungeon_t *d)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
#if DUMP_HARDNESS_IMAGES
  FILE *out;
#endif
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];

  memset(&hardness, 0, sizeof (hardness));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % DUNGEON_X;
      y = rand() % DUNGEON_Y;
    } while (hardness[y][x]);
    hardness[y][x] = i;
    if (i == 1) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

#if DUMP_HARDNESS_IMAGES
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&hardness, sizeof (hardness), 1, out);
  fclose(out);
#endif
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = hardness[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !hardness[y - 1][x - 1]) {
      hardness[y - 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !hardness[y][x - 1]) {
      hardness[y][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < DUNGEON_Y && !hardness[y + 1][x - 1]) {
      hardness[y + 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !hardness[y - 1][x]) {
      hardness[y - 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < DUNGEON_Y && !hardness[y + 1][x]) {
      hardness[y + 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < DUNGEON_X && y - 1 >= 0 && !hardness[y - 1][x + 1]) {
      hardness[y - 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < DUNGEON_X && !hardness[y][x + 1]) {
      hardness[y][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < DUNGEON_X && y + 1 < DUNGEON_Y && !hardness[y + 1][x + 1]) {
      hardness[y + 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_X) {
            s += gaussian[p][q];
            t += hardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      d->hardness[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_X) {
            s += gaussian[p][q];
            t += hardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      d->hardness[y][x] = t / s;
    }
  }

#if DUMP_HARDNESS_IMAGES
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&hardness, sizeof (hardness), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&d->hardness, sizeof (d->hardness), 1, out);
  fclose(out);
#endif

  return 0;
}

static int empty_dungeon(dungeon_t *d)
{
  uint8_t x, y;

  smooth_hardness(d);
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      mapxy(x, y) = ter_wall;
      if (y == 0 || y == DUNGEON_Y - 1 ||
          x == 0 || x == DUNGEON_X - 1) {
        mapxy(x, y) = ter_wall_immutable;
        hardnessxy(x, y) = 255;
      }
    }
  }

  return 0;
}

static int place_rooms(dungeon_t *d)
{
  pair_t p;
  uint32_t i;
  int success;
  room_t *r;

  for (success = 0; !success; ) {
    success = 1;
    for (i = 0; success && i < d->num_rooms; i++) {
      r = d->rooms + i;
      r->position[dim_x] = 1 + rand() % (DUNGEON_X - 2 - r->size[dim_x]);
      r->position[dim_y] = 1 + rand() % (DUNGEON_Y - 2 - r->size[dim_y]);
      for (p[dim_y] = r->position[dim_y] - 1;
           success && p[dim_y] < r->position[dim_y] + r->size[dim_y] + 1;
           p[dim_y]++) {
        for (p[dim_x] = r->position[dim_x] - 1;
             success && p[dim_x] < r->position[dim_x] + r->size[dim_x] + 1;
             p[dim_x]++) {
          if (mappair(p) >= ter_floor) {
            success = 0;
            empty_dungeon(d);
          } else if ((p[dim_y] != r->position[dim_y] - 1)              &&
                     (p[dim_y] != r->position[dim_y] + r->size[dim_y]) &&
                     (p[dim_x] != r->position[dim_x] - 1)              &&
                     (p[dim_x] != r->position[dim_x] + r->size[dim_x])) {
            mappair(p) = ter_floor_room;
            hardnesspair(p) = 0;
          }
        }
      }
    }
  }

  return 0;
}

static void place_stairs(dungeon_t *d)
{
  pair_t p;
  do {
    while ((p[dim_y] = rand_range(1, DUNGEON_Y - 2)) &&
           (p[dim_x] = rand_range(1, DUNGEON_X - 2)) &&
           ((mappair(p) < ter_floor)                 ||
            (mappair(p) > ter_stairs)))
      ;
    mappair(p) = ter_stairs_down;
  } while (rand_under(1, 3));
  do {
    while ((p[dim_y] = rand_range(1, DUNGEON_Y - 2)) &&
           (p[dim_x] = rand_range(1, DUNGEON_X - 2)) &&
           ((mappair(p) < ter_floor)                 ||
            (mappair(p) > ter_stairs)))
      
      ;
    mappair(p) = ter_stairs_up;
  } while (rand_under(2, 4));
}

static int make_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = MIN_ROOMS; i < MAX_ROOMS && rand_under(5, 8); i++)
    ;
  d->num_rooms = i;
  d->rooms = malloc(sizeof (*d->rooms) * d->num_rooms);
  
  for (i = 0; i < d->num_rooms; i++) {
    d->rooms[i].size[dim_x] = ROOM_MIN_X;
    d->rooms[i].size[dim_y] = ROOM_MIN_Y;
    while (rand_under(3, 5) && d->rooms[i].size[dim_x] < ROOM_MAX_X) {
      d->rooms[i].size[dim_x]++;
    }
    while (rand_under(3, 5) && d->rooms[i].size[dim_y] < ROOM_MAX_Y) {
      d->rooms[i].size[dim_y]++;
    }
  }

  return 0;
}

int gen_dungeon(dungeon_t *d)
{
  empty_dungeon(d);

  do {
    make_rooms(d);
  } while (place_rooms(d));
  connect_rooms(d);
  place_stairs(d);

  return 0;
}

void render_dungeon(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (d->pc.position[dim_x] == p[dim_x] && d->pc.position[dim_y] == p[dim_y]) {
        putchar('@');
      } else {
        switch (mappair(p)) {
        case ter_wall:
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_floor:
        case ter_floor_room:
          putchar('.');
          break;
        case ter_floor_hall:
          putchar('#');
          break;
        case ter_debug:
          putchar('*');
          fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
          break;
        case ter_stairs_up:
          putchar('<');
          break;
        case ter_stairs_down:
          putchar('>');
          break;
        default:
          break;
        }
      }
    }
    putchar('\n');
  }
}

void delete_dungeon(dungeon_t *d)
{
  int y, x;

  // freeing all monsters created in the character table
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
        if (d->characters[y][x] != NULL) {
            free(d->characters[y][x]);  
        }
        d->characters[y][x] = NULL;  
    }
  }

  free(d->rooms);
}

void init_dungeon(dungeon_t *d)
{
  empty_dungeon(d);
}

int write_dungeon_map(dungeon_t *d, FILE *f)
{
  uint32_t x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      fwrite(&d->hardness[y][x], sizeof (unsigned char), 1, f);
    }
  }

  return 0;
}

int write_rooms(dungeon_t *d, FILE *f)
{
  uint32_t i;
  uint16_t p;

  p = htobe16(d->num_rooms);
  fwrite(&p, 2, 1, f);
  for (i = 0; i < d->num_rooms; i++) {
    /* write order is xpos, ypos, width, height */
    p = d->rooms[i].position[dim_x];
    fwrite(&p, 1, 1, f);
    p = d->rooms[i].position[dim_y];
    fwrite(&p, 1, 1, f);
    p = d->rooms[i].size[dim_x];
    fwrite(&p, 1, 1, f);
    p = d->rooms[i].size[dim_y];
    fwrite(&p, 1, 1, f);
  }

  return 0;
}

uint16_t count_up_stairs(dungeon_t *d)
{
  uint32_t x, y;
  uint16_t i;

  for (i = 0, y = 1; y < DUNGEON_Y - 1; y++) {
    for (x = 1; x < DUNGEON_X - 1; x++) {
      if (mapxy(x, y) == ter_stairs_up) {
        i++;
      }
    }
  }

  return i;
}

uint16_t count_down_stairs(dungeon_t *d)
{
  uint32_t x, y;
  uint16_t i;

  for (i = 0, y = 1; y < DUNGEON_Y - 1; y++) {
    for (x = 1; x < DUNGEON_X - 1; x++) {
      if (mapxy(x, y) == ter_stairs_down) {
        i++;
      }
    }
  }

  return i;
}

int write_stairs(dungeon_t *d, FILE *f)
{
  uint16_t num_stairs;
  uint8_t x, y;

  num_stairs = htobe16(count_up_stairs(d));
  fwrite(&num_stairs, 2, 1, f);
  for (y = 1; y < DUNGEON_Y - 1 && num_stairs; y++) {
    for (x = 1; x < DUNGEON_X - 1 && num_stairs; x++) {
      if (mapxy(x, y) == ter_stairs_up) {
        num_stairs--;
        fwrite(&x, 1, 1, f);
        fwrite(&y, 1, 1, f);
      }
    }
  }

  num_stairs = htobe16(count_down_stairs(d));
  fwrite(&num_stairs, 2, 1, f);
  for (y = 1; y < DUNGEON_Y - 1 && num_stairs; y++) {
    for (x = 1; x < DUNGEON_X - 1 && num_stairs; x++) {
      if (mapxy(x, y) == ter_stairs_down) {
        num_stairs--;
        fwrite(&x, 1, 1, f);
        fwrite(&y, 1, 1, f);
      }
    }
  }

  return 0;
}

uint32_t calculate_dungeon_size(dungeon_t *d)
{
  /* Per the spec, 1708 is 12 byte semantic marker + 4 byte file verion + *
   * 4 byte file size + 2 byte PC position + 1680 byte hardness array +   *
   * 2 byte each number of rooms, number of up stairs, number of down     *
   * stairs.                                                              */
  return (1708 + (d->num_rooms * 4) +
          (count_up_stairs(d) * 2)  +
          (count_down_stairs(d) * 2));
}

int write_dungeon(dungeon_t *d, char *file)
{
  char *home;
  char *filename;
  FILE *f;
  size_t len;
  uint32_t be32;

  if (!file) {
    if (!(home = getenv("HOME"))) {
      fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
      home = ".";
    }

    len = (strlen(home) + strlen(SAVE_DIR) + strlen(DUNGEON_SAVE_FILE) +
           1 /* The NULL terminator */                                 +
           2 /* The slashes */);

    filename = malloc(len * sizeof (*filename));
    sprintf(filename, "%s/%s/", home, SAVE_DIR);
    makedirectory(filename);
    strcat(filename, DUNGEON_SAVE_FILE);

    if (!(f = fopen(filename, "w"))) {
      perror(filename);
      free(filename);

      return 1;
    }
    free(filename);
  } else {
    if (!(f = fopen(file, "w"))) {
      perror(file);
      exit(-1);
    }
  }

  /* The semantic, which is 6 bytes, 0-11 */
  fwrite(DUNGEON_SAVE_SEMANTIC, 1, sizeof (DUNGEON_SAVE_SEMANTIC) - 1, f);

  /* The version, 4 bytes, 12-15 */
  be32 = htobe32(DUNGEON_SAVE_VERSION);
  fwrite(&be32, sizeof (be32), 1, f);

  /* The size of the file, 4 bytes, 16-19 */
  be32 = htobe32(calculate_dungeon_size(d));
  fwrite(&be32, sizeof (be32), 1, f);

  /* The PC position, 2 bytes, 20-21 */
  fwrite(&d->pc.position[dim_x], 1, 1, f);
  fwrite(&d->pc.position[dim_y], 1, 1, f);

  /* The dungeon map, 1680 bytes, 22-1702 */
  write_dungeon_map(d, f);

  /* The rooms, num_rooms * 4 bytes, 1703-end */
  write_rooms(d, f);

  /* And the stairs */
  write_stairs(d, f);

  fclose(f);

  return 0;
}

int read_dungeon_map(dungeon_t *d, FILE *f)
{
  uint32_t x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      fread(&d->hardness[y][x], sizeof (d->hardness[y][x]), 1, f);
      if (d->hardness[y][x] == 0) {
        /* Mark it as a corridor.  We can't recognize room cells until *
         * after we've read the room array, which we haven't done yet. */
        d->map[y][x] = ter_floor_hall;
      } else if (d->hardness[y][x] == 255) {
        d->map[y][x] = ter_wall_immutable;
      } else {
        d->map[y][x] = ter_wall;
      }
    }
  }


  return 0;
}

int read_stairs(dungeon_t *d, FILE *f)
{
  uint16_t num_stairs;
  uint8_t x, y;

  fread(&num_stairs, 2, 1, f);
  num_stairs = be16toh(num_stairs);
  for (; num_stairs; num_stairs--) {
    fread(&x, 1, 1, f);
    fread(&y, 1, 1, f);
    mapxy(x, y) = ter_stairs_up;
  }

  fread(&num_stairs, 2, 1, f);
  num_stairs = be16toh(num_stairs);
  for (; num_stairs; num_stairs--) {
    fread(&x, 1, 1, f);
    fread(&y, 1, 1, f);
    mapxy(x, y) = ter_stairs_down;
  }
  return 0;
}

int read_rooms(dungeon_t *d, FILE *f)
{
  uint32_t i;
  uint32_t x, y;
  uint16_t p;

  fread(&p, 2, 1, f);
  d->num_rooms = be16toh(p);
  d->rooms = malloc(sizeof (*d->rooms) * d->num_rooms);

  for (i = 0; i < d->num_rooms; i++) {
    fread(&p, 1, 1, f);
    d->rooms[i].position[dim_x] = p;
    fread(&p, 1, 1, f);
    d->rooms[i].position[dim_y] = p;
    fread(&p, 1, 1, f);
    d->rooms[i].size[dim_x] = p;
    fread(&p, 1, 1, f);
    d->rooms[i].size[dim_y] = p;

    if (d->rooms[i].size[dim_x] < 1             ||
        d->rooms[i].size[dim_y] < 1             ||
        d->rooms[i].size[dim_x] > DUNGEON_X - 1 ||
        d->rooms[i].size[dim_y] > DUNGEON_X - 1) {
      fprintf(stderr, "Invalid room size in restored dungeon.\n");

      exit(-1);
    }

    if (d->rooms[i].position[dim_x] < 1                                       ||
        d->rooms[i].position[dim_y] < 1                                       ||
        d->rooms[i].position[dim_x] > DUNGEON_X - 1                           ||
        d->rooms[i].position[dim_y] > DUNGEON_Y - 1                           ||
        d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x] > DUNGEON_X - 1 ||
        d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x] < 0             ||
        d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y] > DUNGEON_Y - 1 ||
        d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y] < 0)             {
      fprintf(stderr, "Invalid room position in restored dungeon.\n");

      exit(-1);
    }
        

    /* After reading each room, we need to reconstruct them in the dungeon. */
    for (y = d->rooms[i].position[dim_y];
         y < d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y];
         y++) {
      for (x = d->rooms[i].position[dim_x];
           x < d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x];
           x++) {
        mapxy(x, y) = ter_floor_room;
      }
    }
  }

  return 0;
}

int read_dungeon(dungeon_t *d, char *file)
{
  char semantic[sizeof (DUNGEON_SAVE_SEMANTIC)];
  uint32_t be32;
  FILE *f;
  char *home;
  size_t len;
  char *filename;
  struct stat buf;

  if (!file) {
    if (!(home = getenv("HOME"))) {
      fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
      home = ".";
    }

    len = (strlen(home) + strlen(SAVE_DIR) + strlen(DUNGEON_SAVE_FILE) +
           1 /* The NULL terminator */                                 +
           2 /* The slashes */);

    filename = malloc(len * sizeof (*filename));
    sprintf(filename, "%s/%s/%s", home, SAVE_DIR, DUNGEON_SAVE_FILE);

    if (!(f = fopen(filename, "r"))) {
      perror(filename);
      free(filename);
      exit(-1);
    }

    if (stat(filename, &buf)) {
      perror(filename);
      exit(-1);
    }

    free(filename);
  } else {
    if (!(f = fopen(file, "r"))) {
      perror(file);
      exit(-1);
    }
    if (stat(file, &buf)) {
      perror(file);
      exit(-1);
    }
  }

  d->num_rooms = 0;

  fread(semantic, sizeof (DUNGEON_SAVE_SEMANTIC) - 1, 1, f);
  semantic[sizeof (DUNGEON_SAVE_SEMANTIC) - 1] = '\0';
  if (strncmp(semantic, DUNGEON_SAVE_SEMANTIC,
	      sizeof (DUNGEON_SAVE_SEMANTIC) - 1)) {
    fprintf(stderr, "Not an RLG327 save file.\n");
    exit(-1);
  }
  fread(&be32, sizeof (be32), 1, f);
  if (be32toh(be32) != 0) { /* Since we expect zero, be32toh() is a no-op. */
    fprintf(stderr, "File version mismatch.\n");
    exit(-1);
  }
  fread(&be32, sizeof (be32), 1, f);
  if (buf.st_size != be32toh(be32)) {
    fprintf(stderr, "File size mismatch.\n");
    exit(-1);
  }

  fread(&d->pc.position[dim_x], 1, 1, f);
  fread(&d->pc.position[dim_y], 1, 1, f);
  
  read_dungeon_map(d, f);

  read_rooms(d, f);

  read_stairs(d, f);

  fclose(f);

  return 0;
}

/* PGM dungeon descriptions do not support PC or stairs */
int read_pgm(dungeon_t *d, char *pgm)
{
  FILE *f;
  char s[80];
  uint8_t gm[DUNGEON_Y - 2][DUNGEON_X - 2];
  uint32_t x, y;
  uint32_t i;
  char size[8]; /* Big enough to hold two 3-digit values with a space between. */

  if (!(f = fopen(pgm, "r"))) {
    perror(pgm);
    exit(-1);
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P5\n");
    exit(-1);
  }
  if (!fgets(s, 80, f) || s[0] != '#') {
    fprintf(stderr, "Expected comment\n");
    exit(-1);
  }
  snprintf(size, 8, "%d %d", DUNGEON_X - 2, DUNGEON_Y - 2);
  if (!fgets(s, 80, f) || strncmp(s, size, 5)) {
    fprintf(stderr, "Expected %s\n", size);
    exit(-1);
  }
  if (!fgets(s, 80, f) || strncmp(s, "255", 2)) {
    fprintf(stderr, "Expected 255\n");
    exit(-1);
  }

  fread(gm, 1, (DUNGEON_X - 2) * (DUNGEON_Y - 2), f);

  fclose(f);

  /* In our gray map, treat black (0) as corridor, white (255) as room, *
   * all other values as a hardness.  For simplicity, treat every white *
   * cell as its own room, so we have to count white after reading the  *
   * image in order to allocate the room array.                         */
  for (d->num_rooms = 0, y = 0; y < DUNGEON_Y - 2; y++) {
    for (x = 0; x < DUNGEON_X - 2; x++) {
      if (!gm[y][x]) {
        d->num_rooms++;
      }
    }
  }
  d->rooms = malloc(sizeof (*d->rooms) * d->num_rooms);

  for (i = 0, y = 0; y < DUNGEON_Y - 2; y++) {
    for (x = 0; x < DUNGEON_X - 2; x++) {
      if (!gm[y][x]) {
        d->rooms[i].position[dim_x] = x + 1;
        d->rooms[i].position[dim_y] = y + 1;
        d->rooms[i].size[dim_x] = 1;
        d->rooms[i].size[dim_y] = 1;
        i++;
        d->map[y + 1][x + 1] = ter_floor_room;
        d->hardness[y + 1][x + 1] = 0;
      } else if (gm[y][x] == 255) {
        d->map[y + 1][x + 1] = ter_floor_hall;
        d->hardness[y + 1][x + 1] = 0;
      } else {
        d->map[y + 1][x + 1] = ter_wall;
        d->hardness[y + 1][x + 1] = gm[y][x];
      }
    }
  }

  for (x = 0; x < DUNGEON_X; x++) {
    d->map[0][x] = ter_wall_immutable;
    d->hardness[0][x] = 255;
    d->map[DUNGEON_Y - 1][x] = ter_wall_immutable;
    d->hardness[DUNGEON_Y - 1][x] = 255;
  }
  for (y = 1; y < DUNGEON_Y - 1; y++) {
    d->map[y][0] = ter_wall_immutable;
    d->hardness[y][0] = 255;
    d->map[y][DUNGEON_X - 1] = ter_wall_immutable;
    d->hardness[y][DUNGEON_X - 1] = 255;
  }

  return 0;
}

void render_hardness_map(dungeon_t *d)
{
  /* The hardness map includes coordinates, since it's larger *
   * size makes it more difficult to index a position by eye. */
  
  pair_t p;
  int i;
  
  putchar('\n');
  printf("   ");
  for (i = 0; i < DUNGEON_X; i++) {
    printf("%2d", i);
  }
  putchar('\n');
  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    printf("%2d ", p[dim_y]);
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      printf("%02x", hardnesspair(p));
    }
    putchar('\n');
  }
}

void render_movement_cost_map(dungeon_t *d)
{
  pair_t p;

  putchar('\n');
  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (p[dim_x] ==  d->pc.position[dim_x] &&
          p[dim_y] ==  d->pc.position[dim_y]) {
        putchar('@');
      } else {
        if (hardnesspair(p) == 255) {
          printf("X");
        } else {
          printf("%d", (hardnesspair(p) / 85) + 1);
        }
      }
    }
    putchar('\n');
  }
}

void render_distance_map(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (p[dim_x] ==  d->pc.position[dim_x] &&
          p[dim_y] ==  d->pc.position[dim_y]) {
        putchar('@');
      } else {
        switch (mappair(p)) {
        case ter_wall:
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_floor:
        case ter_floor_room:
        case ter_floor_hall:
        case ter_stairs:
        case ter_stairs_up:
        case ter_stairs_down:
          /* Placing X for infinity */
          if (d->pc_distance[p[dim_y]][p[dim_x]] == UCHAR_MAX) {
            putchar('X');
          } else {
            putchar('0' + d->pc_distance[p[dim_y]][p[dim_x]] % 10);
          }
          break;
        case ter_debug:
          fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
          putchar('*');
          break;
        }
      }
    }
    putchar('\n');
  }
}

void render_tunnel_distance_map(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (p[dim_x] ==  d->pc.position[dim_x] &&
          p[dim_y] ==  d->pc.position[dim_y]) {
        putchar('@');
      } else {
        switch (mappair(p)) {
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_wall:
        case ter_floor:
        case ter_floor_room:
        case ter_floor_hall:
        case ter_stairs:
        case ter_stairs_up:
        case ter_stairs_down:
          /* Placing X for infinity */
          if (d->pc_tunnel[p[dim_y]][p[dim_x]] == UCHAR_MAX) {
            putchar('X');
          } else {
            putchar('0' + d->pc_tunnel[p[dim_y]][p[dim_x]] % 10);
          }
          break;
        case ter_debug:
          fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
          putchar('*');
          break;
        }
      }
    }
    putchar('\n');
  }
}

// ------------------ New functions below this comment -----------------------

int gen_characters(dungeon_t *d)
{
  uint32_t i;
  monster_t *tmp;
  pair_t p;

  // initializing the PC
  d->pc.sequence_num = 0;
  d->pc.next_turn = 0;
  d->pc.speed = PC_SPEED;
  
  // initializing the character table
  for (int y = 0; y < DUNGEON_Y; y++) {
    for (int x = 0; x < DUNGEON_X; x++) {
        d->characters[y][x] = NULL;  
    }
  }

  // initializing the monsters
  for (i = 1; i <= d->num_monsters; i++) {
    // allocating space
    if(!(tmp = malloc(sizeof (*tmp)))){
      return -1;
    }

    // finding a proper position for monsters
    while ((p[dim_y] = rand_range(1, DUNGEON_Y - 2)) &&
           (p[dim_x] = rand_range(1, DUNGEON_X - 2)) &&
           ((mappair(p) < ter_floor)                 ||
            (mappair(p) > ter_stairs)                ||
            (mappair(p) == ter_floor_hall)           ||
            (p[dim_y] == d->pc.position[dim_y] && p[dim_x] == d->pc.position[dim_x])))
      ;
    
    // if a monster spawns on top of another monster, the orginial monster is killed
    if (d->characters[p[dim_y]][p[dim_x]] != NULL) {
      ((monster_t*)d->characters[p[dim_y]][p[dim_x]])->alive = 0;
      free(d->characters[p[dim_y]][p[dim_x]]);
      d->characters[p[dim_y]][p[dim_x]] = NULL;
      d->num_monsters--;
    }

    // giving the monster its characteristics and info
    int attr = rand_range(0, 15);
    tmp->speed = rand_range(MIN_MON_SPEED, MAX_MON_SPEED);
    tmp->characteristics = attr;
    tmp->sequence_num = i;
    tmp->symbol = (attr < 10) ? (attr + '0') : (attr - 10 + 'a');
    tmp->next_turn = 0;
    tmp->alive = 1;
    // by default a monster is given its own position for the PC position 
    tmp->pc_position[dim_y] = p[dim_y];
    tmp->pc_position[dim_x] = p[dim_x];
    tmp->position[dim_y] = p[dim_y];
    tmp->position[dim_x] = p[dim_x];

    d->characters[p[dim_y]][p[dim_x]] = tmp;

    // DEBUGGING

    // if((monster_is(tmp, TELEPATHIC))){
    //   printf("it is a TELEPATHIC\n");
    // }
    // if((monster_is(tmp, TUNNEL))){
    //   printf("it is a TUNNEL\n");
    // }
    // if((monster_is(tmp, SMART))){
    //   printf("it is a SMART\n");
    // }
    // if((monster_is(tmp, ERRATIC))){
    //   printf("it is a ERRATIC\n");
    // }

  }
  return 0;
}

void render_dungeon_mon(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (d->characters[p[dim_y]][p[dim_x]] != NULL){
        putchar(((monster_t*)d->characters[p[dim_y]][p[dim_x]])->symbol);
      } else if (d->pc.position[dim_x] == p[dim_x] && d->pc.position[dim_y] == p[dim_y]) {
        putchar('@');
      } else {
        switch (mappair(p)) {
        case ter_wall:
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_floor:
        case ter_floor_room:
          putchar('.');
          break;
        case ter_floor_hall:
          putchar('#');
          break;
        case ter_debug:
          putchar('*');
          fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
          break;
        case ter_stairs_up:
          putchar('<');
          break;
        case ter_stairs_down:
          putchar('>');
          break;
        default:
          break;
        }
      }
    }
    putchar('\n');
  }
}

static int in_room(dungeon_t *d, pair_t pos)
{
  int i;
  
  for (i = 0; i < d->num_rooms; i++) {
    if(pos[dim_y] >= d->rooms[i].position[dim_y] && 
       pos[dim_y] < d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y] && 
       pos[dim_x] >= d->rooms[i].position[dim_x] && 
       pos[dim_x] < d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x]){
        return i + 1;
    } 
  }
  return 0;
}

static void bresendam_next_step(pair_t from, pair_t to, pair_t *next)
{
  // return if from == to
  if (to[dim_x] == from[dim_x] && to[dim_y] == from[dim_y]){
    (*next)[dim_x] = from[dim_x];
    (*next)[dim_y] = from[dim_y];
    return;
  }
  
  // determining direction of travel
  int dx = (to[dim_x] > from[dim_x]) ? (to[dim_x] - from[dim_x]) : (from[dim_x] - to[dim_x]);
  int dy = (to[dim_y] > from[dim_y]) ? (to[dim_y] - from[dim_y]) : (from[dim_y] - to[dim_y]);
  int sx = (from[dim_x] < to[dim_x]) ? 1 : -1;
  int sy = (from[dim_y] < to[dim_y]) ? 1 : -1;

  // determining position of next move
  if (dx > dy) {
    (*next)[dim_x] = from[dim_x] + sx;
    (*next)[dim_y] = from[dim_y];
  } else if (dy > dx) {
    (*next)[dim_x] = from[dim_x];
    (*next)[dim_y] = from[dim_y] + sy;
  } else {
    (*next)[dim_x] = from[dim_x] + sx;
    (*next)[dim_y] = from[dim_y] + sy;
  }
}

static void smart_next_step(dungeon_t *d, pair_t from, pair_t to, pair_t *next)
{
  // return if from == to
  if (to[dim_x] == from[dim_x] && to[dim_y] == from[dim_y]){
    (*next)[dim_x] = from[dim_x];
    (*next)[dim_y] = from[dim_y];
    return;
  }

  // Runs the dijkstra algorithm based on where the monster thinks/knows the PC is
  // Creating tmp coordinate for PC actual position
  pair_t PC_tmp;
  PC_tmp[dim_y] = d->pc.position[dim_y];
  PC_tmp[dim_x] = d->pc.position[dim_x];

  d->pc.position[dim_y] = to[dim_y];
  d->pc.position[dim_x] = to[dim_x];

  dijkstra(d);

  d->pc.position[dim_y] = PC_tmp[dim_y];
  d->pc.position[dim_x] = PC_tmp[dim_x];
  
  // Intelligent movement based on distance map
  uint8_t min = d->pc_distance[from[dim_y]   ][from[dim_x]   ];
  (*next)[dim_y] = from[dim_y];
  (*next)[dim_x] = from[dim_x];
  // checks surrounding cells for best next move
  // right
  if(min > d->pc_distance[from[dim_y]    ][from[dim_x] +1 ]){
    min = d->pc_distance[from[dim_y]    ][from[dim_x] +1 ];
    (*next)[dim_y] += 0;
    (*next)[dim_x] += 1;
  }
  // left
  if(min > d->pc_distance[from[dim_y]    ][from[dim_x] -1 ]){
    min = d->pc_distance[from[dim_y]    ][from[dim_x] -1 ];
    (*next)[dim_y] += 0;
    (*next)[dim_x] -= 1;
  }
  // top
  if(min > d->pc_distance[from[dim_y] -1 ][from[dim_x]    ]){
    min = d->pc_distance[from[dim_y] -1 ][from[dim_x]    ];
    (*next)[dim_y] -= 1;
    (*next)[dim_x] += 0;
  }
  // down
  if(min > d->pc_distance[from[dim_y] +1 ][from[dim_x]    ]){
    min = d->pc_distance[from[dim_y] +1 ][from[dim_x]    ];
    (*next)[dim_y] += 1;
    (*next)[dim_x] += 0;
  }
  // up right
  if(min > d->pc_distance[from[dim_y] -1 ][from[dim_x] +1 ]){
    min = d->pc_distance[from[dim_y] -1 ][from[dim_x] +1 ];
    (*next)[dim_y] -= 1;
    (*next)[dim_x] += 1;
  }
  // down left
  if(min > d->pc_distance[from[dim_y] +1 ][from[dim_x] -1 ]){
    min = d->pc_distance[from[dim_y] +1 ][from[dim_x] -1 ];
    (*next)[dim_y] += 1;
    (*next)[dim_x] -= 1;
  }
  // down right
  if(min > d->pc_distance[from[dim_y] +1 ][from[dim_x] +1 ]){
    min = d->pc_distance[from[dim_y] +1 ][from[dim_x] +1 ];
    (*next)[dim_y] += 1;
    (*next)[dim_x] += 1;
  }
  // up left
  if(min > d->pc_distance[from[dim_y] -1 ][from[dim_x] -1 ]){
    min = d->pc_distance[from[dim_y] -1 ][from[dim_x] -1 ];
    (*next)[dim_y] -= 1;
    (*next)[dim_x] -= 1;
  }

}

static void smart_next_step_tunnel(dungeon_t *d, pair_t from, pair_t to, pair_t *next)
{
  // return if from == to
  if (to[dim_x] == from[dim_x] && to[dim_y] == from[dim_y]){
    (*next)[dim_x] = from[dim_x];
    (*next)[dim_y] = from[dim_y];
    return;
  }

  // Runs the dijkstra algorithm based on where the monster thinks/knows the PC is
  // Creating tmp coordinate for PC actual position
  pair_t PC_tmp;
  PC_tmp[dim_y] = d->pc.position[dim_y];
  PC_tmp[dim_x] = d->pc.position[dim_x];

  d->pc.position[dim_y] = to[dim_y];
  d->pc.position[dim_x] = to[dim_x];

  dijkstra_tunnel(d);

  d->pc.position[dim_y] = PC_tmp[dim_y];
  d->pc.position[dim_x] = PC_tmp[dim_x];

  // Intelligent movement based on distance map
  uint8_t min = d->pc_tunnel[from[dim_y]   ][from[dim_x]   ];
  (*next)[dim_y] = from[dim_y];
  (*next)[dim_x] = from[dim_x];
  // checks surrounding cells for best next move
  // right
  if(min > d->pc_tunnel[from[dim_y]    ][from[dim_x] +1 ]){
    min = d->pc_tunnel[from[dim_y]    ][from[dim_x] +1 ];
    (*next)[dim_y] += 0;
    (*next)[dim_x] += 1;
  }
  // left
  if(min > d->pc_tunnel[from[dim_y]    ][from[dim_x] -1 ]){
    min = d->pc_tunnel[from[dim_y]    ][from[dim_x] -1 ];
    (*next)[dim_y] += 0;
    (*next)[dim_x] -= 1;
  }
  // top
  if(min > d->pc_tunnel[from[dim_y] -1 ][from[dim_x]    ]){
    min = d->pc_tunnel[from[dim_y] -1 ][from[dim_x]    ];
    (*next)[dim_y] -= 1;
    (*next)[dim_x] += 0;
  }
  // down
  if(min > d->pc_tunnel[from[dim_y] +1 ][from[dim_x]    ]){
    min = d->pc_tunnel[from[dim_y] +1 ][from[dim_x]    ];
    (*next)[dim_y] += 1;
    (*next)[dim_x] += 0;
  }
  // up right
  if(min > d->pc_tunnel[from[dim_y] -1 ][from[dim_x] +1 ]){
    min = d->pc_tunnel[from[dim_y] -1 ][from[dim_x] +1 ];
    (*next)[dim_y] -= 1;
    (*next)[dim_x] += 1;
  }
  // down left
  if(min > d->pc_tunnel[from[dim_y] +1 ][from[dim_x] -1 ]){
    min = d->pc_tunnel[from[dim_y] +1 ][from[dim_x] -1 ];
    (*next)[dim_y] += 1;
    (*next)[dim_x] -= 1;
  }
  // down right
  if(min > d->pc_tunnel[from[dim_y] +1 ][from[dim_x] +1 ]){
    min = d->pc_tunnel[from[dim_y] +1 ][from[dim_x] +1 ];
    (*next)[dim_y] += 1;
    (*next)[dim_x] += 1;
  }
  // up left
  if(min > d->pc_tunnel[from[dim_y] -1 ][from[dim_x] -1 ]){
    min = d->pc_tunnel[from[dim_y] -1 ][from[dim_x] -1 ];
    (*next)[dim_y] -= 1;
    (*next)[dim_x] -= 1;
  }
  

}

static void pc_next_step(dungeon_t *d)
{
  pair_t next_move;
  next_move[dim_x] = d->pc.position[dim_x];
  next_move[dim_y] = d->pc.position[dim_y];

  // All corner locations
  int corners[4][2] = {
    {1, 1},  
    {1, DUNGEON_X - 2},  
    {DUNGEON_Y - 2, 1},  
    {DUNGEON_Y - 2, DUNGEON_X - 2}  
  };

  int nearest_corner[2] = {0, 0};
  int min_distance = DUNGEON_Y + DUNGEON_X; // initial max distance

  // Find the closest corner
  for (int i = 0; i < 4; i++) {
      int dist = abs(next_move[dim_x] - corners[i][1]) +
                abs(next_move[dim_y] - corners[i][0]);

      if (dist < min_distance) {
          min_distance = dist;
          nearest_corner[1] = corners[i][1];
          nearest_corner[0] = corners[i][0];
      }
  }

  // Move PC one step toward the closest corner
  if (next_move[dim_x] < nearest_corner[1]) {
    next_move[dim_x] += 1;
  } else if (next_move[dim_x] > nearest_corner[1]) {
    next_move[dim_x] -= 1;
  }

  if (next_move[dim_y] < nearest_corner[0]) {
    next_move[dim_y] += 1;
  } else if (next_move[dim_y] > nearest_corner[0]) {
    next_move[dim_y] -= 1;
  }

  // Handling if next step is a wall
  if(mappair(next_move) == ter_wall){
    d->hardness[next_move[dim_y]][next_move[dim_x]] = 0;
    d->map[next_move[dim_y]][next_move[dim_x]] = ter_floor_hall;
  }

  // Handling if next step kills a monster
  if (d->characters[next_move[dim_y]][next_move[dim_x]] != NULL) {
    ((monster_t*)d->characters[next_move[dim_y]][next_move[dim_x]])->alive = 0;
    free(d->characters[next_move[dim_y]][next_move[dim_x]]);
    d->characters[next_move[dim_y]][next_move[dim_x]] = NULL;
    d->num_monsters--;
  }

  // updating PC location
  d->pc.position[dim_x] = next_move[dim_x];
  d->pc.position[dim_y] = next_move[dim_y];
}

static int32_t character_cmp(const void *key, const void *with) 
{
  int32_t res = 0;
  int32_t key_NT, with_NT, key_SN, with_SN;

  // check if key is a PC or monster
  if (((pc_t *)key)->sequence_num == 0) {
    key_NT = ((pc_t *)key)->next_turn;
    key_SN = ((pc_t *)key)->sequence_num;
  } else {
    key_NT = ((monster_t *)key)->next_turn;
    key_SN = ((monster_t *)key)->sequence_num;
  }

  // check if with is a PC or monster
  if (((pc_t *)with)->sequence_num == 0) {
    with_NT = ((pc_t *)with)->next_turn;
    with_SN = ((pc_t *)with)->sequence_num;
  } else {
    with_NT = ((monster_t *)with)->next_turn;
    with_SN = ((monster_t *)with)->sequence_num;
  }

  // first compare the next turn values
  res = key_NT - with_NT;
  if (res != 0) {
      return res;
  }

  // then compare sequence numbers
  return key_SN - with_SN;
}

void render_gameover()
{
  printf("\n");
  printf("          ██████   █████  ███    ███ ███████  \n");
  printf("         ██       ██   ██ ████  ████ ██       \n");
  printf("         ██   ███ ███████ ██ ████ ██ █████    \n");
  printf("         ██    ██ ██   ██ ██  ██  ██ ██       \n");
  printf("          ██████  ██   ██ ██      ██ ███████  \n");
  printf("\n");
  printf("           ██████  ██    ██ ███████ ██████    \n");
  printf("          ██    ██ ██    ██ ██      ██   ██   \n");
  printf("          ██    ██  ██  ██  █████   ██████    \n");
  printf("          ██    ██  ██  ██  ██      ██   ██   \n");
  printf("           ██████    ████   ███████ ██   ██   \n");
  printf("\n");
  printf("                  YOU HAVE DIED. \n\n");
}

void render_game_win()
{
  printf("\n\n");
  printf("         ██     ██ ██ ███    ██ ███    ██ ███████ ██████  \n");
  printf("         ██     ██ ██ ████   ██ ████   ██ ██      ██   ██ \n");
  printf("         ██  █  ██ ██ ██ ██  ██ ██ ██  ██ █████   ██████  \n");
  printf("         ██ ███ ██ ██ ██  ██ ██ ██  ██ ██ ██      ██   ██ \n");
  printf("          ███ ███  ██ ██   ████ ██   ████ ███████ ██   ██ \n");
  printf("\n");
  printf("                  CONGRATULATIONS! YOU WON!\n\n");
}

int dungeon_turns(dungeon_t *d)
{
  heap_t h;
  int y, x;
  void *c;

  heap_init(&h, character_cmp, NULL);

  d->pc.hn = heap_insert(&h, &d->pc);
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->characters[y][x] != NULL) {
        ((monster_t *)d->characters[y][x])->hn = heap_insert(&h, d->characters[y][x]);
      }
    }
  }

  while ((c = heap_remove_min(&h))) {
    if(c != NULL){
      //--------------------------------------------------------------PC Behavior
      if (((pc_t *)c)->sequence_num == 0) {
        ((pc_t *)c)->hn = NULL;
        render_dungeon_mon(d);
        usleep(250000);
      
        // DEBUGGING pc moves right
        // if(d->pc.position[dim_x] < DUNGEON_X - 1){
        //   d->pc.position[dim_x] += 1;
        // }
        
        pc_next_step(d);

        // adding PC back to the queue
        d->pc.next_turn += 1000 / d->pc.speed;
        d->pc.hn = heap_insert(&h, &d->pc);

      //--------------------------------------------------------------Monster Behavior
      } else if(((monster_t *)c)->position[dim_y] >= 0 && 
                ((monster_t *)c)->position[dim_y] < DUNGEON_Y && 
                ((monster_t *)c)->position[dim_x] >= 0 && 
                ((monster_t *)c)->position[dim_x] < DUNGEON_X &&
                ((monster_t *)c)->alive == 1) {
        ((monster_t *)c)->hn = NULL;

        // Determining where the monster thinks/knows the PC is located
        if((monster_is(((monster_t *)c), TELEPATHIC))        ||
            (in_room(d, ((monster_t *)c)->pc_position) != 0   &&
            in_room(d, ((monster_t *)c)->pc_position) == in_room(d, d->pc.position))){
          //printf("Monster in room with PC\n");
          // is monster in the same room as PC or the monster is TELEPATHIC
          ((monster_t *)c)->pc_position[dim_y] = d->pc.position[dim_y];
          ((monster_t *)c)->pc_position[dim_x] = d->pc.position[dim_x];
        } else if(!(monster_is(((monster_t *)c), SMART))) {
          // monster is not SMART (no remembering)
          ((monster_t *)c)->pc_position[dim_y] = ((monster_t *)c)->position[dim_y];
          ((monster_t *)c)->pc_position[dim_x] = ((monster_t *)c)->position[dim_x];
        }

        // DEBUGGING
        // printf("Monster is at (y, x): %d, %d\n", ((monster_t *)c)->position[dim_y], ((monster_t *)c)->position[dim_x]);
        // printf("Monster thinks PC is at (y, x): %d, %d\n", ((monster_t *)c)->pc_position[dim_y], ((monster_t *)c)->pc_position[dim_x]);
        
        // Determining which way the monster wants to move
        pair_t next_move;
        if((monster_is(((monster_t *)c), ERRATIC)) && (rand_range(0, 1))){
          
          //50% chance for ERRATIC monsters
          if(monster_is(((monster_t *)c), TUNNEL)){
            // ERRATIC behavior for TUNNELER
            do{
              next_move[dim_y] = ((monster_t *)c)->position[dim_y] + rand_range(-1, 1);
              next_move[dim_x] = ((monster_t *)c)->position[dim_x] + rand_range(-1, 1);
            } while (next_move[dim_y] == ((monster_t *)c)->position[dim_y] && 
                    next_move[dim_x] == ((monster_t *)c)->position[dim_x]);
          }else{
            // ERRATIC behavior for non-TUNNELER
            do{
              next_move[dim_y] = ((monster_t *)c)->position[dim_y] + rand_range(-1, 1);
              next_move[dim_x] = ((monster_t *)c)->position[dim_x] + rand_range(-1, 1);
            } while (next_move[dim_y] == ((monster_t *)c)->position[dim_y] && 
                    next_move[dim_x] == ((monster_t *)c)->position[dim_x] &&
                    mappair(next_move) < ter_floor);
          }
        }
        else if(monster_is(((monster_t *)c), SMART) && monster_is(((monster_t *)c), TUNNEL)){
          // Monster is SMART and TUNNELLER
          smart_next_step_tunnel(d, ((monster_t *)c)->position, ((monster_t *)c)->pc_position, &next_move);
        }else if(monster_is(((monster_t *)c), SMART)){
          // Monster is SMART and non-TUNNELLER
          smart_next_step(d, ((monster_t *)c)->position, ((monster_t *)c)->pc_position, &next_move);
        }else{
          // Monster is not SMART
          bresendam_next_step(((monster_t *)c)->position, ((monster_t *)c)->pc_position, &next_move);
        }
        
        // DEBUGGING
        //printf("Monster thinks next move is at (y, x): %d, %d\n", next_move[dim_y], next_move[dim_x]);
        
        // Determining the outcome of the monster's desired next move
        if(mappair(next_move) > ter_floor){
          
          // Next move is open space
          d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]] = NULL;
          ((monster_t *)c)->position[dim_y] = next_move[dim_y];
          ((monster_t *)c)->position[dim_x] = next_move[dim_x];
        
          // Next move kills the PC
          if(((monster_t *)c)->position[dim_y] == d->pc.position[dim_y] && ((monster_t *)c)->position[dim_x] == d->pc.position[dim_x]){
            d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]] = ((monster_t *)c);
            render_dungeon_mon(d);
            render_gameover();
            break;
          }

          // Next move kills another monster
          if (d->characters[((monster_t *)c)->position[dim_y]]
                          [((monster_t *)c)->position[dim_x]] != NULL) {
            // heap_node_delete(&h, ((monster_t *)d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]])->hn);
            // ((monster_t *)d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]])->hn = NULL;
            ((monster_t*)d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]])->alive = 0;
            free(d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]]);
            d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]] = NULL;
            d->num_monsters--;
            // DEBUGGING
            //printf("-----------------------------------------------------Monster Killed\n");
          }

          // Setting new position
          d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]] = ((monster_t *)c);
        } // movement for tunnelers
        else if(mappair(next_move) == ter_wall && monster_is(((monster_t *)c), TUNNEL)){
          
          // Next move is rock and monster is TUNNELER
          if(d->hardness[next_move[dim_y]][next_move[dim_x]] <= TUNNEL_DAMAGE){
            // If the rock hardness <= 85, the rock converts into a hallway
            d->hardness[next_move[dim_y]][next_move[dim_x]] = 0;
            d->map[next_move[dim_y]][next_move[dim_x]] = ter_floor_hall;
            
            // Setting new position
            d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]] = NULL;
            ((monster_t *)c)->position[dim_y] = next_move[dim_y];
            ((monster_t *)c)->position[dim_x] = next_move[dim_x];
            d->characters[((monster_t *)c)->position[dim_y]][((monster_t *)c)->position[dim_x]] = ((monster_t *)c);
          }else{
            // If the rock hardness > 85, the rock hardness is lowered
            d->hardness[next_move[dim_y]][next_move[dim_x]] -= 85;
          }
        }

        // Updating the monster's next turn status and adding it back to the queue
        ((monster_t *)c)->next_turn += 1000 / ((monster_t *)c)->speed;
        ((monster_t *)d->characters[y][x])->hn = heap_insert(&h, c);
      }

      // If there are no more monsters, the game exits with PC win status
      if(d->num_monsters == 0){
        render_dungeon_mon(d);
        render_game_win();
        break;
      }
    }

  }
  heap_delete(&h);
  return 0;
  
}






