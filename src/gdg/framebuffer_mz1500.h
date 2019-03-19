/* 
 * File:   framebuffer_mz1500.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 5. července 2015, 8:43
 * 
 * 
 * ----------------------------- License -------------------------------------
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * ---------------------------------------------------------------------------
 */

#ifndef FRAMEBUFFER_MZ1500_H
#define FRAMEBUFFER_MZ1500_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ1500

#include <stdint.h>
#include "z80ex/include/z80ex.h"

    /* Definice barev pro MZ700  (pouzita skala 0 - 15 z definice colormap MZ800) */
    const static uint8_t c_MZ1500_COLORMAP[8] = { 0, 9, 10, 11, 12, 13, 14, 15 };

#define FRAMEBUFFER_MZ1500_CHARS_WIDTH       40
#define FRAMEBUFFER_MZ1500_CHARS_HEIGHT      25

#define FRAMEBUFFER_MZ1500_PIXEL_WIDTH       2
#define FRAMEBUFFER_MZ1500_PIXEL_HEIGHT      1

#define FRAMEBUFFER_MZ1500_CANVAS_WIDTH      ( 8 * FRAMEBUFFER_MZ1500_PIXEL_WIDTH * FRAMEBUFFER_MZ1500_CHARS_WIDTH )
#define FRAMEBUFFER_MZ1500_CANVAS_HEIGHT     ( 8 * FRAMEBUFFER_MZ1500_PIXEL_HEIGHT * FRAMEBUFFER_MZ1500_CHARS_HEIGHT )

#define FRAMEBUFFER_MZ1500_ATTR_VRAM         0x0800
#define FRAMEBUFFER_MZ1500_ATTR_PCGLO        0x0400
#define FRAMEBUFFER_MZ1500_ATTR_PCGHI        0x0c00

    extern void framebuffer_mz1500_init ( Z80EX_BYTE *vram, Z80EX_BYTE *cg, Z80EX_BYTE *pcg0, Z80EX_BYTE *pcg1, Z80EX_BYTE *pcg2, uint8_t *px, int px_width, int px_height, int display_width, int canvas_first_row, int left_border_width );

    extern void framebuffer_mz1500_set_vram ( Z80EX_BYTE *vram );
    extern void framebuffer_mz1500_set_cg ( Z80EX_BYTE *cg );
    extern void framebuffer_mz1500_set_pcg ( Z80EX_BYTE *pcg0, Z80EX_BYTE *pcg1, Z80EX_BYTE *pcg2 );
    extern void framebuffer_mz1500_set_pixels ( uint8_t *px, int px_width, int px_height );
    extern void framebuffer_mz1500_set_video ( int display_width, int canvas_first_row, int left_border_width );
    extern void framebuffer_mz1500_set_color ( int pal, int color );

    extern void framebuffer_mz1500_update_full_current_screen_row ( void );
    extern void framebuffer_mz1500_update_all_rows ( void );

#endif

#ifdef __cplusplus
}
#endif

#endif /* FRAMEBUFFER_MZ1500_H */

