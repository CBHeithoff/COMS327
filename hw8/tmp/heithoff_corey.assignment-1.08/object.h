#ifndef OBJECT_H
# define OBJECT_H

# include <stdint.h>

# include "dice.h"
# include "descriptions.h"
# include "dims.h"

class object {
    public:
     std::string name;
     object_type_t type;
     uint32_t color;
     dice damage;
     bool destroyed;
     char symbol;
     int32_t hit, dodge, defence, weight, speed, attribute, value;
     int obj_desc_id;
     object(std::string name, object_type_t type, uint32_t color, dice damageBonus, int32_t hitBonus, int32_t dodgeBonus,
        int32_t defenseBonus, int32_t weight, int32_t speedBonus, int32_t attribute, int32_t value) : 
        name(name), type(type), color(color), damage(damageBonus), hit(hitBonus), dodge(dodgeBonus), 
        defence(defenseBonus), weight(weight), speed(speedBonus), attribute(attribute), value(value), obj_desc_id(0) {
            destroyed = false;
            symbol = object_symbol[type];
        }
};

class dungeon;

void gen_objects(dungeon *d);



#endif