#ifndef NPC_TYPE_H
#define NPC_TYPE_H

#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_set>

#define MONSTER_DESC_FILE "monster_desc.txt"

// defined color options
const std::unordered_set<std::string> valid_colors = {
    "RED", "GREEN", "BLUE", "CYAN", "YELLOW", "MAGENTA", "WHITE", "BLACK"
};

// defined ability options
const std::unordered_set<std::string> valid_ability = {
    "SMART", "TELE", "TUNNEL", "ERRATIC", "PASS", "PICKUP", "DESTROY", "UNIQ", "BOSS"
};

typedef enum fields {
    NAME,
    DESC,
    COLOR,
    SPEED,
    ABIL,
    HP,
    DAM,
    SYMB,
    RRTY,
    num_fields
} fields_t;

typedef struct dice {
    int base;
    int dice;
    int sides;
} dice_t;

class npc_type {
    public:
        std::string name;
        std::string desc;
        std::vector<std::string> color;     
        dice_t speed;
        std::vector<std::string> abilities; 
        dice_t hitpoints;
        dice_t att_damage;
        char symbol;
        int rarity;
        int valid[num_fields] = {}; 
};


std::vector<npc_type> read_monster_desc();
void print_monster_desc(const std::vector<npc_type>& npcs);
int fill_field(const std::string& field, npc_type& npc, std::ifstream& i, std::istringstream& iss);


#endif
