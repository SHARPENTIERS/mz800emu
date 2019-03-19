/* 
 * File:   framebuffer_mz1500.c
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
 * Zapis na port 0xF0:
 * --------------------
 * 
 * 0. bit - MODE
 * 
 *      0 - rezim 700
 *      1 - rezim 1500
 * 
 * 1. bit - v DRAW_PRIORITY (plati jen v rezimu 1500)
 * 
 *      0 - BPF (pozadi, PCG, popredi)
 *      1 - BFP (pozadi, popredi, PCG)
 * 
 * VRAM a seskladani obrazu:
 * -------------------------
 * 
 * Rezim kompatibilni s MZ-700:
 * 
 * 0xD000 - 0xD320: Znakova VRAM, kod znaku z CGROM
 * 0xD800 - 0xDB20: Atributova VRAM
 * 
 * Rezim specificky pro MZ-1500:
 * 
 * 0xD000 - 0xD320: Znakova VRAM, kod znaku z CGROM
 * 0xD400 - 0xD720: Znakova VRAM, kod znaku z PCG (dolnich 8 bitu )
 * 0xD800 - 0xDB20: Atributova VRAM
 * 0xDC00 - 0xDF20: Znakova VRAM: 
 *                      bity 6. - 7. kod znaku z PCG ( horni 2 bity )
 *                      bit 3. PCG_ENABLE
 * 
 * 
 */

#include "mz800emu_cfg.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <glib.h>

#ifdef MACHINE_EMU_MZ1500

#include "z80ex/include/z80ex.h"

#include "framebuffer_mz1500.h"
#include "gdg.h"

#include "mz800.h"


#define MZ1500_ATTRPCG_ENABLED  0x08


typedef struct st_FRAMEBUFFER_MZ1500 {
    Z80EX_BYTE *VRAM; // prostor CHAR_VRAM a ATTR_VRAM
    Z80EX_BYTE *CG; // prostor s aktivnimi maskami CGROM | CGRAM

    Z80EX_BYTE *PCG0; // prostor PCG
    Z80EX_BYTE *PCG1; // prostor PCG
    Z80EX_BYTE *PCG2; // prostor PCG

    uint8_t *PX; // 8 bitovy pixels surface (1px = 1 bajt)
    int one_pixel_width; // sirka jednoho pixelu
    int one_pixel_height; // vyska jednoho pixelu

    int video_canvas_first_row;
    int video_border_left_width;
    int video_display_width; // pocet pixelu na jeden cely obrazovy radek

    uint8_t colormap[8];
} st_FRAMEBUFFER_MZ1500;

st_FRAMEBUFFER_MZ1500 g_framebuffer_mz1500;


void framebuffer_mz1500_init ( Z80EX_BYTE *vram, Z80EX_BYTE *cg, Z80EX_BYTE *pcg0, Z80EX_BYTE *pcg1, Z80EX_BYTE *pcg2, uint8_t *px, int px_width, int px_height, int display_width, int canvas_first_row, int left_border_width ) {
    g_framebuffer_mz1500.VRAM = vram;
    g_framebuffer_mz1500.CG = cg;

    g_framebuffer_mz1500.PCG0 = pcg0;
    g_framebuffer_mz1500.PCG1 = pcg1;
    g_framebuffer_mz1500.PCG2 = pcg2;

    g_framebuffer_mz1500.PX = px;
    g_framebuffer_mz1500.one_pixel_width = px_width;
    g_framebuffer_mz1500.one_pixel_height = px_height;

    g_framebuffer_mz1500.video_display_width = display_width;
    g_framebuffer_mz1500.video_canvas_first_row = canvas_first_row;
    g_framebuffer_mz1500.video_border_left_width = left_border_width;

    memset ( &g_framebuffer_mz1500.colormap, 0x00, sizeof ( g_framebuffer_mz1500.colormap ) );
}


void framebuffer_mz1500_set_vram ( Z80EX_BYTE *vram ) {
    g_framebuffer_mz1500.VRAM = vram;
}


void framebuffer_mz1500_set_cg ( Z80EX_BYTE *cg ) {
    g_framebuffer_mz1500.CG = cg;
}


void framebuffer_mz1500_set_pcg ( Z80EX_BYTE *pcg0, Z80EX_BYTE *pcg1, Z80EX_BYTE *pcg2 ) {
    g_framebuffer_mz1500.PCG0 = pcg0;
    g_framebuffer_mz1500.PCG0 = pcg1;
    g_framebuffer_mz1500.PCG0 = pcg2;
}


void framebuffer_mz1500_set_pixels ( uint8_t *px, int px_width, int px_height ) {
    g_framebuffer_mz1500.PX = px;
    g_framebuffer_mz1500.one_pixel_width = px_width;
    g_framebuffer_mz1500.one_pixel_height = px_height;
}


void framebuffer_mz1500_set_video ( int display_width, int canvas_first_row, int left_border_width ) {
    g_framebuffer_mz1500.video_display_width = display_width;
    g_framebuffer_mz1500.video_canvas_first_row = canvas_first_row;
    g_framebuffer_mz1500.video_border_left_width = left_border_width;
}


void framebuffer_mz1500_set_color ( int pal, int color ) {
    g_framebuffer_mz1500.colormap[pal] = c_MZ1500_COLORMAP[color];
}


/**
 * Vykresleni obrazoveho radku platna (CANVAS) v rezimu MZ1500.
 * 
 */
static inline void framebuffer_mz1500_update_full_screen_row ( int row ) {

    uint8_t PCG_COLORMAP[8];
    int i;
    for ( i = 0; i < G_N_ELEMENTS ( PCG_COLORMAP ); i++ ) {
        PCG_COLORMAP[i] = c_MZ1500_COLORMAP[g_gdg_mz1500.mode1500_color[i]];
    };

    int canvas_row = row - g_framebuffer_mz1500.video_canvas_first_row;
    int vram_position = ( canvas_row / 8 ) * FRAMEBUFFER_MZ1500_CHARS_WIDTH;

    Z80EX_BYTE *CHAR_VRAM = &g_framebuffer_mz1500.VRAM[vram_position];
    Z80EX_BYTE *ATTR_VRAM = &g_framebuffer_mz1500.VRAM[( FRAMEBUFFER_MZ1500_ATTR_VRAM + vram_position )];
    Z80EX_BYTE *ATTR_PCGLO_VRAM = &g_framebuffer_mz1500.VRAM[( FRAMEBUFFER_MZ1500_ATTR_PCGLO + vram_position )];
    Z80EX_BYTE *ATTR_PCGHI_VRAM = &g_framebuffer_mz1500.VRAM[( FRAMEBUFFER_MZ1500_ATTR_PCGHI + vram_position )];

    int px_position = ( row * ( g_framebuffer_mz1500.video_display_width * g_framebuffer_mz1500.one_pixel_height ) ) + g_framebuffer_mz1500.video_border_left_width;
    uint8_t *ROW_PX = &g_framebuffer_mz1500.PX[px_position];

    int px_height;
    for ( px_height = 0; px_height < g_framebuffer_mz1500.one_pixel_height; px_height++ ) {

        Z80EX_BYTE *char_p = CHAR_VRAM;
        Z80EX_BYTE *attr_p = ATTR_VRAM;
        Z80EX_BYTE *attr_pcglo_p = ATTR_PCGLO_VRAM;
        Z80EX_BYTE *attr_pcghi_p = ATTR_PCGHI_VRAM;
        uint8_t *px_p = &ROW_PX[px_height * g_framebuffer_mz1500.video_display_width];

        int char_x;
        for ( char_x = 0; char_x < FRAMEBUFFER_MZ1500_CHARS_WIDTH; char_x++ ) {

            int attr_fgc = ( *attr_p >> 4 ) & 0x07;
            int attr_bgc = *attr_p & 0x07;
            int attr_grp = ( ( *attr_p << 4 ) & 0x0800 );

            int cg_addr = attr_grp | ( *char_p * 8 );

            int mask = g_framebuffer_mz1500.CG[( cg_addr + canvas_row % 8 )];

            if ( *attr_pcghi_p & MZ1500_ATTRPCG_ENABLED ) {
                // PCG na teto pozici je povoleno

                int pcg_addr = ( ( ( *attr_pcghi_p & 0xc0 ) << 2 ) | *attr_pcglo_p ) * 8;

                int pcg0_mask = g_framebuffer_mz1500.PCG0[( pcg_addr + canvas_row % 8 )];
                int pcg1_mask = g_framebuffer_mz1500.PCG1[( pcg_addr + canvas_row % 8 )];
                int pcg2_mask = g_framebuffer_mz1500.PCG2[( pcg_addr + canvas_row % 8 )];

                int mask_bit;
                for ( mask_bit = 0; mask_bit < 8; mask_bit++ ) {

                    uint8_t value = ( mask & 1 ) ? g_framebuffer_mz1500.colormap[attr_fgc] : g_framebuffer_mz1500.colormap[attr_bgc];

                    int pcg_color_id;
                    pcg_color_id = ( pcg0_mask & 0x80 ) >> 7;
                    pcg_color_id |= ( pcg1_mask & 0x80 ) >> 6;
                    pcg_color_id |= ( pcg2_mask & 0x80 ) >> 5;

                    int px_width;
                    uint8_t *px_p1 = px_p;

                    if ( GDG_MZ1500_TEST_PMODE_BFP ) {
                        for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                            *px_p++ = value;
                        };
                        if ( pcg_color_id != 0 ) {
                            uint8_t value_pcg = PCG_COLORMAP[pcg_color_id];
                            for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                                *px_p1++ = value_pcg;
                            };
                        }
                    } else {
                        uint8_t value_pcg = PCG_COLORMAP[pcg_color_id];
                        for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                            *px_p++ = value_pcg;
                        };
                        if ( mask & 1 ) {
                            for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                                *px_p1++ = value;
                            };
                        };
                    };

                    mask = mask >> 1;
                    pcg0_mask = pcg0_mask << 1;
                    pcg1_mask = pcg1_mask << 1;
                    pcg2_mask = pcg2_mask << 1;
                };

            } else {
                // PCG na teto pozici neni povoleno - zobrazujeme jako v mode700
                int mask_bit;
                for ( mask_bit = 0; mask_bit < 8; mask_bit++ ) {
                    uint8_t value = ( mask & 1 ) ? g_framebuffer_mz1500.colormap[attr_fgc] : g_framebuffer_mz1500.colormap[attr_bgc];
                    int px_width;
                    for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                        *px_p++ = value;
                    };
                    mask = mask >> 1;
                };
            };

            char_p++;
            attr_p++;
            attr_pcglo_p++;
            attr_pcghi_p++;
        };
    };
}


/**
 * Vykresleni obrazoveho radku platna (CANVAS) v rezimu MZ700.
 * 
 */
static inline void framebuffer_mz1500_mode700_update_full_screen_row ( int row ) {

    int canvas_row = row - g_framebuffer_mz1500.video_canvas_first_row;
    int vram_position = ( canvas_row / 8 ) * FRAMEBUFFER_MZ1500_CHARS_WIDTH;

    Z80EX_BYTE *CHAR_VRAM = &g_framebuffer_mz1500.VRAM[vram_position];
    Z80EX_BYTE *ATTR_VRAM = &g_framebuffer_mz1500.VRAM[( FRAMEBUFFER_MZ1500_ATTR_VRAM + vram_position )];

    int px_position = ( row * ( g_framebuffer_mz1500.video_display_width * g_framebuffer_mz1500.one_pixel_height ) ) + g_framebuffer_mz1500.video_border_left_width;
    uint8_t *ROW_PX = &g_framebuffer_mz1500.PX[px_position];

    int px_height;
    for ( px_height = 0; px_height < g_framebuffer_mz1500.one_pixel_height; px_height++ ) {

        Z80EX_BYTE *char_p = CHAR_VRAM;
        Z80EX_BYTE *attr_p = ATTR_VRAM;
        uint8_t *px_p = &ROW_PX[px_height * g_framebuffer_mz1500.video_display_width];

        int char_x;
        for ( char_x = 0; char_x < FRAMEBUFFER_MZ1500_CHARS_WIDTH; char_x++ ) {

            int attr_fgc = ( *attr_p >> 4 ) & 0x07;
            int attr_bgc = *attr_p & 0x07;
            int attr_grp = ( ( *attr_p << 4 ) & 0x0800 );

            int cg_addr = attr_grp | ( *char_p * 8 );

            int mask = g_framebuffer_mz1500.CG[( cg_addr + canvas_row % 8 )];

            char_p++;
            attr_p++;

            int mask_bit;
            for ( mask_bit = 0; mask_bit < 8; mask_bit++ ) {
                uint8_t value = ( mask & 1 ) ? g_framebuffer_mz1500.colormap[attr_fgc] : g_framebuffer_mz1500.colormap[attr_bgc];
                int px_width;
                for ( px_width = 0; px_width < g_framebuffer_mz1500.one_pixel_width; px_width++ ) {
                    *px_p++ = value;
                };
                mask = mask >> 1;
            };
        };
    };
}


void framebuffer_mz1500_update_full_current_screen_row ( void ) {
    if ( GDG_MZ1500_TEST_MODE700 ) {
        framebuffer_mz1500_mode700_update_full_screen_row ( g_gdg.beam_row );
    } else {
        framebuffer_mz1500_update_full_screen_row ( g_gdg.beam_row );
    };
}


void framebuffer_mz1500_update_all_rows ( void ) {
    int last_row = g_framebuffer_mz1500.video_canvas_first_row + ( FRAMEBUFFER_MZ1500_CHARS_HEIGHT * 8 );
    int i;
    if ( GDG_MZ1500_TEST_MODE700 ) {
        for ( i = g_framebuffer_mz1500.video_canvas_first_row; i < last_row; i++ ) {
            framebuffer_mz1500_mode700_update_full_screen_row ( i );
        };
    } else {
        for ( i = g_framebuffer_mz1500.video_canvas_first_row; i < last_row; i++ ) {
            framebuffer_mz1500_update_full_screen_row ( i );
        };
    };
}

#endif
