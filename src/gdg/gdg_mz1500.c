/* 
 * File:   gdg_mz1500.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 3. března 2019, 14:36
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
 * Zapis na port 0xF1:
 * --------------------
 * 
 */

#include "mz800emu_cfg.h"

#include <stdio.h>
#include <string.h>
#include <glib.h>

#ifdef MACHINE_EMU_MZ1500

#include "main.h"

#include "z80ex/include/z80ex.h"

#include "gdg_mz1500.h"
#include "vramctrl.h"
#include "framebuffer.h"

#include "video.h"
#include "iface_sdl/iface_sdl.h"
#include "memory/memory.h"
#include "ctc8253/ctc8253.h"

//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"

st_GDG_MZ1500 g_gdg_mz1500;

/* Eventy musi byt serazeny vzestupne podle event_column ! */
struct st_GDGEVENT g_gdgevent_mz1500 [] = {

                                           /* row: ALL, col: 150 */
    { EVENT_GDG_HBLN_END, 0, VIDEO_SCREEN_HEIGHT, VIDEO_BEAM_CANVAS_FIRST_COLUMN - 4 },

                                           /* row: ALL, col: 790 */
                                           /* + row: 45, col: 790 - VBLN_END */
                                           /* + row: 245, col: 790 - VBLN_START */
    { EVENT_GDG_HBLN_START, 0, VIDEO_SCREEN_HEIGHT, VIDEO_BEAM_HBLN_FIRST_COLUMN },

                                           /* row: ALL, col: 1135 STS_HSYNC end (z duvodu uspory jej ukoncime malinko pozdeji) */
                                           /* row: ALL, col: 1135 */
    { EVENT_GDG_SCREEN_ROW_END, 0, VIDEO_SCREEN_HEIGHT, VIDEO_SCREEN_WIDTH },
};


void gdg_mz1500_reset ( void ) {
    g_gdg_mz1500.dmd = GDG_MZ1500_FLAG_DMODE_700;
}


void gdg_mz1500_init ( void ) {
    g_gdgevent = g_gdgevent_mz1500;
    memset ( &g_gdg_mz1500, 0x00, sizeof ( g_gdg_mz1500 ) );
    gdg_mz1500_reset ( );

    framebuffer_mz1500_init (
                              &g_memory_VRAM[0],
                              &g_memory_ROM[0x1000],
                              g_memory_mz1500_PCG1,
                              g_memory_mz1500_PCG2,
                              g_memory_mz1500_PCG3,
                              (uint8_t*) g_iface_sdl.active_surface->pixels,
                              FRAMEBUFFER_MZ1500_PIXEL_WIDTH,
                              FRAMEBUFFER_MZ1500_PIXEL_HEIGHT,
                              VIDEO_DISPLAY_WIDTH,
                              VIDEO_BEAM_CANVAS_FIRST_ROW,
                              VIDEO_BORDER_LEFT_WIDTH );

}

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED


void gdg_mz1500_write_byte_raw ( unsigned addr, Z80EX_BYTE value ) {
    if ( addr == 0xf0 ) {
        g_gdg_mz1500.dmd = value & 0x03;
        g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
    } else if ( addr == 0xf1 ) {
        int i = ( value >> 4 ) & 0x07;
        int color = value & 0x07;
        g_gdg_mz1500.mode1500_color[i] = color;
        framebuffer_mz1500_set_color ( i, color );
        g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
    } else if ( addr == 0xe008 ) {
        /* zapis na status registr 0xe008 v rezimu MZ-1500 */
        value = value & 0x01;
        if ( value != g_gdg.regct53g7 ) {
            g_gdg.regct53g7 = value;
            ctc8253_gate ( 0, value, gdg_get_insigeop_ticks ( ) );
        };
    };
}
#endif


void gdg_mz1500_write_byte ( unsigned addr, Z80EX_BYTE value ) {
    // TODO: zjistit, zda se zmeny projevi ihned a zda zapis podle stavu hbln vyvola WAIIT
    if ( addr == 0xf0 ) {
        mz800_sync_insideop_mreq_mz700_vramctrl ( );
        //printf ( "GDG WR (F0): 0x%02x, PC = 0x%04x\n", value, g_mz800.instruction_addr );
        g_gdg_mz1500.dmd = value & 0x03;
        g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
    } else if ( addr == 0xf1 ) {
        mz800_sync_insideop_mreq_mz700_vramctrl ( );
        int i = ( value >> 4 ) & 0x07;
        int color = value & 0x07;
        //printf ( "GDG WR (F1): 0x%02x, i=%d, c=%d, PC = 0x%04x\n", value, i, color, g_mz800.instruction_addr );
        g_gdg_mz1500.mode1500_color[i] = color;
        framebuffer_mz1500_set_color ( i, color );
        g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
    } else if ( addr == 0xe008 ) {
        /* zapis na status registr 0xe008 v rezimu MZ-1500 */
        value = value & 0x01;
        if ( value != g_gdg.regct53g7 ) {
            g_gdg.regct53g7 = value;
            ctc8253_gate ( 0, value, gdg_get_insigeop_ticks ( ) );
        };
    };
}


Z80EX_BYTE gdg_mz1500_read_dmd_status ( void ) {

    Z80EX_BYTE retval = 0x00;

    retval = GDG_SIGNAL_HBLNK ? 1 << 7 : 0x00;
    // bit 3, 4 - JOY B
    // bit 1, 2 - JOY A
    retval |= GDG_SIGNAL_TEMPO;
    return retval;
}

#endif
