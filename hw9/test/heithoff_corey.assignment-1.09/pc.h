#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "character.h"
# include "dungeon.h"

typedef enum pc_equipment {
  WEAPON,
  OFFHAND,
  RANGED,
  ARMOR,
  HELMET,
  CLOAK,
  GLOVES,
  BOOTS,
  AMULET,
  LIGHT,
  RING_1,
  RING_2,
  num_equipment,
} pc_equipment_t;

class pc : public character {
 public:
  ~pc() {}
  terrain_type known_terrain[DUNGEON_Y][DUNGEON_X];
  uint8_t visible[DUNGEON_Y][DUNGEON_X];
  object *equipment[num_equipment];
  //object *inventory[INVENTORY_SIZE];
};

void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, uint32_t room);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter);
terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon *d);
int32_t is_illuminated(pc *p, int16_t y, int16_t x);
void pc_reset_visibility(pc *p);
void drop_inventory_item(dungeon *d, uint32_t carry_slot);
void expunge_inventory_item(dungeon *d, uint32_t carry_slot);
void wear_inventory_item(dungeon *d, uint32_t carry_slot);
void take_off_equipment_item(dungeon *d, uint32_t carry_slot);
void update_equipment_bonus(dungeon *d);

#endif
