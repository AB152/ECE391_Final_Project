/* paging.h (MP3.1): Header file for paging.c
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

// Define macros for byte sizes
#define ONE_KB 1024             // 1024 bytes make a KB
#define FOUR_MB 0x400000        // Coincidentally, ONE_KB * ONE_KB * 4 == 0x400000
#define VIDEO 0xB8000           // Copied from lib.c (this shouldn't be a problem)

#endif /* _PAGING_H */
