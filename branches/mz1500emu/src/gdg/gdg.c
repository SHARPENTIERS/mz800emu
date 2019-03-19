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

#include "mz800emu_cfg.h"

#include <stdio.h>

#include "gdg.h"

st_GDG g_gdg;

struct st_GDGEVENT *g_gdgevent = NULL;


void gdg_init ( void ) {

#ifdef MACHINE_EMU_MZ800
    gdg_mz800_init ( );
#endif
#ifdef MACHINE_EMU_MZ1500
    gdg_mz1500_init ( );
#endif

    g_gdg.total_elapsed.ticks = 0;
    g_gdg.total_elapsed.screens = 0;

    g_gdg.event.event_name = 0;
    g_gdg.event.ticks = g_gdgevent[g_gdg.event.event_name].event_column;

    g_gdg.hbln = HBLN_ACTIVE;
    g_gdg.vbln = VBLN_ACTIVE;


#ifdef MACHINE_EMU_MZ800
    g_gdg.sts_hsync = HSYN_OFF;
    g_gdg.sts_vsync = VSYN_ACTIVE;
#endif

    g_gdg.beam_row = 0;
    g_gdg.screen_is_already_rendered_at_beam_pos = 0;

    g_gdg.screen_need_update_from = 0;
    g_gdg.last_updated_border_pixel = 0;

    g_gdg.framebuffer_state = FB_STATE_NOT_CHANGED;
    g_gdg.screen_changes = SCRSTS_IS_NOT_CHANGED;
    g_gdg.border_changes = SCRSTS_IS_NOT_CHANGED;

    g_gdg.tempo_divider = 0;
    g_gdg.tempo = 0;

    g_gdg.regct53g7 = 0;

#ifdef MZ800EMU_CFG_CLK1M1_SLOW
    g_gdg.ctc0clk = 0;
#endif
}


void gdg_reset ( void ) {

    g_gdg.regct53g7 = 0;

#ifdef MACHINE_EMU_MZ800
    gdg_mz800_reset ( );
#endif
#ifdef MACHINE_EMU_MZ1500
    gdg_mz1500_reset ( );
#endif

    g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;
}
