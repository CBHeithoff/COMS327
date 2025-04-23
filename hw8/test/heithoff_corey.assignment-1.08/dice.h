#ifndef DICE_H
# define DICE_H

# include <stdint.h>
# include <iostream>

typedef enum object_type {
  objtype_no_type,
  objtype_WEAPON,
  objtype_OFFHAND,
  objtype_RANGED,
  objtype_LIGHT,
  objtype_ARMOR,
  objtype_HELMET,
  objtype_CLOAK,
  objtype_GLOVES,
  objtype_BOOTS,
  objtype_AMULET,
  objtype_RING,
  objtype_SCROLL,
  objtype_BOOK,
  objtype_FLASK,
  objtype_GOLD,
  objtype_AMMUNITION,
  objtype_FOOD,
  objtype_WAND,
  objtype_CONTAINER
} object_type_t;

const char object_symbol[] = {
  '*', /* objtype_no_type */
  '|', /* objtype_WEAPON */
  ')', /* objtype_OFFHAND */
  '}', /* objtype_RANGED */
  '~', /* objtype_LIGHT */
  '[', /* objtype_ARMOR */
  ']', /* objtype_HELMET */
  '(', /* objtype_CLOAK */
  '{', /* objtype_GLOVES */
  '\\', /* objtype_BOOTS */
  '"', /* objtype_AMULET */
  '=', /* objtype_RING */
  '`', /* objtype_SCROLL */
  '?', /* objtype_BOOK */
  '!', /* objtype_FLASK */
  '$', /* objtype_GOLD */
  '/', /* objtype_AMMUNITION */
  ',', /* objtype_FOOD */
  '-', /* objtype_WAND */
  '%', /* objtype_CONTAINER */
};

class dice {
 private:
  int32_t base;
  uint32_t number, sides;
 public:
  dice() : base(0), number(0), sides(0)
  {
  }
  dice(int32_t base, uint32_t number, uint32_t sides) :
  base(base), number(number), sides(sides)
  {
  }
  inline void set(int32_t base, uint32_t number, uint32_t sides)
  {
    this->base = base;
    this->number = number;
    this->sides = sides;
  }
  inline void set_base(int32_t base)
  {
    this->base = base;
  }
  inline void set_number(uint32_t number)
  {
    this->number = number;
  }
  inline void set_sides(uint32_t sides)
  {
    this->sides = sides;
  }
  int32_t roll(void) const;
  std::ostream &print(std::ostream &o);
  inline int32_t get_base() const
  {
    return base;
  }
  inline int32_t get_number() const
  {
    return number;
  }
  inline int32_t get_sides() const
  {
    return sides;
  }
};

std::ostream &operator<<(std::ostream &o, dice &d);

#endif
