/** -- Lunix default usering implementation --
 * -> Wed Nov 27 15:50
 * This is the default Lunix users header...
 */
#pragma once
#include <stdint.h>

/**
 * Initialize "/.users" file. If doesn't exists, then create.
 */
int init_users_file(void);

