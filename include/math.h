/** -- LunixOS standard Math Header --
 * -> Wed 23 Oct 2024
 * Just a standard math.h header nothing much to be documented
 */

#pragma once
#include <stdint.h>

static uint32_t umin(uint32_t a, uint32_t b){ if (a<b) return a; return b; }
static uint32_t umax(uint32_t a, uint32_t b){ if (a>b) return a; return b; }
static int min(int a, int b){ if (a<b) return a; return b; }
static int max(int a, int b){ if (a>b) return a; return b; }



