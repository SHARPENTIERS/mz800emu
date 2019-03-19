/* 
 * File:   framebuffer_mz800.c
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

#include "mz800emu_cfg.h"

#include <string.h>
#include <stdint.h>

#ifdef MACHINE_EMU_MZ800

#include "z80ex/include/z80ex.h"

#include "framebuffer_mz800.h"
#include "gdg.h"
#include "hwscroll_mz800.h"
#include "video.h"
#include "memory/memory.h"

#include "iface_sdl/iface_sdl.h"

#define BIT0POS( val, pos ) ( ( val & 1 ) << pos )


/* 
 * 
 * Na konci viditelneho radku zaktualizujeme aktualni radek borderu ve framebufferu.
 * 
 */
void framebuffer_mz800_border_current_row_fill ( void ) {

    /* Pokud uz je radek ve framebufferu hotov, tak jdeme pryc */
    if ( g_gdg.last_updated_border_pixel == VIDEO_BEAM_DISPLAY_LAST_COLUMN + 1 ) return;

    unsigned beam_row = g_gdg.beam_row;
    uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + beam_row * VIDEO_DISPLAY_WIDTH;

    if ( ( beam_row < VIDEO_BORDER_TOP_HEIGHT ) || ( beam_row > VIDEO_BORDER_TOP_HEIGHT + VIDEO_CANVAS_HEIGHT - 1 ) ) {
        memset ( &p [ g_gdg.last_updated_border_pixel ], g_gdg_mz800.regBOR, VIDEO_DISPLAY_WIDTH - g_gdg.last_updated_border_pixel );
    } else {
        if ( g_gdg.last_updated_border_pixel < VIDEO_BORDER_LEFT_WIDTH ) {
            memset ( &p [ g_gdg.last_updated_border_pixel ], g_gdg_mz800.regBOR, VIDEO_BORDER_LEFT_WIDTH - g_gdg.last_updated_border_pixel );
        };
        unsigned pos_x = ( g_gdg.last_updated_border_pixel >= VIDEO_BORDER_LEFT_WIDTH + VIDEO_CANVAS_WIDTH ) ? g_gdg.last_updated_border_pixel : VIDEO_BORDER_LEFT_WIDTH + VIDEO_CANVAS_WIDTH;
        memset ( &p [ pos_x ], g_gdg_mz800.regBOR, VIDEO_DISPLAY_WIDTH - pos_x );
    };
}


/*
 * 
 *  Prave doslo ke zmene borderu. Nez provedeme zmenu, tak zaktualizujeme aktualni radek az po misto, kde nastala zmena.
 *
 */
void framebuffer_mz800_border_changed ( void ) {

    g_gdg.border_changes = SCRSTS_THIS_IS_CHANGED;

    unsigned beam_col = VIDEO_GET_SCREEN_COL ( g_gdg.total_elapsed.ticks );
    unsigned beam_row = VIDEO_GET_SCREEN_ROW ( g_gdg.total_elapsed.ticks );


    /* Osetreni situace, kdy OUT provedl zmenu na hranici mezi dvema snimky. */
    if ( beam_row == VIDEO_SCREEN_HEIGHT ) {
        beam_row = 0;
    };

    /* Pokud jsme na radku, ktery je mimo viditelnou oblast, tak neni potreba aktualizovat framebuffer. */
    if ( beam_row > VIDEO_BEAM_DISPLAY_LAST_ROW ) return;

    /* Pokud je tento radek ve framebufferu uz hotov, tak muzeme jit pryc. */
    if ( ( beam_row == g_gdg.beam_row ) && ( g_gdg.last_updated_border_pixel == VIDEO_BEAM_DISPLAY_LAST_COLUMN + 1 ) ) return;

    /* Pokud doslo ke zmene na zacatku framebufferu, tak neni co updatovat. */
    if ( beam_col == 0 ) return;

    /* Osetreni maximalni horni hranice paprsku na radku. */
    beam_col = ( beam_col > VIDEO_BEAM_DISPLAY_LAST_COLUMN ) ? ( VIDEO_BEAM_DISPLAY_LAST_COLUMN + 1 ) : beam_col;

    /* */
    if ( beam_row != g_gdg.beam_row ) {
        g_gdg.last_updated_border_pixel = 0;
    }
    /*
     *  Nyni provedeme aktualizaci framebufferu. 
     *  Ulozime stav od posledni aktualizace po nynejsi pozici - 1.
     *  Do last_updated ulozime pozici od ktere bude platit novy stav.
     *  Pokud do last_updated ulozime DISPLAY_VISIBLE_LAST_COLUMN + 1, tak to znamena, ze uz je cely radek hotovy.
     */
    uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + beam_row * VIDEO_DISPLAY_WIDTH;

    /* Jsme v hornim, nebo dolnim borderu? */
    if ( ( beam_row <= VIDEO_BEAM_BORDER_TOP_LAST_ROW ) || ( beam_row >= VIDEO_BEAM_BORDER_BOTOM_FIRST_ROW ) ) {
        memset ( &p [ g_gdg.last_updated_border_pixel ], g_gdg_mz800.regBOR, beam_col - g_gdg.last_updated_border_pixel );

        g_gdg.last_updated_border_pixel = beam_col;

    } else {

        /* Je potreba aktualizovat levy border? */
        if ( g_gdg.last_updated_border_pixel <= VIDEO_BEAM_BORDER_LEFT_LAST_COLUMN ) {

            unsigned last_pixel = ( beam_col < VIDEO_BEAM_BORDER_LEFT_LAST_COLUMN ) ? beam_col : VIDEO_BEAM_BORDER_LEFT_LAST_COLUMN + 1;
            memset ( &p [ g_gdg.last_updated_border_pixel ], g_gdg_mz800.regBOR, last_pixel - g_gdg.last_updated_border_pixel );

            g_gdg.last_updated_border_pixel = last_pixel;
        };

        /* Je potreba aktualizovat pravy border? */
        if ( beam_col >= VIDEO_BEAM_BORDER_RIGHT_FIRST_COLUMN ) {

            unsigned start_pixel = ( g_gdg.last_updated_border_pixel >= VIDEO_BEAM_BORDER_RIGHT_FIRST_COLUMN ) ? g_gdg.last_updated_border_pixel : VIDEO_BEAM_BORDER_RIGHT_FIRST_COLUMN;
            memset ( &p [ start_pixel ], g_gdg_mz800.regBOR, beam_col - start_pixel );

            g_gdg.last_updated_border_pixel = beam_col;
        };
    };
}


void framebuffer_mz800_mode800_screen_changed ( void ) {
    g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
    unsigned column = VIDEO_GET_SCREEN_COL ( g_gdg.total_elapsed.ticks );
    if ( ( column < VIDEO_BEAM_CANVAS_FIRST_COLUMN + 3 ) || ( column > VIDEO_BEAM_CANVAS_LAST_COLUMN ) || ( g_gdg.beam_row < VIDEO_BEAM_CANVAS_FIRST_ROW ) || ( g_gdg.beam_row > VIDEO_BEAM_CANVAS_LAST_ROW ) ) return;
    framebuffer_mz800_mode800_current_screen_row_fill ( column - VIDEO_BEAM_CANVAS_FIRST_COLUMN );
}


static inline unsigned framebuffer_mz800__mode800_screen_row_fill ( unsigned beam_row, unsigned last_pixel ) {
    static unsigned vram_data1 = 0;
    static unsigned vram_data2 = 0;
    static unsigned vram_data3 = 0;
    static unsigned vram_data4 = 0;

    unsigned pos_Y = beam_row - VIDEO_BEAM_CANVAS_FIRST_ROW;
    unsigned pos_X = g_gdg.screen_need_update_from;

    uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + ( ( beam_row * VIDEO_DISPLAY_WIDTH ) + VIDEO_BORDER_LEFT_WIDTH + pos_X ) * sizeof ( uint8_t );

    unsigned vram_addr;
    vram_addr = ( pos_X / 16 ) + ( pos_Y * 40 );
    if ( pos_X % 16 ) vram_addr++;

    unsigned plt_code;
    unsigned IGRB_output;

    /* TODO: z nepripojenych EXVRAM budeme cist 0xff */

    switch ( g_gdg_mz800.regDMD & 0x07 ) {

            /* 320x200 @ 4/A */
        case 0x00:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data1 = g_memory_mz800_VRAM_I [ real_vram_addr ];
                    vram_data2 = g_memory_mz800_VRAM_II [ real_vram_addr ];
                };

                plt_code = BIT0POS ( vram_data2, 1 ) | BIT0POS ( vram_data1, 0 );

                switch ( plt_code ) {
                    case 0:
                        IGRB_output = g_gdg_mz800.regPAL0;
                        break;
                    case 1:
                        IGRB_output = g_gdg_mz800.regPAL1;
                        break;
                    case 2:
                        IGRB_output = g_gdg_mz800.regPAL2;
                        break;
                    case 3:
                        IGRB_output = g_gdg_mz800.regPAL3;
                        break;
                };

                *p++ = IGRB_output;
                *p++ = IGRB_output;

                vram_data1 = vram_data1 >> 1;
                vram_data2 = vram_data2 >> 1;

                pos_X += 2;
            };
            break;


            /* 320x200 @ 4/B */
        case 0x01:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data3 = g_memory_mz800_VRAM_III [ real_vram_addr ];
                    vram_data4 = g_memory_mz800_VRAM_IV [ real_vram_addr ];
                };

                plt_code = BIT0POS ( vram_data4, 1 ) | BIT0POS ( vram_data3, 0 );

                switch ( plt_code ) {
                    case 0:
                        IGRB_output = g_gdg_mz800.regPAL0;
                        break;
                    case 1:
                        IGRB_output = g_gdg_mz800.regPAL1;
                        break;
                    case 2:
                        IGRB_output = g_gdg_mz800.regPAL2;
                        break;
                    case 3:
                        IGRB_output = g_gdg_mz800.regPAL3;
                        break;
                };

                *p++ = IGRB_output;
                *p++ = IGRB_output;

                vram_data3 = vram_data3 >> 1;
                vram_data4 = vram_data4 >> 1;

                pos_X += 2;
            };
            break;


            /* 320x200 @ 16 */
        case 0x02:
            while ( pos_X < last_pixel ) {
                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data1 = g_memory_mz800_VRAM_I [ real_vram_addr ];
                    vram_data2 = g_memory_mz800_VRAM_II [ real_vram_addr ];
                    vram_data3 = g_memory_mz800_VRAM_III [ real_vram_addr ];
                    vram_data4 = g_memory_mz800_VRAM_IV [ real_vram_addr ];
                };

                plt_code = BIT0POS ( vram_data4, 3 ) | BIT0POS ( vram_data3, 2 ) | BIT0POS ( vram_data2, 1 ) | BIT0POS ( vram_data1, 0 );

                if ( g_gdg_mz800.regPALGRP == ( ( plt_code >> 2 ) & 0x03 ) ) {
                    switch ( plt_code & 0x03 ) {
                        case 0:
                            IGRB_output = g_gdg_mz800.regPAL0;
                            break;
                        case 1:
                            IGRB_output = g_gdg_mz800.regPAL1;
                            break;
                        case 2:
                            IGRB_output = g_gdg_mz800.regPAL2;
                            break;
                        case 3:
                            IGRB_output = g_gdg_mz800.regPAL3;
                            break;
                    };
                } else {
                    IGRB_output = plt_code;
                };

                *p++ = IGRB_output;
                *p++ = IGRB_output;

                vram_data1 = vram_data1 >> 1;
                vram_data2 = vram_data2 >> 1;
                vram_data3 = vram_data3 >> 1;
                vram_data4 = vram_data4 >> 1;

                pos_X += 2;
            };
            break;


            /* 320x200 @ 16 undoc */
        case 0x03:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data1 = g_memory_mz800_VRAM_I [ real_vram_addr ];
                    vram_data2 = g_memory_mz800_VRAM_II [ real_vram_addr ];
                    vram_data3 = g_memory_mz800_VRAM_III [ real_vram_addr ];
                    vram_data4 = g_memory_mz800_VRAM_IV [ real_vram_addr ];
                };

                if ( g_gdg_mz800.regPALGRP == ( BIT0POS ( vram_data4, 1 ) | BIT0POS ( vram_data3, 0 ) ) ) {
                    plt_code = BIT0POS ( vram_data4, 3 ) | BIT0POS ( vram_data3, 2 ) | BIT0POS ( vram_data4, 1 ) | BIT0POS ( vram_data3, 0 );
                } else {
                    plt_code = BIT0POS ( vram_data4, 3 ) | BIT0POS ( vram_data3, 2 ) | BIT0POS ( vram_data2, 1 ) | BIT0POS ( vram_data1, 0 );
                };

                if ( g_gdg_mz800.regPALGRP == ( ( plt_code >> 2 ) & 0x03 ) ) {
                    switch ( plt_code & 0x03 ) {
                        case 0:
                            IGRB_output = g_gdg_mz800.regPAL0;
                            break;
                        case 1:
                            IGRB_output = g_gdg_mz800.regPAL1;
                            break;
                        case 2:
                            IGRB_output = g_gdg_mz800.regPAL2;
                            break;
                        case 3:
                            IGRB_output = g_gdg_mz800.regPAL3;
                            break;
                    };
                } else {
                    IGRB_output = plt_code;
                };

                *p++ = IGRB_output;
                *p++ = IGRB_output;

                vram_data1 = vram_data1 >> 1;
                vram_data2 = vram_data2 >> 1;
                vram_data3 = vram_data3 >> 1;
                vram_data4 = vram_data4 >> 1;

                pos_X += 2;
            };
            break;


            /* 640x200 @ 2/A */
        case 0x04:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data1 = g_memory_mz800_VRAM_I [ real_vram_addr ];
                    vram_data2 = g_memory_mz800_VRAM_II [ real_vram_addr ];
                };

                if ( 0x00 == ( pos_X & 0x08 ) ) {
                    /* sudy bajt */
                    plt_code = BIT0POS ( vram_data1, 0 );
                    vram_data1 = vram_data1 >> 1;
                } else {
                    /* lichy bajt */
                    plt_code = BIT0POS ( vram_data2, 0 );
                    vram_data2 = vram_data2 >> 1;
                };

                switch ( plt_code ) {
                    case 0:
                        IGRB_output = g_gdg_mz800.regPAL0;
                        break;
                    case 1:
                        IGRB_output = g_gdg_mz800.regPAL1;
                        break;
                };

                *p++ = IGRB_output;

                pos_X++;
            };
            break;


            /* 640x200 @ 2/B */
        case 0x05:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data3 = g_memory_mz800_VRAM_III [ real_vram_addr ];
                    vram_data4 = g_memory_mz800_VRAM_IV [ real_vram_addr ];
                };

                if ( 0x00 == ( pos_X & 0x08 ) ) {
                    /* sudy bajt */
                    plt_code = BIT0POS ( vram_data3, 0 );
                    vram_data3 = vram_data3 >> 1;
                } else {
                    /* lichy bajt */
                    plt_code = BIT0POS ( vram_data4, 0 );
                    vram_data4 = vram_data4 >> 1;
                };

                switch ( plt_code ) {
                    case 0:
                        IGRB_output = g_gdg_mz800.regPAL0;
                        break;
                    case 1:
                        IGRB_output = g_gdg_mz800.regPAL1;
                        break;
                };

                *p++ = IGRB_output;

                pos_X++;
            };
            break;


            /* 640x200 @ 4 */
        case 0x06:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data1 = g_memory_mz800_VRAM_I [ real_vram_addr ];
                    vram_data2 = g_memory_mz800_VRAM_II [ real_vram_addr ];
                    vram_data3 = g_memory_mz800_VRAM_III [ real_vram_addr ];
                    vram_data4 = g_memory_mz800_VRAM_IV [ real_vram_addr ];
                };

                if ( 0x00 == ( pos_X & 0x08 ) ) {
                    /* sudy bajt */
                    plt_code = BIT0POS ( vram_data3, 1 ) | BIT0POS ( vram_data1, 0 );
                    vram_data1 = vram_data1 >> 1;
                    vram_data3 = vram_data3 >> 1;
                } else {
                    /* lichy bajt */
                    plt_code = BIT0POS ( vram_data4, 1 ) | BIT0POS ( vram_data2, 0 );
                    vram_data2 = vram_data2 >> 1;
                    vram_data4 = vram_data4 >> 1;
                };

                switch ( plt_code ) {
                    case 0:
                        IGRB_output = g_gdg_mz800.regPAL0;
                        break;
                    case 1:
                        IGRB_output = g_gdg_mz800.regPAL1;
                        break;
                    case 2:
                        IGRB_output = g_gdg_mz800.regPAL2;
                        break;
                    case 3:
                        IGRB_output = g_gdg_mz800.regPAL3;
                        break;
                };

                *p++ = IGRB_output;

                pos_X++;
            };
            break;


            /* 640x200 @ 4 undoc */
        case 0x07:
            while ( pos_X < last_pixel ) {

                if ( 0 == ( pos_X & 0x0f ) ) {
                    unsigned real_vram_addr = hwscroll_mz800_shift_addr ( vram_addr++ );
                    vram_data3 = g_memory_mz800_VRAM_III [ real_vram_addr ];
                    vram_data4 = g_memory_mz800_VRAM_IV [ real_vram_addr ];
                };

                if ( 0x00 == ( pos_X & 0x08 ) ) {
                    /* sudy bajt */
                    plt_code = BIT0POS ( vram_data3, 1 ) | BIT0POS ( vram_data3, 0 );
                    vram_data3 = vram_data3 >> 1;
                } else {
                    /* lichy bajt */
                    plt_code = BIT0POS ( vram_data4, 1 ) | BIT0POS ( vram_data4, 0 );
                    vram_data4 = vram_data4 >> 1;
                };

                switch ( plt_code ) {
                    case 0:
                        IGRB_output = g_gdg_mz800.regPAL0;
                        break;
                    case 1:
                        IGRB_output = g_gdg_mz800.regPAL1;
                        break;
                    case 2:
                        IGRB_output = g_gdg_mz800.regPAL2;
                        break;
                    case 3:
                        IGRB_output = g_gdg_mz800.regPAL3;
                        break;
                };

                *p++ = IGRB_output;

                pos_X++;
            };
            break;
    };
    return pos_X;

}


void framebuffer_mz800_mode800_current_screen_row_fill ( unsigned last_pixel ) {
    last_pixel -= last_pixel % 2;
    if ( g_gdg.screen_need_update_from >= last_pixel ) return;
    unsigned beam_row = g_gdg.beam_row;
    g_gdg.screen_need_update_from = framebuffer_mz800__mode800_screen_row_fill ( beam_row, last_pixel );
}


void framebuffer_mz800_mode800_all_screen_rows_fill ( void ) {
    int i;
    g_gdg.screen_need_update_from = 0;
    for ( i = VIDEO_BEAM_CANVAS_FIRST_ROW; i <= VIDEO_BEAM_CANVAS_LAST_ROW; i++ ) {
        framebuffer_mz800__mode800_screen_row_fill ( i, VIDEO_CANVAS_WIDTH );
    };
}


void framebuffer_mz800_border_all_rows_fill ( void ) {

    int i;

    for ( i = 0; i < VIDEO_BORDER_TOP_HEIGHT; i++ ) {
        uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + ( i * VIDEO_DISPLAY_WIDTH );
        memset ( p, g_gdg_mz800.regBOR, VIDEO_DISPLAY_WIDTH );
    };

    for ( i = VIDEO_BORDER_TOP_HEIGHT; i < ( VIDEO_BORDER_TOP_HEIGHT + VIDEO_BORDER_LEFT_HEIGHT ); i++ ) {
        uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + ( i * VIDEO_DISPLAY_WIDTH );
        memset ( p, g_gdg_mz800.regBOR, VIDEO_BORDER_LEFT_WIDTH );
    };

    for ( i = VIDEO_BORDER_TOP_HEIGHT; i < ( VIDEO_BORDER_TOP_HEIGHT + VIDEO_BORDER_RIGHT_HEIGHT ); i++ ) {
        uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + ( i * VIDEO_DISPLAY_WIDTH ) + ( VIDEO_BORDER_LEFT_WIDTH + VIDEO_CANVAS_WIDTH );
        memset ( p, g_gdg_mz800.regBOR, VIDEO_BORDER_RIGHT_WIDTH );
    };

    for ( i = ( VIDEO_BORDER_TOP_HEIGHT + VIDEO_BORDER_RIGHT_HEIGHT ); i < VIDEO_DISPLAY_HEIGHT; i++ ) {
        uint8_t *p = (uint8_t*) g_iface_sdl.active_surface->pixels + ( i * VIDEO_DISPLAY_WIDTH );
        memset ( p, g_gdg_mz800.regBOR, VIDEO_DISPLAY_WIDTH );
    };
}
#endif
