/* 
 * File:   mz800.c
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

#include "mz800emu_cfg.h"

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "cfgmain.h"

#include "z80ex/include/z80ex.h"

#include "mz800.h"
#include "gdg/gdg.h"
#include "gdg/video.h"
#include "gdg/framebuffer.h"
#include "gdg/vramctrl.h"

#include "memory/memory.h"
#include "port.h"
#include "ctc8253/ctc8253.h"
#include "pio8255/pio8255.h"
#include "pioz80/pioz80.h"
#include "psg/psg.h"
#include "audio.h"

#include "fdc/fdc.h"
#include "ramdisk/ramdisk.h"
#include "cmt/cmt.h"
#include "cmt/cmt_hack.h"
#include "qdisk/qdisk.h"

#include "qdisk/qdisk.h"

#include "iface_sdl/iface_sdl.h"
#include "iface_sdl/iface_sdl_audio.h"

#include <SDL_timer.h>
// ve Win32 neni ???
//#include <SDL2/SDL_assert.h>

#include "ui/ui_main.h"
#include "typedefs.h"

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
#include "debugger/debugger.h"
#include "debugger/breakpoints.h"
#include "ui/debugger/ui_breakpoints.h"
#endif


#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"
#include "ui/debugger/ui_debugger.h"


struct st_mz800 g_mz800;


void mz800_reset ( void ) {

    printf ( "\nMZ800 Reset!\n" );
    if ( TEST_EMULATION_PAUSED ) {
        printf ( "Emulation is still PAUSED!\n" );
    };
    printf ( "\n" );

    memory_reset ( );
    gdg_reset ( );
    //    ctc8253_reset ( );

    z80ex_reset ( g_mz800.cpu );
    pioz80_reset ( );
    fdc_reset ( );

    cmthack_reset ( );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    debugger_update_all ( );
#endif

}


void mz800_exit ( void ) {
    fdc_exit ( );
    qdisk_exit ( );
    ramdisc_exit ( );
    cmthack_exit ( );
    cmt_exit ( );
#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    debugger_exit ( );
#endif
}


/* tady resime rozdily chovani HW mezi MZ rezimy */
void mz800_set_display_mode ( Z80EX_BYTE dmd_mode ) {

    /* TODO: doplnit ovladani citace kurzoru */

    if ( dmd_mode & REGISTER_DMD_FLAG_MZ700 ) {
        /* rezim MZ-700 */
        ctc8253_gate ( 0, g_gdg.regct53g7, g_gdg.elapsed_screen_ticks );

    } else {
        /* rezim MZ-800 */
        ctc8253_gate ( 0, 1, g_gdg.elapsed_screen_ticks );
        g_vramctrl.mz700_wr_latch_is_used = 0;
    };

    g_gdg.regDMD = dmd_mode;
}


void mz800_ctc2_interrupt_handle ( void ) {
    //printf ( "ctc2 interrupt handle\n" );
    /* k interruptu dojde jen pokud CTC2: 1 a PC02: 1 */
    if ( CTC8253_OUT ( 2 ) & g_pio8255.signal_pc02 ) {
        g_mz800.interrupt |= MZ800_INTERRUPT_CTC2;
        //printf ( "ctc2 interrupt handle - ctc2: %d, pc02: %d - set \n", CTC8253_OUT ( 2 ), g_pio8255.signal_pc02 );
    } else {
        g_mz800.interrupt &= ~MZ800_INTERRUPT_CTC2;
        //printf ( "ctc2 interrupt handle - ctc2: %d, pc02: %d - reset \n", CTC8253_OUT ( 2 ), g_pio8255.signal_pc02 );
    };
}


void mz800_pioz80_interrupt_handle ( void ) {
    //printf ( "pioz80 interrupt handle\n" );
    if ( PIOZ80_INTERRUPT ) {
        g_mz800.interrupt |= MZ800_INTERRUPT_PIOZ80;
    } else {
        g_mz800.interrupt &= ~MZ800_INTERRUPT_PIOZ80;
    };
}


void mz800_fdc_interrupt_handle ( unsigned interrupt ) {
    if ( interrupt ) {
        g_mz800.interrupt |= MZ800_INTERRUPT_FDC;
    } else {
        g_mz800.interrupt &= ~MZ800_INTERRUPT_FDC;
    };
}


/* CPU preslo do rezimu, kdy je opet povoleno ruseni - zkontrolujeme, zda tu neceka nejaky interrupt */
void mz800_ei_cb ( Z80EX_CONTEXT *cpu, void *user_data ) {
    //printf ( "EICB - PC = 0x%04x\n", z80ex_get_reg ( cpu, regPC ) );
    if ( g_mz800.interrupt ) {
        SET_MZ800_EVENT ( EVENT_MZ800_INTERRUPT, 0 );
        //printf ( "EICB - found interrupt\n" );
    } else {
        //printf ( "EICB - nothing\n" );
    };
}


void mz800_init ( void ) {

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "MZ800" );
    CFGELM *elm;

    elm = cfgmodule_register_new_element ( cmod, "development_mode", CFGENTYPE_KEYWORD, DEVELMODE_NO,
                                           DEVELMODE_NO, "NO",
                                           DEVELMODE_YES, "YES",
                                           -1 );

    cfgelement_set_handlers ( elm, (void*) &g_mz800.development_mode, (void*) &g_mz800.development_mode );

    elm = cfgmodule_register_new_element ( cmod, "mz800_switch", CFGENTYPE_KEYWORD, MZ800SWITCH_OFF,
                                           MZ800SWITCH_OFF, "OFF",
                                           MZ800SWITCH_ON, "ON",
                                           -1 );

    cfgelement_set_handlers ( elm, (void*) &g_mz800.mz800_switch, (void*) &g_mz800.mz800_switch );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    mz800_switch_emulation_speed ( MZ800_EMULATION_SPEED_NORMAL );
    //mz800_set_cpu_speed ( MZ800_EMULATION_SPEED_MAX );

    g_mz800.emulation_paused = 0;
    g_mz800.event_locked = 0;

    mz800_pause_emulation ( 0 );

    g_mz800.cpu = z80ex_create (
                                 memory_read_cb, NULL,
                                 memory_write_cb, NULL,
                                 port_read_cb, NULL,
                                 port_write_cb, NULL,
                                 pioz80_intread_cb, NULL
                                 );

    /*
        z80ex_set_tstate_callback ( g_mz800.cpu, mz800_tstate_cb, NULL );
     */

    z80ex_set_reti_callback ( g_mz800.cpu, pioz80_reti_cb, NULL );

    z80ex_set_ei_callback ( g_mz800.cpu, mz800_ei_cb, NULL );

    g_mz800.regDBUS_latch = 0;
    g_mz800.debug_pc = 0;
    g_mz800.status_changed = 0;
    g_mz800.interrupt = 0;

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
    g_mz800.speed_in_percentage = 100;
    g_mz800.speed_frame_width = VIDEO_SCREEN_TICKS * (float) g_mz800.speed_in_percentage / 100;
    g_mz800.speed_sync_event.event_name = EVENT_SPEED_SYNC;
    g_mz800.speed_sync_event.ticks = g_mz800.speed_frame_width;
#endif

    memory_init ( );
    ctc8253_init ( );
    pio8255_init ( );
    pioz80_init ( );
    gdg_init ( );
    psg_init ( );
    audio_init ( );

    fdc_init ( );
    ramdisk_init ( );
    cmthack_init ( );
    cmt_init ( );
    qdisk_init ( );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    debugger_init ( );
#endif

}



static volatile unsigned make_picture_time = 1;
static volatile unsigned update_status_time = 1;

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
static volatile unsigned update_debugger_time = 1;
#endif

#define INTERRUPT_TIMER_MS          20
#define INTERRUPT_MAKEPIC_PER_SEC   25
#define INTERRUPT_POOL_EVENTS_PER_SEC   20


uint32_t screens_counter_flush ( uint32_t interval, void* param ) {
    static unsigned make_pic_counter = 0;
    //static unsigned pool_events_counter = 0;
    static unsigned call_counter = 0;

#if 0
    if ( pool_events_counter == 0 ) {
        /* Volani pool events v interruptu je problemove ve win32 */
        //        iface_sdl_pool_window_events ( );
        pool_events_time = 1;
        pool_events_counter = ( 1000 / INTERRUPT_TIMER_MS / INTERRUPT_POOL_EVENTS_PER_SEC ) - 1;
    } else {
        pool_events_counter--;
    };
#endif

    //if ( ( g_mz800.emulation_speed == 0 ) || ( make_pic_counter == 0 ) ) {
    if ( make_pic_counter == 0 ) {
        //printf ("newpic\n");
        make_picture_time++;
        make_pic_counter = ( 1000 / INTERRUPT_TIMER_MS / INTERRUPT_MAKEPIC_PER_SEC ) - 1;
    } else {
        make_pic_counter--;
    };

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    static unsigned debugger_update_counter = 0;

    if ( !TEST_EMULATION_PAUSED ) {
        if ( TEST_DEBUGGER_ACTIVE ) {
            if ( update_debugger_time != 1 ) {
                if ( debugger_update_counter++ >= 2 ) {
                    debugger_update_counter = 0;
                    update_debugger_time = 1;
                    SET_MZ800_EVENT ( EVENT_USER_INTERFACE, 0 );
                } else {
                    debugger_update_counter++;
                };
            };
        };
    };
#endif

    //return interval;

    if ( call_counter == 0 ) {
#if 0
        if ( !TEST_EMULATION_PAUSED ) {
            if ( TEST_DEBUGGER_ACTIVE ) {
                if ( update_debugger_time != 1 ) {
                    update_debugger_time = 1;
                    SET_MZ800_EVENT ( EVENT_USER_INTERFACE, 0 );
                };
            };
        };
#endif

        call_counter = ( ( 1000 / INTERRUPT_TIMER_MS ) ) - 1;
        static unsigned last = 0;
        unsigned i = g_gdg.elapsed_total_screens;
        unsigned j = i - last;
        last = i;
        g_mz800.status_emulation_speed = (float) j / 0.5;
        //        g_mz800.status_changed = 1;
        //        printf ( "Pocet snimku: %d, %d %%\n", j, g_mz800.status_emulation_speed );
        //iface_sdl_render_status_line ( );
        update_status_time = 1;
    } else {
        call_counter--;
    };

    /* zkusime volad odsud */
    //ui_iteration ( );

    return interval;
}


static inline void mz800_speed_sync_event ( void ) {

#ifdef AUDIO_FILLBUFF_v1
    audio_fill_buffer_v1 ( g_mz800.event.ticks );
    g_audio.last_update = 0;
    g_audio.buffer_position = 0;
#endif

#ifdef AUDIO_FILLBUFF_v2
    audio_fill_buffer_v2 ( gdg_compute_total_ticks ( g_mz800.event.ticks ) );
#endif

#ifndef MZ800EMU_CFG_AUDIO_DISABLED
    audio_sdl_wait_to_cycle_done ( );
#endif

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
    g_mz800.speed_sync_event.ticks += g_mz800.speed_frame_width;
#endif

}


static inline st_EVENT* mz800_place_proximate_event ( void ) {

    st_EVENT *proximate_event;

#ifdef MZ800EMU_CFG_CLK1M1_FAST

    if ( g_ctc8253[CTC_CS0].clk1m1_event.ticks <= g_cmt.clk1m1_event.ticks ) {
        proximate_event = &g_ctc8253 [CTC_CS0].clk1m1_event;
    } else {
        proximate_event = &g_cmt.clk1m1_event;
    };

    if ( g_gdg.event.ticks <= proximate_event->ticks ) {
        proximate_event = &g_gdg.event;
    };
#else
    proximate_event = &g_gdg.event;
#endif

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
    if ( g_mz800.speed_sync_event.ticks <= proximate_event->ticks ) {
        proximate_event = &g_mz800.speed_sync_event;
    };
#endif

    return proximate_event;
}


static inline void mz800_screen_done_event ( void ) {

#if 0
    if ( pool_events_time ) {
        iface_sdl_pool_window_events ( );
        pool_events_time = 0;
    };
#endif

    static unsigned last_make_picture_time;

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
    if ( ( ( g_mz800.use_max_emulation_speed == 0 ) && ( g_mz800.speed_in_percentage == 100 ) ) || ( last_make_picture_time != make_picture_time ) ) {
#else
    if ( ( g_mz800.use_max_emulation_speed == 0 ) || ( last_make_picture_time != make_picture_time ) ) {
#endif
        last_make_picture_time = make_picture_time;

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
        if ( g_debugger.animated_updates ) debugger_animation ( );
#endif

        if ( update_status_time ) {
            update_status_time = 0;
            iface_sdl_render_status_line ( );
        };
        iface_sdl_pool_all_events ( );
        ui_iteration ( );



        //g_iface_sdl.redraw_full_screen_request = 1;
        if ( g_gdg.framebuffer_state || g_iface_sdl.redraw_full_screen_request ) {
            iface_sdl_update_window ( );
            g_gdg.framebuffer_state = FB_STATE_NOT_CHANGED;


            g_gdg.screen_is_already_rendered_at_beam_pos = g_mz800.event.ticks;
#if 0

        } else if ( g_mz800.status_changed ) {
            iface_sdl_render_status_line ( );
#endif
        };

#ifndef MZ800EMU_CFG_VARIABLE_SPEED
        mz800_speed_sync_event ( );
#endif

    };

#ifdef MZ800EMU_CFG_CLK1M1_FAST
    ctc8253_on_screen_done_event ( );
    cmt_on_screen_done_event ( );
#endif

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
    g_mz800.speed_sync_event.ticks -= VIDEO_SCREEN_TICKS;
#endif

#ifdef MZ800EMU_CFG_CLK1M1_SLOW
    g_gdg.ctc0clk++;
#endif

    g_gdg.elapsed_total_screens++;
#ifdef MZ800EMU_CFG_SPEED_TEST
    if ( g_gdg.elapsed_total_screens > 1000 ) main_app_quit ( EXIT_SUCCESS );
#endif
    g_gdg.elapsed_screen_ticks -= ( VIDEO_SCREEN_TICKS - 1 );
    g_gdg.beam_row = 0;
    //printf ("snimek!\n");
}


static inline void mz800_sync ( void ) {

    while ( g_gdg.elapsed_screen_ticks >= g_mz800.event.ticks ) {

#if 1
        if ( g_mz800.event.event_name >= EVENT_NO_GDG ) {
#else
        if ( g_mz800.event.event_name >= EVENT_NOT_HW ) {
#endif

#if 1
#ifdef MZ800EMU_CFG_CLK1M1_FAST
            if ( g_ctc8253 [ CTC_CS0 ].clk1m1_event.ticks <= g_mz800.event.ticks ) {
                ctc8253_ctc1m1_event ( g_mz800.event.ticks );
            };

            if ( g_cmt.clk1m1_event.ticks <= g_mz800.event.ticks ) {
                cmt_ctc1m1_event ( g_mz800.event.ticks );
            };
#endif

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
            if ( g_mz800.speed_sync_event.ticks <= g_mz800.event.ticks ) {
                mz800_speed_sync_event ( );
            };
#endif
#endif
            st_EVENT *proximate_event = mz800_place_proximate_event ( );

            g_mz800.event.event_name = proximate_event->event_name;
            g_mz800.event.ticks = proximate_event->ticks;

            if ( g_mz800.event.event_name >= EVENT_NOT_HW ) return;

            if ( !( g_gdg.elapsed_screen_ticks >= g_mz800.event.ticks ) ) return;
        };


        switch ( g_mz800.event.event_name ) {

            case EVENT_GDG_HBLN_END:
                g_gdg.hbln = HBLN_OFF;

                g_gdg.screen_is_already_rendered_at_beam_pos = g_mz800.event.ticks;

                /* V rezimu MZ-700 aktualizujeme screen framebuffer jakmile skonci HBLN. */
                if ( ( DMD_TEST_MZ700 ) && ( ( g_gdg.beam_row >= VIDEO_BEAM_CANVAS_FIRST_ROW ) && ( g_gdg.beam_row <= VIDEO_BEAM_CANVAS_LAST_ROW ) ) ) {
                    if ( g_gdg.screen_changes ) {
                        framebuffer_update_MZ700_screen_row ( );
                    };

                    /* Pokud jsme na poslednim pixelu screen area */
                    if ( g_gdg.beam_row == VIDEO_BEAM_CANVAS_LAST_ROW ) {
                        if ( g_gdg.screen_changes ) {
                            g_gdg.screen_changes--;
                            g_gdg.framebuffer_state |= FB_STATE_SCREEN_CHANGED;
                        };
                    };
                };
                break;


            case EVENT_GDG_HBLN_START:
                g_gdg.hbln = HBLN_ACTIVE;
                g_vramctrl.mz700_wr_latch_is_used = 0;

                unsigned last_vbln_state = g_gdg.vbln;

                if ( g_gdg.beam_row == VIDEO_BEAM_CANVAS_LAST_ROW ) {
                    g_gdg.vbln = VBLN_ACTIVE;
                } else if ( g_gdg.beam_row == VIDEO_BEAM_CANVAS_FIRST_ROW - 1 ) {
                    g_gdg.vbln = VBLN_OFF;
                };

                if ( last_vbln_state != g_gdg.vbln ) {
                    /* udalost pro PIO-Z80 - VBLN */
                    pioz80_port_event ( PIOZ80_PORT_A, 5, g_gdg.vbln );
                };
                break;


            case EVENT_GDG_STS_VSYNC_END:
                g_gdg.sts_vsync = VSYN_OFF;
                break;


            case EVENT_GDG_STS_VSYNC_START:
                g_gdg.sts_vsync = VSYN_ACTIVE;
                break;


            case EVENT_GDG_AFTER_LAST_SCREEN_PIXEL:
                /* V rezimu MZ-800 aktualizujeme screen framebuffer az po dokoncenem radku. */
                if ( !DMD_TEST_MZ700 ) {
                    //g_gdg.screen_changes = 1;
                    if ( g_gdg.screen_changes ) {
                        framebuffer_MZ800_screen_row_fill ( VIDEO_CANVAS_WIDTH );
                    };

                    /* Pokud jsme na poslednim pixelu screen area */
                    if ( g_gdg.beam_row == VIDEO_BEAM_CANVAS_LAST_ROW ) {
                        if ( g_gdg.screen_changes ) {
                            g_gdg.screen_changes--;
                            g_gdg.framebuffer_state |= FB_STATE_SCREEN_CHANGED;
                        };
                    };
                };
                break;


                //            case EVENT_GDG_STS_HSYNC_START:
                //                g_gdg.sts_hsync = 0;
                //                break;


            case EVENT_GDG_AFTER_LAST_VISIBLE_PIXEL:
                g_gdg.sts_hsync = HSYN_ACTIVE; /* aktivni o par ticku drive */
                /* Jsme skutecne jeste ve viditelne casti obrazu? */
                if ( g_gdg.beam_row < VIDEO_DISPLAY_HEIGHT ) {
                    if ( g_gdg.border_changes ) {
                        framebuffer_border_row_fill ( );
                    };
                    g_gdg.last_updated_border_pixel = VIDEO_BEAM_DISPLAY_LAST_COLUMN + 1;

                    /* Pokud jsme na poslednim pixelu visible area */
                    if ( g_gdg.beam_row == VIDEO_BEAM_DISPLAY_LAST_ROW ) {
                        if ( g_gdg.border_changes ) {
                            g_gdg.border_changes--;
                            g_gdg.framebuffer_state |= FB_STATE_BORDER_CHANGED;
                        };
                    };
                };
                break;


            case EVENT_GDG_REAL_HSYNC_START:
                /* tady zacina skutecny HSYNC, ktery je na RGBI a jeho sestupna hrana je CTC1_CLK */
                ctc8253_clkfall ( CTC_CS1, g_mz800.event.ticks );
                break;


            case EVENT_GDG_SCREEN_ROW_END:
                g_gdg.sts_hsync = HSYN_OFF;
                g_gdg.tempo_divider++;
                /* Na mem MZ800 ma TEMPO pravdepodobne cca 34 Hz - tedy od oka :) */
                if ( g_gdg.tempo_divider == 229 ) {
                    g_gdg.tempo_divider = 0;
                    g_gdg.tempo++;
                };

                /* Muzeme vynulovat update pozici borderu ve framebufferu? */
                /* (Pokud ma jinou hodnotu, tak to znamena, ze pres OUT doslo ke zmene radku, ktery teprve nastane.) */
                if ( g_gdg.last_updated_border_pixel == VIDEO_BEAM_DISPLAY_LAST_COLUMN + 1 ) {
                    g_gdg.last_updated_border_pixel = 0;
                };

                g_gdg.screen_need_update_from = 0;

                if ( g_gdg.beam_row < VIDEO_SCREEN_HEIGHT - 1 ) {
                    g_gdg.beam_row++;
                } else {
                    mz800_screen_done_event ( );
                };
                break;
#if 0
#ifdef MZ800EMU_CFG_CLK1M1_FAST
            case EVENT_CTC0:
                ctc8253_ctc1m1_event ( g_mz800.event.ticks );
                break;

            case EVENT_CMT:
                cmt_ctc1m1_event ( g_mz800.event.ticks );
                break;
#endif

#ifdef MZ800EMU_CFG_VARIABLE_SPEED
            case EVENT_SPEED_SYNC:
                mz800_speed_sync_event ( );
                break;
#endif
#endif
                /* tyto eventy neni potreba zde resit */
            case EVENT_NO_GDG:
            case EVENT_NOT_HW:
            case EVENT_MZ800_INTERRUPT:
            case EVENT_USER_INTERFACE:
#if 1
#ifdef MZ800EMU_CFG_CLK1M1_FAST
            case EVENT_CTC0:
            case EVENT_CMT:
#endif
#ifdef MZ800EMU_CFG_VARIABLE_SPEED
            case EVENT_SPEED_SYNC:
#endif
#endif
                break;

        };


        /* Presun na dalsi GDG event */
        if ( g_gdg.event.event_name == EVENT_GDG_SCREEN_ROW_END ) {
            g_gdg.event.event_name = 0;
        } else {
            g_gdg.event.event_name++;
        };

        while ( 1 ) {
            if ( ( g_gdgevent[ g_gdg.event.event_name ].start_row <= g_gdg.beam_row ) &&
                 ( ( g_gdgevent [ g_gdg.event.event_name ].start_row + g_gdgevent[ g_gdg.event.event_name ].num_rows - 1 ) >= g_gdg.beam_row ) ) {

                g_gdg.event.ticks = g_gdg.beam_row * VIDEO_SCREEN_WIDTH + g_gdgevent[ g_gdg.event.event_name ].event_column;

                break;
            };
            g_gdg.event.event_name++;
        };

        st_EVENT *proximate_event = mz800_place_proximate_event ( );

        g_mz800.event.event_name = proximate_event->event_name;
        g_mz800.event.ticks = proximate_event->ticks;
    };
}


#ifdef MZ800EMU_CFG_CLK1M1_SLOW


static inline void mz800_sync_ctc0_and_cmt ( unsigned instruction_ticks ) {

    g_gdg.elapsed_screen_ticks -= g_gdg.ctc0clk;
    instruction_ticks += g_gdg.ctc0clk;

    while ( instruction_ticks > GDGCLK_1M1_DIVIDER - 1 ) {

        g_gdg.elapsed_screen_ticks += GDGCLK_1M1_DIVIDER;
        instruction_ticks -= GDGCLK_1M1_DIVIDER;

        ctc8253_clkfall ( CTC_CS0, g_gdg.elapsed_screen_ticks );

        /* TODO: prozatim si sem povesime i pomaly cmt_step() */
        if ( TEST_CMT_PLAYING ) {
            cmt_step ( );
        };
    };

    g_gdg.ctc0clk = instruction_ticks;
    g_gdg.elapsed_screen_ticks += instruction_ticks;
}

#endif


/* TODO: casovani (MZ700 VRAM casovani) neni jeste uplne OK - viz FX soundtrack 3 - border pri zmacknute klavese je v trochu jine poloze, nez na realnem HW */


/* 
 * To by mohlo byt zpusobeno tim jak a kdy se scanuje klavesnice - tedy zase az tak velikou haluz zrejme nemame.
 * Pozn: scanovanim klavesnice to zarucene zpusobeno neni!
 * 
 */
void mz800_sync_inside_cpu ( en_INSIDEOP insideop ) {

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    if ( TEST_DEBUGGER_MEMOP_CALL ) return;
#endif

    g_mz800.event_locked = 1;

    int flag_high_priority_event = 0;

    st_EVENT hpr_event;

    if ( g_mz800.event.event_name >= EVENT_MZ800_INTERRUPT ) {

        if ( g_mz800.event.event_name > EVENT_MZ800_INTERRUPT ) {
            flag_high_priority_event = 1;
            hpr_event.event_name = g_mz800.event.event_name;
            hpr_event.ticks = g_mz800.event.ticks;
        };


        st_EVENT *proximate_event = mz800_place_proximate_event ( );

        g_mz800.event.event_name = proximate_event->event_name;
        g_mz800.event.ticks = proximate_event->ticks;
    };

    unsigned insideop_ticks = 0;

    if ( insideop == INSIDEOP_MZ700_NOTHBLN_VRAM_MREQ ) {

        unsigned gdg_ticks_to_hbln = VIDEO_BEAM_HBLN_FIRST_COLUMN - VIDEO_GET_SCREEN_COL ( g_gdg.elapsed_screen_ticks );
        gdg_ticks_to_hbln += GDGCLK2CPU_DIVIDER - ( gdg_ticks_to_hbln % GDGCLK2CPU_DIVIDER );
        z80ex_w_states ( g_mz800.cpu, ( gdg_ticks_to_hbln / GDGCLK2CPU_DIVIDER ) );

        g_mz800.synchronised_insideop_GDG_ticks += gdg_ticks_to_hbln;
        insideop_ticks = gdg_ticks_to_hbln;
        //printf ( "inside hbln: %d\n", g_mz800.synchronised_insideop_GDG_ticks );

    } else {

        unsigned gdg_ticks_to_sync = ( z80ex_op_tstate ( g_mz800.cpu ) * GDGCLK2CPU_DIVIDER ) - g_mz800.synchronised_insideop_GDG_ticks;

        switch ( insideop ) {
            case INSIDEOP_IORQ_RD:
                gdg_ticks_to_sync += IORQ_RD_TICKS;
                break;

            case INSIDEOP_IORQ_WR:
                gdg_ticks_to_sync += IORQ_WR_TICKS;
                break;

            case INSIDEOP_MREQ_RD:
                gdg_ticks_to_sync += MREQ_RD_TICKS;
                break;

            case INSIDEOP_MREQ_M1_RD:
                /* TODO: bug - z80ex signalizuje jen prvni M1 cycle, tzn., ze dalsi instrukcni bajt bude cten jako by byl datovy */
                gdg_ticks_to_sync += MREQ_RD_M1_TICKS;
                break;

            case INSIDEOP_MREQ_WR:
                gdg_ticks_to_sync += MREQ_WR_TICKS;
                break;

            case INSIDEOP_MZ700_NOTHBLN_VRAM_MREQ:
                break;
        };
        g_mz800.synchronised_insideop_GDG_ticks += gdg_ticks_to_sync;
        insideop_ticks = gdg_ticks_to_sync;
    };



#ifdef MZ800EMU_CFG_CLK1M1_SLOW
    mz800_sync_ctc0_and_cmt ( insideop_ticks );
#else
    g_gdg.elapsed_screen_ticks += insideop_ticks;
#endif

    if ( g_gdg.elapsed_screen_ticks >= g_gdg.event.ticks ) {
        mz800_sync ( );
    };

    /* Tohle je potreba, aby nas to vyhnalo z instrukcni smycky do obsluhy interruptu */
    if ( g_mz800.interrupt ) {
        g_mz800.event.event_name = EVENT_MZ800_INTERRUPT;
        g_mz800.event.ticks = 0;
    } else if ( flag_high_priority_event == 1 ) {
        g_mz800.event.event_name = hpr_event.event_name;
        g_mz800.event.ticks = hpr_event.ticks;
    };

    g_mz800.event_locked = 0;
}


static inline void mz800_do_emulation_paused ( void ) {

    //printf ( "Emulation paused - PC: 0x%04x.\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED

    debugger_update_all ( );
    debugger_step_call ( 0 );

    iface_sdl_render_status_line ( );

    framebuffer_border_changed ( );
    if ( !DMD_TEST_MZ700 ) {
        framebuffer_MZ800_screen_changed ( );
    };
    unsigned screen_elapsed_ticks = g_gdg.elapsed_screen_ticks;
    iface_sdl_update_window_in_beam_interval ( g_gdg.screen_is_already_rendered_at_beam_pos, screen_elapsed_ticks );
    g_gdg.screen_is_already_rendered_at_beam_pos = screen_elapsed_ticks;

    while ( TEST_EMULATION_PAUSED && ( !TEST_DEBUGGER_STEP_CALL ) ) {
        iface_sdl_pool_all_events ( );
        ui_iteration ( );

        if ( g_iface_sdl.redraw_full_screen_request ) {
            iface_sdl_update_window ( );
        };
    };

    if ( TEST_DEBUGGER_STEP_CALL ) {
        SET_MZ800_EVENT ( EVENT_USER_INTERFACE, 0 );
    };

#else // MZ800_DEBUGGER

    iface_sdl_render_status_line ( );
    iface_sdl_update_window_in_beam_interval ( g_gdg.screen_is_already_rendered_at_beam_pos, g_gdg.elapsed_screen_ticks );


    while ( TEST_EMULATION_PAUSED ) {
        iface_sdl_pool_all_events ( );
        ui_iteration ( );

        if ( g_iface_sdl.redraw_full_screen_request ) {
            iface_sdl_update_window ( );
        };

    };
#endif

}

#include <inttypes.h>


void mz800_main ( void ) {

    //            FILE *fp;
    //            if ( ! ( fp = fopen ( "output.txt", "w" ) ) ) {
    //                fprintf ( stderr, "nejde otevrit" );
    //            };

    mz800_reset ( );

    SDL_AddTimer ( INTERRUPT_TIMER_MS, screens_counter_flush, NULL );

    st_EVENT *proximate_event = mz800_place_proximate_event ( );

    g_mz800.event.event_name = proximate_event->event_name;
    g_mz800.event.ticks = proximate_event->ticks;


#ifdef MZ800EMU_CFG_SPEED_TEST
    z80ex_set_reg ( g_mz800.cpu, regHL, 0x10f0 );
    cmthack_load_filename ( "Flappy.mzf" );
    //cmthack_load_filename ( "Galao.mzf" );
    z80ex_set_reg ( g_mz800.cpu, regHL, ( g_memory.RAM [ 0x1105 ] << 8 ) | g_memory.RAM [ 0x1104 ] );
    z80ex_set_reg ( g_mz800.cpu, regBC, ( g_memory.RAM [ 0x1103 ] << 8 ) | g_memory.RAM [ 0x1102 ] );
    cmthack_read_body ( );

    z80ex_set_reg ( g_mz800.cpu, regSP, 0x1106 );
    printf ( "Test prg start: 0x%04x\n", ( g_memory.RAM [ 0x1107 ] << 8 ) | g_memory.RAM [ 0x1106 ] );
    //z80ex_set_reg ( g_mz800.cpu, regPC, ( g_memory.RAM [ 0x1107 ] << 8 ) | g_memory.RAM [ 0x1106 ] );
    g_memory.map = MEMORY_MAP_FLAG_ROM_0000 | MEMORY_MAP_FLAG_ROM_E000;

    z80ex_set_reg ( g_mz800.cpu, regPC, 0x308 );

    z80ex_set_reg ( g_mz800.cpu, regIM, 1 );
    pioz80_write_byte ( 0xfc & 0x03, 0x00 );

    pio8255_write ( 0x03, 0x8a );
    pio8255_write ( 0x03, 0x07 );
    pio8255_write ( 0x03, 0x05 );
    pio8255_write ( 0x03, 0x01 );
    pio8255_write ( 0x03, 0x05 );

    //memcpy ( g_memoryVRAM, g_rom.cgrom, MEMORY_SIZE_ROM_CGROM );
#endif


    /*
        pio8255_write ( 0x03, 0x8a );
        pio8255_write ( 0x03, 0x07 );
        pio8255_write ( 0x03, 0x05 );
        pio8255_write ( 0x03, 0x01 );
        pio8255_write ( 0x03, 0x05 );
     */

    while ( 1 ) {

        //                fprintf ( fp, "0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );
        //        printf ( "0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );

#if 0
        //        if ( g_mz800.debug_pc ) {
        char mnemonic [ 200 ];
        Z80EX_WORD addr = z80ex_get_reg ( g_mz800.cpu, regPC );
        int t_states, t_states2;
        unsigned bytecode_length = z80ex_dasm ( mnemonic, 200 - 1, 0, &t_states, &t_states2, debugger_dasm_read_cb, addr, NULL );
        printf ( "0x%04x\t%s\n", addr, mnemonic );
        //        } else if ( z80ex_get_reg ( g_mz800.cpu, regPC ) == 0x2f49 ) {
        //            printf ( "regA: 0x%02x\n", z80ex_get_reg ( g_mz800.cpu, regAF ) >> 8 );
        //_mz800.debug_pc = 1;
        //        };
#endif


        //        if ( z80ex_get_reg ( g_mz800.cpu, regPC ) == 0x2060 ) {
        //            printf ( "HL: 0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regHL ) );
        //        }

        unsigned instruction_ticks = 0;
        do {
            g_mz800.synchronised_insideop_GDG_ticks = 0;
            //            if ( z80ex_get_reg ( g_mz800.cpu, regPC ) == 0x0df3 ) {
            //                printf ( "ld hl,(0x1171)\n" );
            //            }
            instruction_ticks += ( z80ex_step ( g_mz800.cpu ) * GDGCLK2CPU_DIVIDER ) - g_mz800.synchronised_insideop_GDG_ticks;
            //            unsigned ticks = z80ex_step ( g_mz800.cpu );
            //            printf ( "Ticks: %d\n", ticks );
            //            instruction_ticks += ticks * BASE2CPU_DIVIDER;
        } while ( z80ex_last_op_type ( g_mz800.cpu ) != 0 );


#ifdef MZ800EMU_CFG_CLK1M1_SLOW
        mz800_sync_ctc0_and_cmt ( instruction_ticks );
#else
        g_gdg.elapsed_screen_ticks += instruction_ticks;
#endif


#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
        /*
         * 
         * Implementace zakladnich breakpointu
         * 
         */
        int breakpoint_id = g_breakpoints.bpmap [ z80ex_get_reg ( g_mz800.cpu, regPC ) ];
        if ( breakpoint_id != -1 ) {
            printf ( "INFO - activated breakpoint on addr: 0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );
            mz800_pause_emulation ( 1 );
            debugger_show_main_window ( );
            ui_breakpoints_show_window ( );
            ui_breakpoints_select_id ( breakpoint_id );
        };
#endif


        if ( g_gdg.elapsed_screen_ticks >= g_mz800.event.ticks ) {
            /* Pokud ceka ve fronte nejaky gdg event, interrupt, ci uzivatelem volana pauza */

            g_mz800.event_locked = 1;
            mz800_sync ( );
            g_mz800.event_locked = 0;


            /* Ceka na nas nejaky interrupt? */
            if ( g_mz800.interrupt ) {

                //printf ( "INTERRUPT: %d\n", g_mz800.interrupt );

                unsigned interrupt_ticks = z80ex_int ( g_mz800.cpu );

                if ( interrupt_ticks ) {
                    /* interrupt byl prijat */

                    //printf ( "Interrupt received!\n" );

                    /* Je potreba volat po prijeti jakehokoliv interruptu! PIOZ80 vidi prijaty interrupt na sbernici. */
                    pioz80_int_ack ( );

                    instruction_ticks = interrupt_ticks * GDGCLK2CPU_DIVIDER;

#ifdef MZ800EMU_CFG_CLK1M1_SLOW
                    mz800_sync_ctc0_and_cmt ( instruction_ticks );
#else
                    g_gdg.elapsed_screen_ticks += instruction_ticks;
#endif

                    if ( g_gdg.elapsed_screen_ticks >= g_mz800.event.ticks ) {
                        mz800_sync ( );
                    };

                    if ( g_mz800.interrupt ) {
                        /* interrupt nadale trva - po dalsi unstrukci budeme rusit znova */
                        SET_MZ800_EVENT ( EVENT_MZ800_INTERRUPT, 0 );
                    };

                } else {
                    /* interrupt nebyl prijat */

                    if ( z80ex_nmi_possible ( g_mz800.cpu ) != 0 ) {
                        /* OK, ted to nejde, protoze jsme uprostred instrukce */
                        SET_MZ800_EVENT ( EVENT_MZ800_INTERRUPT, 0 );
                    } else {
                        /* Ruseni je zakazano - postara se o to EICB */
                    };
                };
            };
            /* Konec obsluhy interruptu */


            /* jsme v pauze? */
            if ( TEST_EMULATION_PAUSED ) {
                mz800_do_emulation_paused ( );
            };
        };
    };
}


/*
 * 
 * Sem si odskocime, pokud jsme zastavili emulator a potrebujeme vykreslit kompletni screeen.
 * 
 */
void mz800_flush_full_screen ( void ) {

    if ( g_gdg.screen_changes | g_gdg.border_changes ) {

        unsigned beam_row = g_gdg.beam_row;

        do {

            if ( g_gdg.screen_changes ) {
                if ( ( g_gdg.beam_row >= VIDEO_BEAM_CANVAS_FIRST_ROW ) && ( g_gdg.beam_row <= VIDEO_BEAM_CANVAS_LAST_ROW ) ) {
                    if ( !DMD_TEST_MZ700 ) {
                        framebuffer_MZ800_screen_row_fill ( VIDEO_CANVAS_WIDTH );
                    } else {
                        framebuffer_update_MZ700_screen_row ( );
                    };
                };
                g_gdg.screen_need_update_from = 0;
            };

            if ( g_gdg.border_changes ) {
                if ( g_gdg.beam_row <= VIDEO_BEAM_DISPLAY_LAST_ROW ) {
                    framebuffer_border_row_fill ( );
                    g_gdg.last_updated_border_pixel = 0;
                };
            };

            if ( g_gdg.beam_row < VIDEO_SCREEN_HEIGHT - 1 ) {
                g_gdg.beam_row++;
            } else {
                g_gdg.beam_row = 0;
            };

        } while ( g_gdg.beam_row != beam_row );

        if ( g_gdg.screen_changes ) {
            g_gdg.framebuffer_state |= FB_STATE_SCREEN_CHANGED;
        };

        if ( g_gdg.border_changes ) {
            g_gdg.framebuffer_state |= FB_STATE_BORDER_CHANGED;
        };
    };


    if ( g_gdg.framebuffer_state || g_iface_sdl.redraw_full_screen_request ) {
        iface_sdl_update_window ( );
        g_gdg.framebuffer_state = FB_STATE_NOT_CHANGED;
    };
    make_picture_time = 0;
}


void mz800_switch_emulation_speed ( unsigned value ) {

    value &= 1;
    if ( g_mz800.use_max_emulation_speed == value ) return;

    g_mz800.use_max_emulation_speed = value;
    if ( g_mz800.use_max_emulation_speed ) {
        printf ( "Fast emulation speed.\n" );
    } else {
        printf ( "Slow emulation speed.\n" );
    };
    ui_main_update_cpu_speed_menu ( g_mz800.use_max_emulation_speed );
}


void mz800_pause_emulation ( unsigned value ) {

    value &= 1;
    if ( value == g_mz800.emulation_paused ) return;

    if ( value ) {
        //printf ( "Emulation pause requested\n" );
        SET_MZ800_EVENT ( EVENT_USER_INTERFACE, 0 );
        //} else if ( value != g_mz800.emulation_paused ) {
        //printf ( "Continue in emulation - PC: 0x%04x.\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );
    };

    g_mz800.emulation_paused = value;
    ui_main_update_emulation_state ( g_mz800.emulation_paused );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    if ( TEST_DEBUGGER_ACTIVE ) {
        if ( value ) {
            ui_debugger_hide_spinner_window ( );
        } else {
            ui_debugger_show_spinner_window ( );
        };
    };
#endif
}
