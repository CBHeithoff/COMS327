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
#include <ctype.h>

#include "heap.h"

/* Returns true if random float in [0,1] is less than *
 * numerator/denominator.  Uses only integer math.    */
# define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))

/* Returns random integer in [min, max]. */
# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))
# define UNUSED(f) ((void) f)

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

#define DUNGEON_X              80
#define DUNGEON_Y              21
#define MIN_ROOMS              6
#define MAX_ROOMS              10
#define ROOM_MIN_X             4
#define ROOM_MIN_Y             3
#define ROOM_MAX_X             20
#define ROOM_MAX_Y             15

#define mappair(pair) (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (d->map[y][x])
#define hardnesspair(pair) (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y) (d->hardness[y][x])

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_debug,
  ter_wall,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_stairs,
  ter_stairs_up,
  ter_stairs_down,
  ter_pc
} terrain_type_t;

typedef struct room {
  pair_t position;
  pair_t size;
} room_t;

typedef struct dungeon {
  uint32_t num_rooms;
  //room_t rooms[MAX_ROOMS];
  room_t *rooms;
  terrain_type_t map[DUNGEON_Y][DUNGEON_X];
  /* Since hardness is usually not used, it would be expensive to pull it *
   * into cache every time we need a map cell, so we store it in a        *
   * parallel array, rather than using a structure to represent the       *
   * cells.  We may want a cell structure later, but from a performanace  *
   * perspective, it would be a bad idea to ever have the map be part of  *
   * that structure.  Pathfinding will require efficient use of the map,  *
   * and pulling in unnecessary data with each map cell would add a lot   *
   * of overhead to the memory system.                                    */
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

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
  FILE *out;
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

  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&hardness, sizeof (hardness), 1, out);
  fclose(out);

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


  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&hardness, sizeof (hardness), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&d->hardness, sizeof (d->hardness), 1, out);
  fclose(out);

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

static int allocate_rooms(dungeon_t *d)
{
  if(!(d->rooms = malloc(d->num_rooms * sizeof(room_t)))){
    return -1;
  }

  return 0;
}

static void destroy_rooms(dungeon_t *d)
{
  free(d->rooms);
}

static int make_rooms(dungeon_t *d)
{
  uint32_t i;

  //memset(d->rooms, 0, sizeof (d->rooms));

  for (i = MIN_ROOMS; i < MAX_ROOMS && rand_under(5, 8); i++)
    ;
  d->num_rooms = i;

  allocate_rooms(d);
  memset(d->rooms, 0, d->num_rooms * sizeof(room_t));

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
      switch (mappair(p)) {
      case ter_wall:
        putchar(' ');
        break;
      case ter_wall_immutable:
        //putchar('W');
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
      case ter_pc:
        putchar('@');
        break;
      default:
        break;
      }
    }
    putchar('\n');
  }
}

void delete_dungeon(dungeon_t *d)
{
  destroy_rooms(d);
}

void init_dungeon(dungeon_t *d)
{
  empty_dungeon(d);
}

int load_dungeon(dungeon_t *d)
{
  int i, j, r, row, col, rooms;
  uint8_t r_x, r_y, size_x, size_y, pc_x, pc_y;
  uint8_t byte1;
  uint16_t byte2;
  uint32_t byte4;//, fver, fsize; // fver and fsize are for debugging

  FILE *f;

  char *home;
  char *dungeon_file;
  int dungeon_file_length;

  // opening dungeon file
  home = getenv("HOME");
  dungeon_file_length = strlen(home) + strlen("/.rlg327/dungeon") + 1; // +1 for the null byte
  //dungeon_file_length = strlen(home) + strlen("/.rlg327/01.rlg327") + 1; 
  dungeon_file = malloc(dungeon_file_length * sizeof (*dungeon_file));
  strcpy(dungeon_file, home);
  strcat(dungeon_file, "/.rlg327/dungeon");
  //strcat(dungeon_file, "/.rlg327/01.rlg327");
  // Now you can fopen() dungeon_file for reading or writing as needed
  if(!(f = fopen(dungeon_file, "rb"))){
    return -1;
  }

  // file-type marker
  for (int i = 0; i < 12; i++) {
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }

    // debugging
    // printf("%c", byte1); // Print as character
  }
  // printf("\n");

  fseek(f, 12, SEEK_SET);

  // file version
  if(fread(&byte4, sizeof (byte4), 1, f) != 1){
    fclose(f);
    return -2;
  }
  // debugging
  // fver = be32toh(byte4);
  // printf("file version #: %d\n", fver);

  // file size
  if(fread(&byte4, sizeof (byte4), 1, f) != 1){
    fclose(f);
    return -2;
  }
  // debugging
  // fsize = be32toh(byte4);
  // printf("file size: %d\n", fsize);

  fseek(f, 20, SEEK_SET);

  // PC position
  if(fread(&byte1, sizeof(byte1), 1, f) != 1){
    fclose(f);
    return -2;
  }
  pc_x = byte1;
  if(fread(&byte1, sizeof(byte1), 1, f) != 1){
    fclose(f);
    return -2;
  }
  pc_y = byte1;

  // debugging
  // printf("pc_x: %d\n", pc_x);
  // printf("pc_y: %d\n", pc_y);

  fseek(f, 22, SEEK_SET);
  
  // cell hardness
  for(i = 0; i < DUNGEON_X * DUNGEON_Y; i++){
    row = i / DUNGEON_X;   
    col = i % DUNGEON_X;  
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    d->hardness[row][col] = byte1;
    d->map[row][col] = ter_wall;
  }

  // Debugging
  // for (i = 0; i < DUNGEON_Y; i++) {
  //   for (j = 0; j < DUNGEON_X; j++) {
  //       printf("%3d ", d->hardness[i][j]);  // Print with formatting
  //   }
  //   printf("\n");
  // }

  // Number of rooms
  if(fread(&byte2, sizeof(byte2), 1, f) != 1){
    fclose(f);
    return -2;
  }
  rooms = be16toh(byte2);
  d->num_rooms = rooms;
  
  // debugging
  // printf("rooms #: %d\n", rooms);

  // allocate dynamic room array
  allocate_rooms(d);
  memset(d->rooms, 0, d->num_rooms * sizeof(room_t));

  // room position and size
  for(r = 0; r < rooms; r++){

    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    r_x = byte1;
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    r_y = byte1;
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    size_x = byte1;
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    size_y = byte1;

    d->rooms[r].position[dim_x] = r_x;
    d->rooms[r].position[dim_y] = r_y;
    d->rooms[r].size[dim_x] = size_x;
    d->rooms[r].size[dim_y] = size_y;

    // Debugging
    // printf("x: %d, y: %d, length: %d, height: %d\n", r_x, r_y, size_x, size_y);

    for(i = r_y; i < r_y + size_y; i++){
      for(j = r_x; j < r_x + size_x; j++){  
          d->map[i][j] = ter_floor_room;
      }

    }
  }

  // number of upward staircases
  uint16_t upstair;
  if(fread(&byte2, sizeof(byte2), 1, f) != 1){
    fclose(f);
    return -2;
  }
  upstair = be16toh(byte2);

  // debugging
  // printf("upstair #: %d\n", upstair);

  // upward staircase positions
  for(r = 0; r < upstair; r++){
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    r_x = byte1;
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    r_y = byte1;

    d->map[r_y][r_x] = ter_stairs_up;
  }

  // number of downward staircases
  uint16_t downstair;
  if(fread(&byte2, sizeof(byte2), 1, f) != 1){
    fclose(f);
    return -2;
  }
  downstair = be16toh(byte2);
  
  // debugging
  // printf("upstair #: %d\n", downstair);

  // downward staircase positions
  for(r = 0; r < downstair; r++){
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    r_x = byte1;
    if(fread(&byte1, sizeof(byte1), 1, f) != 1){
      fclose(f);
      return -2;
    }
    r_y = byte1;

    d->map[r_y][r_x] = ter_stairs_down;
  }

  // setting dungeon boundary, wall/rocks, and corridors
  for(i = 0; i < DUNGEON_Y; i++){
    for(j = 0; j < DUNGEON_X; j++){
      if(d->hardness[i][j] == 255){
        d->map[i][j] = ter_wall_immutable;  
      }else if(d->hardness[i][j] > 0 ){
        d->map[i][j] = ter_wall;
      }else if(d->hardness[i][j] == 0 && (d->map[i][j] != ter_floor_room) && (d->map[i][j] != ter_stairs_down) && (d->map[i][j] != ter_stairs_up)){
        d->map[i][j] = ter_floor_hall;
      }
    }
  }

  // placing PC '@'
  if(d->map[pc_y][pc_x] != ter_wall_immutable){
    d->map[pc_y][pc_x] = ter_pc;
  }

  fclose(f);
  return 0;

}

int save_dungeon(dungeon_t *d)
{
  int i, j;
  uint8_t byte1, pc_x, pc_y;
  uint16_t byte2, r, up, dn;
  uint32_t byte4, fsize;

  FILE *f;
  char *home;
  char *dungeon_file;
  int dungeon_file_length;

  home = getenv("HOME");
  dungeon_file_length = strlen(home) + strlen("/.rlg327/dungeon") + 1; // +1 for the null byte
  dungeon_file = malloc(dungeon_file_length * sizeof (*dungeon_file));
  strcpy(dungeon_file, home);
  strcat(dungeon_file, "/.rlg327/dungeon");
  // Now you can fopen() dungeon_file for reading or writing as needed
  if(!(f = fopen(dungeon_file, "wb"))){
    return -1;
  }

  // semantic file-type marker
  fseek(f, 0, SEEK_SET);

  const char mkr[] = "RLG327-S2025";
  for (size_t i = 0; i < sizeof(mkr) - 1; i++) {
    byte1 = mkr[i];
    if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
      fclose(f);
      return -2;
    }
  }

  // file version marker
  fseek(f, 12, SEEK_SET);
  byte4 = htobe32(0);
  if (fwrite(&byte4, sizeof(byte4), 1, f) != 1) {
    fclose(f);
    return -2;
  }

  // calculating some values
  r = d->num_rooms;
  up = dn = 0;
  for(i = 0; i < DUNGEON_Y; i++){
    for(j = 0; j < DUNGEON_X; j++){
      if(d->map[i][j] == ter_stairs_up){
        up += 1;
      }else if(d->map[i][j] == ter_stairs_down){
        dn += 1;
      }else if(d->map[i][j] == ter_pc){
        pc_y = (uint8_t) i;
        pc_x = (uint8_t) j;
      }
    }
  }
  
  // file size
  fsize = 12 + 4 + 4 + 2 + 1680 + 2 + (r * 4) + 2 + (up * 2) + 2 + (dn * 2);
  // printf("file size: %d\n", fsize);
  fseek(f, 16, SEEK_SET);
  byte4 = htobe32(fsize);
  if (fwrite(&byte4, sizeof(byte4), 1, f) != 1) {
    fclose(f);
    return -2;
  }

  // PC position
  byte1 = pc_x;
  // printf("pc_x: %d\n", byte1);
  if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
    fclose(f);
    return -2;
  }
  byte1 = pc_y;
  // printf("pc_y: %d\n", byte1);
  if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
    fclose(f);
    return -2;
  }

  // Debugging
  // for (i = 0; i < DUNGEON_Y; i++) {
  //   for (j = 0; j < DUNGEON_X; j++) {
  //       printf("%3d ", d->hardness[i][j]);  // Print with formatting
  //   }
  //   printf("\n");
  // }

  // cell hardness
  fseek(f, 22, SEEK_SET);
  for(i = 0; i < DUNGEON_Y; i++){
    for(j = 0; j < DUNGEON_X; j++){
      byte1 = d->hardness[i][j];
      if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
        fclose(f);
        return -2;
      }
    }
  }

  // number of rooms
  byte2 = htobe16(r);
  if (fwrite(&byte2, sizeof(byte2), 1, f) != 1) {
    fclose(f);
    return -2;
  }

  // room position and sizes
  fseek(f, 1704, SEEK_SET);
  for(i = 0; i < d->num_rooms; i++){
    byte1 = d->rooms[i].position[dim_x];
    if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
      fclose(f);
      return -2;
    }
    byte1 = d->rooms[i].position[dim_y];
    if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
      fclose(f);
      return -2;
    }
    byte1 = d->rooms[i].size[dim_x];
    if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
      fclose(f);
      return -2;
    }
    byte1 = d->rooms[i].size[dim_y];
    if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
      fclose(f);
      return -2;
    }
  }

  // number of upward staircases
  byte2 = htobe16(up);
  if (fwrite(&byte2, sizeof(byte2), 1, f) != 1) {
    fclose(f);
    return -2;
  }

  // upward staircase positions
  for(i = 0; i < DUNGEON_Y; i++){
    for(j = 0; j < DUNGEON_X; j++){
      if(d->map[i][j] == ter_stairs_up){
        byte1 = j;
        if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
          fclose(f);
          return -2;
        }
        byte1 = i;
        if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
          fclose(f);
          return -2;
        }
      }
    }
  }

  // number of downward staircases
  byte2 = htobe16(dn);
  if (fwrite(&byte2, sizeof(byte2), 1, f) != 1) {
    fclose(f);
    return -2;
  }

  // downward staircase positions
  for(i = 0; i < DUNGEON_Y; i++){
    for(j = 0; j < DUNGEON_X; j++){
      if(d->map[i][j] == ter_stairs_down){
        byte1 = j;
        if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
          fclose(f);
          return -2;
        }
        byte1 = i;
        if (fwrite(&byte1, sizeof(byte1), 1, f) != 1) {
          fclose(f);
          return -2;
        }
      }
    }
  }

  fclose(f);
  return 0;

}

int main(int argc, char *argv[])
{
  dungeon_t d;
  struct timeval tv;
  uint32_t seed;
  int i, result, save, load;
  save = load = result = 0;

  UNUSED(in_room);

  // checking for switches and calculating seed
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "--save") == 0) {
        save = 1;
      }
      if (strcmp(argv[i], "--load") == 0) {
        load = 1;
      }
    }
    for (i = 1; i < argc; i++) {
      seed = atoi(argv[i]);
      if (seed != 0 || (seed == 0 && isdigit(argv[i][0]))) {
        // seed is in proper form
        break;
      } else if(!load) {
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
      }
    }
  }else{
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Using seed: %u\n", seed);
  srand(seed);

  // loading or generating
  if(load){
    result = load_dungeon(&d);
    
    // error handling
    if(result == -1){
      fprintf(stderr, "Error: Failed to open dungeon file.\n");
      delete_dungeon(&d);
      return 0;
    }else if(result == -2){
      fprintf(stderr, "Error: Failed to read dungeon file data.\n");
      delete_dungeon(&d);
      return 0;
    }
  }else{
    init_dungeon(&d);
    gen_dungeon(&d);
  }

  render_dungeon(&d);

  // saving if needed
  if(save){
    result = save_dungeon(&d);

    // error handling
    if(result == -1){
      fprintf(stderr, "Error: Failed to open dungeon save file.\n");
      delete_dungeon(&d);
      return 0;
    }else if(result == -2){
      fprintf(stderr, "Error: Failed to save dungeon file data.\n");
      delete_dungeon(&d);
      return 0;
    }
  }

  // deallocating dungeon room array
  delete_dungeon(&d);

  return 0;
}
