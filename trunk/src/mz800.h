/* 
 * File:   mz800.h
 * Author: chaky
 *
 * Created on 14. června 2015, 16:16
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

#ifndef MZ800_H
#define MZ800_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#include "z80ex/include/z80ex.h"


    typedef enum en_MZ800SWITCH {
        MZ800SWITCH_OFF = 0,
        MZ800SWITCH_ON,
    } en_MZ800SWITCH;

#define MZ800_INTERRUPT_CTC2    ( 1 << 0 )
#define MZ800_INTERRUPT_PIOZ80  ( 1 << 1 )
#define MZ800_INTERRUPT_FDC     ( 1 << 2 )


    typedef enum en_MZEVENT {
        //        EVENT_GDG_STS_HSYNC_END,
        EVENT_GDG_HBLN_END,
        EVENT_GDG_HBLN_START,
        EVENT_GDG_STS_VSYNC_END,
        EVENT_GDG_STS_VSYNC_START,
        EVENT_GDG_AFTER_LAST_SCREEN_PIXEL,
        //        EVENT_GDG_STS_HSYNC_START,
        EVENT_GDG_AFTER_LAST_VISIBLE_PIXEL,
        EVENT_GDG_REAL_HSYNC_START,
        EVENT_GDG_SCREEN_ROW_END,

        // jine, nez GDG eventy
        EVENT_NO_GDG, /* pouze hranicni hodnota - neni skutecny event */

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
        EVENT_SPEED_SYNC,
#endif
                
#ifdef MZ800EMU_CFG_CLK1M1_FAST
        EVENT_CTC0,
        EVENT_CMT,
#endif

        // high priority eventy
        EVENT_MZ800_INTERRUPT,
        EVENT_USER_INTERFACE,
    } en_MZEVENT;


    typedef struct st_EVENT {
        en_MZEVENT event_name;
        unsigned ticks;
    } st_EVENT;

#define MZ800_EMULATION_SPEED_NORMAL    0
#define MZ800_EMULATION_SPEED_MAX       1


    typedef enum en_DEVELMODE {
        DEVELMODE_NO = 0,
        DEVELMODE_YES,
    } en_DEVELMODE;


    typedef struct st_mz800 {
        Z80EX_CONTEXT *cpu; /* Model Z80ex */

        Z80EX_BYTE regDBUS_latch; /* Obsahuje esoterickeho ducha hodnoty posledniho bajtu, ktery byl precten na datove sbernici */

        int pio8255_ct53g7; /* rizeni CTC0 v rezimu MZ700 */

        //unsigned have_rejected_interrupt;

        unsigned use_max_emulation_speed; /* MZ800_EMULATION_SPEED_NORMAL = 0, MZ800_EMULATION_SPEED_MAX = 1 */
        unsigned emulation_paused;

        unsigned synchronised_insideop_GDG_ticks; /* Celkovy pocet gdg ticku, ktere jsme v ramci prave provadene instrukce jiz synchronizovali pres mz800_sync_inside_cpu() */

        unsigned debug_pc;

        unsigned event_locked; /* Pokud je 1, tak se prave nachazime v miste, kde neni vhodne prepisovat eventy */
        st_EVENT event;

        unsigned status_changed;
        unsigned status_emulation_speed;

        unsigned interrupt;

        en_DEVELMODE development_mode;

        en_MZ800SWITCH mz800_switch;

#ifdef MZ800EMU_CFG_VARIABLE_SPEED        
        st_EVENT speed_sync_event;
        unsigned speed_in_percentage; /* pozadovana rychlost v procentech 1 - 500, default 100 % */
        unsigned speed_frame_width;
#endif

    } st_mz800;

    extern struct st_mz800 g_mz800;

#define TEST_EMULATION_PAUSED       ( g_mz800.emulation_paused != 0 )

    /* Pokud to bude nutne, tak tady je misto, kde by mohla vzniknout fronta cekajicich eventu */
#define SET_MZ800_EVENT(e,t)        {\
    if ( ! g_mz800.event_locked ) {\
        g_mz800.event.event_name = e;\
        g_mz800.event.ticks = t;\
    };\
}


    typedef enum en_INSIDEOP {
        INSIDEOP_IORQ_WR = ( 0 << 1 | 0 ),
        INSIDEOP_IORQ_RD = ( 0 << 1 | 1 ),
        INSIDEOP_MREQ_WR = ( 1 << 1 | 0 ),
        INSIDEOP_MREQ_RD = ( 1 << 1 | 1 ),
        INSIDEOP_MREQ_M1_RD = ( 1 << 2 | 1 << 1 | 1 ), /* nemenit poradi bitu !!! viz memory.c */
        INSIDEOP_MZ700_NOTHBLN_VRAM_MREQ = ( 1 << 3 ),
    } en_INSIDEOP;

    extern void mz800_ctc2_interrupt_handle ( void );
    extern void mz800_pioz80_interrupt_handle ( void );
    extern void mz800_fdc_interrupt_handle ( unsigned interrupt );

    extern void mz800_reset ( void );
    extern void mz800_init ( void );
    extern void mz800_main ( void );
    extern void mz800_exit ( void );
    extern void mz800_set_display_mode ( Z80EX_BYTE dmd_mode );
    extern void mz800_sync_inside_cpu ( en_INSIDEOP insideop );
    extern void mz800_flush_full_screen ( void );

    extern void mz800_switch_emulation_speed ( unsigned emulation_speed );
    extern void mz800_pause_emulation ( unsigned value );

#ifdef __cplusplus
}
#endif

#endif /* MZ800_H */

