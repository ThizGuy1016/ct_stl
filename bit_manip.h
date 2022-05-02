#ifndef _CT_BIT_MANIP_H
#define _CT_BIT_MANIP_H

#include "types.h"

#define SET_UNSIGNED_BIT(val, pos) (val|(1U<<pos))  
#define CHECK_UNSIGNED_BIT(val, pos) (val & (1U<<pos))

/*
 | Shoutout to this absolute unit who collected all of these bit twiddling hacks
 | By Sean Eron Anderson
 | seander@cs.stanford.edu
 | Source: https://graphics.stanford.edu/~seander/bithacks.html
*/

#define HAS_ZERO(x) (((x) - 0x01010101UL) & ~(x) & 0x80808080UL)
#define HAS_VALUE(x, n) (HAS_ZERO(x) ^ (~0UL/255 * (n)))
#define HASS_LESS(x, n) (((x)-~0UL/255*(n))&~(x)&~0UL/255*128)
#define COUNT_LESS(x,n) (((~0UL/255*(127+(n))-((x)&~0UL/255*127))&~(x)&~0UL/255*128)/128%255)
#define SWAP_VAL(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#endif // End _CT_BIT_MANIP_H
