/* 
 * File:   gdg.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 18:32
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
 *
 *
 *	Registry:
 *
 *		DMD (0xce):
 *			3. bit	- 0 = MZ800, 1 = MZ700
 *			2. bit	- jen MZ800: 0 = 320x200, 1 = 640x200
 *			1. bit	- jen MZ800: 0 = LOW_COLOR, 1 = HI_COLOR (extVRAM)
 *			0. bit	- jen MZ800: 0 = VBANK_A, 1 = VBANK_B (extVRAM)
 *
 *
 *		BOR (0x06cf):
 *			0. - 3. bit	- IGRB barva borderu
 *
 *
 *		PAL (0xf0):
 *			6. bit	- 0 = predvolba barvy v palete
 *					4. az 5. bit - kombinace rovin (cislo barvy): 0 - 3
 *					0. az 3. bit - IGRB barva
 *
 *				  1 = volba aktualni skupiny (PALGRP)
 *					0. - 1. bit - cislo skupiny: 0 - 3
 *
 */

#include "mz800emu_cfg.h"

#include <stdio.h>

#ifdef MACHINE_EMU_MZ800

#include "main.h"

#include "z80ex/include/z80ex.h"

#include "gdg_mz800.h"

#include "vramctrl.h"
#include "framebuffer.h"
#include "hwscroll_mz800.h"

#include "video.h"
#include "iface_sdl/iface_sdl.h"
#include "memory/memory.h"
#include "ctc8253/ctc8253.h"

#include "mz800.h"



//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


st_GDG_MZ800 g_gdg_mz800;


/* Eventy musi byt serazeny vzestupne podle event_column ! */
struct st_GDGEVENT g_gdgevent_mz800 [] = {

                                          /* row: ALL, col: 150 */
    { EVENT_GDG_HBLN_END, 0, VIDEO_SCREEN_HEIGHT, VIDEO_BEAM_CANVAS_FIRST_COLUMN - 4 },


                                          /* row: ALL, col: 790 */
                                          /* + row: 45, col: 790 - VBLN_END */
                                          /* + row: 245, col: 790 - VBLN_START */
    { EVENT_GDG_HBLN_START, 0, VIDEO_SCREEN_HEIGHT, VIDEO_BEAM_HBLN_FIRST_COLUMN },

#ifdef MACHINE_EMU_MZ800
    /* row: 0, col: 792 */
    { EVENT_GDG_STS_VSYNC_END, 0, 1, VIDEO_BORDER_LEFT_WIDTH + VIDEO_CANVAS_WIDTH - 2 },

                                          /* row: 287, col: 792 */
    { EVENT_GDG_STS_VSYNC_START, VIDEO_DISPLAY_HEIGHT - 1, 1, VIDEO_BORDER_LEFT_WIDTH + VIDEO_CANVAS_WIDTH - 2 },

                                          /* row: 46 - 245, col: 794 */
    { EVENT_GDG_AFTER_LAST_SCREEN_PIXEL, VIDEO_BEAM_CANVAS_FIRST_ROW, VIDEO_CANVAS_HEIGHT, VIDEO_BEAM_BORDER_RIGHT_FIRST_COLUMN },
#endif

                                          /* row: ALL, col: 928 - STS_HSYNC start (z duvodu uspory ho spustime trochu drive ) */
                                          /* row: 0 - 287, col: 928 */
                                          //{ EVENT_GDG_AFTER_LAST_VISIBLE_PIXEL, 0, DISPLAY_VISIBLE_HEIGHT, DISPLAY_VISIBLE_LAST_COLUMN + 1 },
    { EVENT_GDG_AFTER_LAST_VISIBLE_PIXEL, 0, VIDEO_SCREEN_HEIGHT, VIDEO_BEAM_DISPLAY_LAST_COLUMN + 1 },

                                          /* row: ALL, col: 926 - podle mych mereni zacina zde */
                                          //{ EVENT_GDG_STS_HSYNC_START, 0, BEAM_TOTAL_ROWS, 926 },

                                          /* row: ALL, col: 950 - realny HSYNC ma delku 80 px, jeho konec nas ale nezajima */
    { EVENT_GDG_REAL_HSYNC_START, 0, VIDEO_SCREEN_HEIGHT, 950 },

                                          /* row: ALL, col: 1133 - podle mych mereni konci zde */
                                          //{ EVENT_GDG_STS_HSYNC_END, 0, BEAM_TOTAL_ROWS, 1133 },

                                          /* row: ALL, col: 1135 STS_HSYNC end (z duvodu uspory jej ukoncime malinko pozdeji) */
                                          /* row: ALL, col: 1135 */
    { EVENT_GDG_SCREEN_ROW_END, 0, VIDEO_SCREEN_HEIGHT, VIDEO_SCREEN_WIDTH },
};


void gdg_mz800_init ( void ) {
    g_gdgevent = g_gdgevent_mz800;
    hwscroll_mz800_init ( );
    g_vramctrl_mz800.mode700_wr_latch_is_used = 0;
    framebuffer_mz700_init (
                             &g_memory_VRAM[0x1000],
                             &g_memory_VRAM[0],
                             (uint8_t*) g_iface_sdl.active_surface->pixels,
                             FRAMEBUFFER_MZ700_PIXEL_WIDTH,
                             FRAMEBUFFER_MZ700_PIXEL_HEIGHT,
                             VIDEO_DISPLAY_WIDTH,
                             VIDEO_BEAM_CANVAS_FIRST_ROW,
                             VIDEO_BORDER_LEFT_WIDTH );
}


void gdg_mz800_reset ( void ) {

    mz800_set_display_mode ( GDG_MZ800_DMD_FLAG_MODE700, 0 );

    if ( g_gdg_mz800.regBOR != 0 ) {
        g_gdg.border_changes = SCRSTS_THIS_IS_CHANGED;
    };
    g_gdg_mz800.regBOR = 0;

    g_gdg_mz800.regPALGRP = 0;
    g_gdg_mz800.regPAL0 = 0x09;
    g_gdg_mz800.regPAL1 = 0x0f;
    g_gdg_mz800.regPAL2 = 0x09;
    g_gdg_mz800.regPAL3 = 0x0f;

    vramctrl_mz800_reset ( );
    hwscroll_mz800_reset ( );
}


Z80EX_BYTE gdg_mz800_read_dmd_status ( void ) {

    Z80EX_BYTE retval;

    /* TODO: zkontrolovat, zda tam jsou stejne bity pri cteni 0xE008 i 0xCE */
    retval = GDG_SIGNAL_HBLNK ? 1 << 7 : 0x00;
    retval |= GDG_SIGNAL_VBLNK ? 1 << 6 : 0x00;
    retval |= GDG_SIGNAL_STS_HS ? 1 << 5 : 0x00;
    retval |= GDG_SIGNAL_STS_VS ? 1 << 4 : 0x00;
    retval |= ( g_mz800.mz800_switch ) ? 1 << 1 : 0x00;
    retval |= GDG_SIGNAL_TEMPO;
    //    printf ( "read DMD sts = 0x%02x - HB: %d, VB: %d, HS: %d, VS: %d, row: %d, col: %d, PC: 0x%04x\n", retval, SIGNAL_GDG_HBLNK, SIGNAL_GDG_VBLNK, SIGNAL_GDG_STS_HS, SIGNAL_GDG_STS_VS, BEAM_ROW ( g_gdg.screen_ticks_elapsed ), BEAM_COL ( g_gdg.screen_ticks_elapsed ), z80ex_get_reg ( g_mz800.cpu, regPC )  );
    return retval;
}


void gdg_mz800_write_byte ( unsigned addr, Z80EX_BYTE value ) {

    /* vramm controller: 0xcc, 0xcd */
    if ( 0xcc == ( addr & 0xfe ) ) {
        vramctrl_mz800_set_wf_rf_reg ( addr & 0x01, value );
        return;
    };

    unsigned addr_msb = ( addr >> 8 ) & 0xff;

    switch ( addr & 0xff ) {

        case 0x08:
            /* zapis na status registr 0xe008 v rezimu MZ-700 */
            value = value & 0x01;
            if ( value != g_gdg.regct53g7 ) {
                g_gdg.regct53g7 = value;
                ctc8253_gate ( 0, value, gdg_get_insigeop_ticks ( ) );
            };
            break;

            /* regDMD */
        case 0xce:

            value = value & 0x0f;

            if ( g_gdg_mz800.regDMD != value ) {

                if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                    /* 
                     * TODO: pri zmenach rezimu 700 / 800 a naopak je potreba osetrit framebuffer. 
                     * MZ700 -> MZ800 - do mista zmeny ponechat MZ700 obsah, zbytek updatovat standardne v MZ800
                     * MZ800 -> MZ700 - udelat update jen do zmeny rezimu, zbytek vygenerovat v 700
                     * 
                     */
                    framebuffer_mz800_mode800_screen_changed ( );
                } else {
                    g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
                };
                mz800_set_display_mode ( value, gdg_get_insigeop_ticks ( ) );

                /*
                            DEBUGGER_MMAP_FULL_UPDATE ( );
                 */
            };
            break;


        case 0xcf:

            /* HW scroll: 0x01cf - 0x05cf */
            if ( ( addr_msb != 0 ) && ( addr_msb < 6 ) ) {

                hwscroll_mz800_set_reg ( addr_msb, value );

                /* BORDER: 0x06cf */
            } else if ( addr_msb == 6 ) {
                value = value & 0x0f;

                if ( g_gdg_mz800.regBOR != value ) {
                    //printf ( "BORDER: 0x%02x, screen: %d, ticks: %d\n", value, g_gdg.total_elapsed.screens, mz800_get_instruction_start_ticks ( ) );
                    framebuffer_mz800_border_changed ( );
                    g_gdg_mz800.regBOR = value;
                };
            };
            break;


        case 0xf0:

            /* nastaveni PALGRP */
            if ( value & 0x40 ) {

                if ( g_gdg_mz800.regPALGRP != ( value & 0x03 ) ) {
                    if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                        framebuffer_mz800_mode800_screen_changed ( );
                    };
                    g_gdg_mz800.regPALGRP = value & 0x03;
                };

                /* nastaveni PAL */
            } else {

                unsigned framebuffer_updated = 0;

                unsigned pal_value = value & 0x0f;

                switch ( ( value & 0x30 ) >> 4 ) {

                    case 0:
                        if ( ( GDG_MZ800_DMD_FLAG_SCRW640 | GDG_MZ800_DMD_FLAG_HICOLOR | GDG_MZ800_DMD_FLAG_VBANK ) == ( g_gdg_mz800.regDMD & ( GDG_MZ800_DMD_FLAG_SCRW640 | GDG_MZ800_DMD_FLAG_HICOLOR | GDG_MZ800_DMD_FLAG_VBANK ) ) ) {
                            /* undoc mode! */
                            if ( g_gdg_mz800.regPAL1 != pal_value ) {
                                if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                                    framebuffer_mz800_mode800_screen_changed ( );
                                };
                                g_gdg_mz800.regPAL1 = pal_value;
                                framebuffer_updated = 1;
                            };
                        };
                        if ( g_gdg_mz800.regPAL0 != pal_value ) {
                            if ( framebuffer_updated == 0 ) {
                                if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                                    framebuffer_mz800_mode800_screen_changed ( );
                                };
                            };
                            g_gdg_mz800.regPAL0 = pal_value;
                        };
                        break;

                    case 1:
                        if ( g_gdg_mz800.regPAL1 != pal_value ) {
                            if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                                framebuffer_mz800_mode800_screen_changed ( );
                            };
                            g_gdg_mz800.regPAL1 = pal_value;
                        };
                        break;

                    case 2:
                        if ( g_gdg_mz800.regPAL2 != pal_value ) {
                            if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                                framebuffer_mz800_mode800_screen_changed ( );
                            };
                            g_gdg_mz800.regPAL2 = pal_value;
                        };
                        break;

                    case 3:
                        if ( ( GDG_MZ800_DMD_FLAG_SCRW640 | GDG_MZ800_DMD_FLAG_HICOLOR | GDG_MZ800_DMD_FLAG_VBANK ) == ( g_gdg_mz800.regDMD & ( GDG_MZ800_DMD_FLAG_SCRW640 | GDG_MZ800_DMD_FLAG_HICOLOR | GDG_MZ800_DMD_FLAG_VBANK ) ) ) {
                            if ( g_gdg_mz800.regPAL2 != pal_value ) {
                                if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                                    framebuffer_mz800_mode800_screen_changed ( );
                                };
                                g_gdg_mz800.regPAL2 = pal_value;
                                framebuffer_updated = 1;
                            };
                        };
                        if ( g_gdg_mz800.regPAL3 != pal_value ) {
                            if ( framebuffer_updated == 0 ) {
                                if ( !GDG_MZ800_TEST_DMD_MODE700 ) {
                                    framebuffer_mz800_mode800_screen_changed ( );
                                };
                            };
                            g_gdg_mz800.regPAL3 = pal_value;
                        };
                        break;
                };
            };
            break;
    };
}

#endif
