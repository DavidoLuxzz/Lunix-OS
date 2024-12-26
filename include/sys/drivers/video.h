/** -- Lunix default middle degree video driver --
 * -> Thu Dec 26 12:33
 *
 * This is NOT a real video driver like for Intel HD, nVidia GPUs or basically VGA.
 * It is called middle degree because it is in middle of a real driver and a UI (window 
 * manager).
 *
 * real driver - MDV driver - UI
 */
#pragma once
#include <stdint.h>
#include <sys/driver.h>

void __lunix_video_clear_screen(void);
void __lunix_video_newLine(void);
void __lunix_video_scrollUp(void);
void __lunix_video_deleteLast(void);

const driver_t *lunix_video_get_driver(void);
