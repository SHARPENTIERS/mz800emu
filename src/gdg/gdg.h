/* 
 * File:   gdg.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 18:38
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

#ifndef GDG_H
#define GDG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#include "mz800.h"

#define GDG_CTC0CLK_DIVIDER          16

    // tohle bude u MZ-1500 jinak - je potreba to zmerit
#define IORQ_RD_TICKS               12  /* Delka IORQ RD pulzu v GTG taktech - cteni probehne pri jeho nabezne hrane */
#define IORQ_WR_TICKS               9   /* ??? TODO: zmerit ??? Delka IORQ WR pulzu v GTG taktech - cteni probehne pri jeho nabezne hrane */
#define MREQ_RD_M1_TICKS            7   /* Delka MREQ M1 RD pulzu v GTG taktech - cteni probehne pri jeho nabezne hrane */
#define MREQ_RD_TICKS               9   /* Delka MREQ DATA RD pulzu v GTG taktech - cteni probehne pri jeho nabezne hrane */
#define MREQ_WR_TICKS               9   /* ??? TODO: zmerit ??? Delka MREQ DATA WR pulzu v GTG taktech - cteni probehne pri jeho nabezne hrane */

#define GDG_SIGNAL_HBLNK                ( g_gdg.hbln )
#define GDG_SIGNAL_VBLNK                ( g_gdg.vbln )
#define GDG_SIGNAL_STS_HS               ( g_gdg.sts_hsync )
#define GDG_SIGNAL_STS_VS               ( g_gdg.sts_vsync )
#define GDG_SIGNAL_TEMPO                ( g_gdg.tempo & 1 )


    typedef enum en_SCRSTS {
        SCRSTS_IS_NOT_CHANGED = 0,
        SCRSTS_PREVIOUS_IS_CHANGED, // v predchozi strance doslo ke zmene, ale v teto uz nikoliv
        SCRSTS_THIS_IS_CHANGED, // nekde v teto strance doslo ke zmene
    } en_SCRSTS;

#define FB_STATE_NOT_CHANGED        0
#define FB_STATE_SCREEN_CHANGED     ( 1 << 0 )
#define FB_STATE_BORDER_CHANGED     ( 1 << 1 )

#define HBLN_ACTIVE     0
#define HBLN_OFF        1
#define VBLN_ACTIVE     0
#define VBLN_OFF        1
#define HSYN_ACTIVE     0
#define HSYN_OFF        1
#define VSYN_ACTIVE     0
#define VSYN_OFF        1


    typedef struct st_GDGEVENT {
        en_MZEVENT event;
        unsigned start_row; /* od ktereho radku event volame */
        unsigned num_rows; /* pocet radku na kterych se volani opakuje */
        unsigned event_column; /* na kterem sloupci se event zavola */
    } st_GDGEVENT;


    typedef struct st_GDG_TIMESTAMP {
        unsigned screens; /* celkovy pocet vykonanych obrazovek */
        unsigned ticks; /* celkovy pocet vykonanych pixelu z posledniho nedokonceneho screenu
                         * Hodnota 0 odpovida 1. pixelu viditelneho obrazu <0; 354431> 
                         */
    } st_GDG_TIMESTAMP;


    typedef struct st_GDG {
        st_EVENT event;

        st_GDG_TIMESTAMP total_elapsed; /* Celkovy pocet vykonanych snimku a pixelu */

        unsigned beam_row;
        unsigned screen_is_already_rendered_at_beam_pos; /* pokud byla pauza a probehnul render obrazovky, tak tady mame posledmi pozici paprsku, ktera uz je zobrazena */

        unsigned framebuffer_state; /* 1, pokud je potreba zobrazit novy obsah framebufferu */

        en_SCRSTS screen_changes; /* nenulova hodnota, pokud v tomto nebo predchozim snimku probehly zmeny */
        en_SCRSTS border_changes; /* nenulova hodnota, pokud v tomto nebo predchozim snimku probehly zmeny */

        unsigned screen_need_update_from; /* od ktereho pixelu aktualniho radku je potreba updatovat framebuffer */
        unsigned last_updated_border_pixel; /* od ktereho pixelu aktualniho radku je potreba updatovat framebuffer */

#ifdef MACHINE_EMU_MZ800
        unsigned sts_vsync; /* STS Vsync, ktery vidime na status registru - neodpovida skutecnemu VS */
        unsigned sts_hsync; /* STS Hsync, ktery vidime na status registru - neodpovida skutecnemu HS */
#endif
        unsigned hbln; /* HBLN: 0 - pokud se sloupec paprsku nachazi mimo screen */
        unsigned vbln; /* VBLN: 0 - pokud se radek paprsku nachazi mimo screen */
        //        unsigned hsync;     /* Skutecny Hsync, ktery se posila na vstup CTC1 */

        unsigned regct53g7; /* rizeni GATE pro CTC0 v ctc8253 v rezimu MZ700 */

        unsigned tempo;
        unsigned tempo_divider;

#ifdef MZ800EMU_CFG_CLK1M1_SLOW
        unsigned ctc0clk;
#endif

    } st_GDG;

    extern st_GDG g_gdg;


    extern struct st_GDGEVENT *g_gdgevent;

#define GDG_TEST_VBLN       ( g_gdg.vbln == 0 )

    extern void gdg_init ( void );
    extern void gdg_reset ( void );

#define gdg_compute_total_ticks( now_ticks ) ( now_ticks + ( (uint64_t) g_gdg.total_elapsed.screens * VIDEO_SCREEN_TICKS ) )
#define gdg_get_total_ticks() gdg_compute_total_ticks( g_gdg.total_elapsed.ticks )
#define gdg_get_insigeop_ticks() ( g_gdg.total_elapsed.ticks + g_mz800.instruction_insideop_sync_ticks )

#ifdef MZ800EMU_CFG_CLK1M1_FAST
#define gdg_proximate_clk1m1_event( now_ticks ) ( now_ticks + ( 0x10 - ( gdg_compute_total_ticks ( now_ticks ) & 0x0f ) ) )
#endif


    static inline void gdg_get_timestamp ( st_GDG_TIMESTAMP *tm ) {
        tm->screens = g_gdg.total_elapsed.screens;
        tm->ticks = g_gdg.total_elapsed.ticks;
    }

#ifdef MACHINE_EMU_MZ800
#include "gdg_mz800.h"
#endif
#ifdef MACHINE_EMU_MZ1500
#include "gdg_mz1500.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* GDG_H */

