/* 
 * File:   framebuffer_mz700.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 5. července 2015, 8:40
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

/*
 * VRAM a seskladani obrazu:
 * -------------------------
 * 
 * 0xD000 - 0xD320: Znakova VRAM, kod znaku z CGROM (v MZ800 z CGRAM)
 * 0xD800 - 0xDB20: Atributova VRAM
 * 
 */

#include "mz800emu_cfg.h"

#include <stdint.h>

#ifdef MACHINE_EMU_MZ800

#include "z80ex/include/z80ex.h"

#include "framebuffer_mz700.h"
#include "gdg.h"

/* Definice barev pro MZ700  (pouzita skala 0 - 15 z definice colormap MZ800) */
const static int c_MZ700_COLORMAP [] = { 0, 9, 10, 11, 12, 13, 14, 15 };


typedef struct st_FRAMEBUFFER_MZ700 {
    Z80EX_BYTE *VRAM; // prostor CHAR_VRAM a ATTR_VRAM
    Z80EX_BYTE *CG; // prostor s aktivnimi maskami CGROM | CGRAM

    uint8_t *PX; // 8 bitovy pixels surface (1px = 1 bajt)
    int one_pixel_width; // sirka jednoho pixelu
    int one_pixel_height; // vyska jednoho pixelu

    int video_canvas_first_row;
    int video_border_left_width;
    int video_display_width; // pocet pixelu na jeden cely obrazovy radek
} st_FRAMEBUFFER_MZ700;

st_FRAMEBUFFER_MZ700 g_framebuffer_mz1500;


void framebuffer_mz700_init ( Z80EX_BYTE *vram, Z80EX_BYTE *cg, uint8_t *px, int px_width, int px_height, int display_width, int canvas_first_row, int left_border_width ) {
    g_framebuffer_mz1500.VRAM = vram;
    g_framebuffer_mz1500.CG = cg;

    g_framebuffer_mz1500.PX = px;
    g_framebuffer_mz1500.one_pixel_width = px_width;
    g_framebuffer_mz1500.one_pixel_height = px_height;

    g_framebuffer_mz1500.video_display_width = display_width;
    g_framebuffer_mz1500.video_canvas_first_row = canvas_first_row;
    g_framebuffer_mz1500.video_border_left_width = left_border_width;
}


void framebuffer_mz700_set_vram ( Z80EX_BYTE *vram ) {
    g_framebuffer_mz1500.VRAM = vram;
}


void framebuffer_mz700_set_cg ( Z80EX_BYTE *cg ) {
    g_framebuffer_mz1500.CG = cg;
}


void framebuffer_mz700_set_pixels ( uint8_t *px, int px_width, int px_height ) {
    g_framebuffer_mz1500.PX = px;
    g_framebuffer_mz1500.one_pixel_width = px_width;
    g_framebuffer_mz1500.one_pixel_height = px_height;
}


void framebuffer_mz700_set_video ( int display_width, int canvas_first_row, int left_border_width ) {
    g_framebuffer_mz1500.video_display_width = display_width;
    g_framebuffer_mz1500.video_canvas_first_row = canvas_first_row;
    g_framebuffer_mz1500.video_border_left_width = left_border_width;
}


/**
 * Vykresleni obrazoveho radku platna (CANVAS) v rezimu MZ700.
 * 
 */
static inline void framebuffer_mz700_update_full_screen_row ( int row ) {

    int canvas_row = row - g_framebuffer_mz1500.video_canvas_first_row;
    int vram_position = ( canvas_row / 8 ) * FRAMEBUFFER_MZ700_CHARS_WIDTH;

    Z80EX_BYTE *CHAR_VRAM = &g_framebuffer_mz1500.VRAM[vram_position];
    Z80EX_BYTE *ATTR_VRAM = &g_framebuffer_mz1500.VRAM[( FRAMEBUFFER_MZ700_ATTR_VRAM + vram_position )];

    int px_position = ( row * ( g_framebuffer_mz1500.video_display_width * g_framebuffer_mz1500.one_pixel_height ) ) + g_framebuffer_mz1500.video_border_left_width;
    uint8_t *ROW_PX = &g_framebuffer_mz1500.PX[px_position];

    int px_height;
    for ( px_height = 0; px_height < g_framebuffer_mz1500.one_pixel_height; px_height++ ) {

        Z80EX_BYTE *char_p = CHAR_VRAM;
        Z80EX_BYTE *attr_p = ATTR_VRAM;
        uint8_t *px_p = &ROW_PX[px_height * g_framebuffer_mz1500.video_display_width];

        int char_x;
        for ( char_x = 0; char_x < FRAMEBUFFER_MZ700_CHARS_WIDTH; char_x++ ) {

            int attr_fgc = ( *attr_p >> 4 ) & 0x07;
            int attr_bgc = *attr_p & 0x07;
            int attr_grp = ( ( *attr_p << 4 ) & 0x0800 );

            int cg_addr = attr_grp | ( *char_p * 8 );

            int mask = g_framebuffer_mz1500.CG[( cg_addr + canvas_row % 8 )];

            char_p++;
            attr_p++;

            int mask_bit;
            for ( mask_bit = 0; mask_bit < 8; mask_bit++ ) {
                uint8_t value = ( mask & 1 ) ? c_MZ700_COLORMAP[attr_fgc] : c_MZ700_COLORMAP[attr_bgc];
                int px_width;
                for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                    *px_p++ = value;
                };
                mask = mask >> 1;
            };
        };
    };
}


void framebuffer_mz700_update_full_current_screen_row ( void ) {
    framebuffer_mz700_update_full_screen_row ( g_gdg.beam_row );
}


void framebuffer_mz700_update_all_rows ( void ) {
    int last_row = g_framebuffer_mz1500.video_canvas_first_row + ( FRAMEBUFFER_MZ700_CHARS_HEIGHT * 8 );
    int i;
    for ( i = g_framebuffer_mz1500.video_canvas_first_row; i < last_row; i++ ) {
        framebuffer_mz700_update_full_screen_row ( i );
    };
}

#endif
