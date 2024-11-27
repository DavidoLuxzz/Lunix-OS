/** -- Lunix default usering implementation --
 * -> Wed Nov 27 15:50
 *
 * This is the default Lunix users header.
 * Contains the user_t struct
 */
#pragma once
#include <stdint.h>

/** Typedef and struct definition of user_t
 */
typedef struct user_t {
    /* User ID */
    int     uid;
    /* User privlege level (defined below) */
    uint8_t plevel;
    /* User type (defined below) */
    uint8_t type;
} user_t;

// #Definitions //
/* misc */
#define MAX_USERS   256         // Max users to exist on a operating system
/* privlege levels */
enum user_plevel {
    USR_PL_0,   // highest privlege level (everything to even system deletion)
    USR_PL_1,   // tbd
    USR_PL_2,   // tbd
    USR_PL_3    // lowest privlege level (can't access anything)
};
/* user types (the names describe them enough) */
#define USER_ROOT   0
#define USER_ADMIN  1
#define USER_MEMBER 2
#define USER_GUEST  3

/**
 * Initialize "/.users" file. If doesn't exists, then create.
 */
int init_users_file(void);

/**
 * Initialize users.
 */
int init_users(void);

/**
 * Add user (memory)
 */
int usr_add(user_t);

/**
 * Remove user (memory)
 */
int usr_remove(int index);

/**
 * Set user at specific index (memory)
 */
int usr_set(int index, user_t user);

/**
 * Save users array to memory (disk)
 */
int save_users_file(void);




