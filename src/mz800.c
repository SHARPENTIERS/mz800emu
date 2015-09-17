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

#include <stdio.h>
#include <stdlib.h>

#include "main.h"

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


#include "iface_sdl/iface_sdl.h"
#include "iface_sdl/iface_sdl_audio.h"

#include <SDL_timer.h>
// ve Win32 neni ???
//#include <SDL2/SDL_assert.h>

#include "ui/ui_main.h"
#include "typedefs.h"

#ifdef MZ800_DEBUGGER
#include "debugger/debugger.h"
#endif


#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


struct st_mz800 g_mz800;

/* Volba v tuto chvili uplne vypne, nebo zapne pomaly 1M1_CLK a cmt_step() */
#define SLOW_CTC0_v1   1


void mz800_reset ( void ) {
    printf ( "\nMZ800 Reset!\n\n" );

    memory_reset ( );
    gdg_reset ( );
    //    ctc8253_reset ( );

    z80ex_reset ( g_mz800.cpu );
    pioz80_reset ( );
    fdc_reset ( );

    cmthack_reset ( );

#ifdef MZ800_DEBUGGER
    debugger_update_all ( );
#endif

}


void mz800_exit ( void ) {
    fdc_exit ( );
    ramdisc_exit ( );
    cmthack_exit ( );
    cmt_exit ( );
}


/* tady resime rozdily chovani HW mezi MZ rezimy */
void mz800_set_display_mode ( Z80EX_BYTE dmd_mode ) {

    /* TODO: doplnit ovladani citace kurzoru */

    if ( dmd_mode & REGISTER_DMD_FLAG_MZ700 ) {
        /* rezim MZ-700 */
        ctc8253_gate ( 0, g_gdg.regct53g7, g_gdg.screen_ticks_elapsed );

    } else {
        /* rezim MZ-800 */
        ctc8253_gate ( 0, 1, g_gdg.screen_ticks_elapsed );
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

    mz800_set_cpu_speed ( MZ800_EMULATION_SPEED_NORMAL );
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

#ifdef MZ800_DEBUGGER
    debugger_init ( );
#endif

}



static volatile unsigned make_picture_time = 1;
static volatile unsigned update_status_time = 1;

#ifdef MZ800_DEBUGGER
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

#ifdef MZ800_DEBUGGER
    static unsigned debugger_update_counter = 0;

    if ( !TEST_EMULATION_PAUSED ) {
        if ( g_debugger.active != 0 ) {
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
            if ( g_debugger.active != 0 ) {
                if ( update_debugger_time != 1 ) {
                    update_debugger_time = 1;
                    SET_MZ800_EVENT ( EVENT_USER_INTERFACE, 0 );
                };
            };
        };
#endif

        call_counter = ( ( 1000 / INTERRUPT_TIMER_MS ) ) - 1;
        static unsigned last = 0;
        unsigned i = g_gdg.screens_counter;
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


void mz800_sync ( void ) {

    while ( g_gdg.screen_ticks_elapsed >= g_mz800.event.ticks ) {

        if ( g_mz800.event.event_name >= EVENT_MZ800_INTERRUPT ) {
            g_mz800.event.event_name = g_gdg.event.event_name;
            g_mz800.event.ticks = g_gdg.event.ticks;
            if ( !( g_gdg.screen_ticks_elapsed >= g_mz800.event.ticks ) ) return;
        };


        switch ( g_mz800.event.event_name ) {

            case EVENT_GDG_HBLN_END:
                g_gdg.hbln = HBLN_OFF;

                /* V rezimu MZ-700 aktualizujeme screen framebuffer jakmile skonci HBLN. */
                if ( ( DMD_TEST_MZ700 ) && ( ( g_gdg.beam_row >= DISPLAY_SCREEN_FIRST_ROW ) && ( g_gdg.beam_row <= DISPLAY_SCREEN_LAST_ROW ) ) ) {
                    if ( g_gdg.screen_changes ) {
                        framebuffer_update_MZ700_screen_row ( );
                    };

                    /* Pokud jsme na poslednim pixelu screen area */
                    if ( g_gdg.beam_row == DISPLAY_SCREEN_LAST_ROW ) {
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

                if ( g_gdg.beam_row == DISPLAY_SCREEN_LAST_ROW ) {
                    g_gdg.vbln = VBLN_ACTIVE;
                } else if ( g_gdg.beam_row == DISPLAY_SCREEN_FIRST_ROW - 1 ) {
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
                        framebuffer_MZ800_screen_row_fill ( DISPLAY_SCREEN_WIDTH );
                    };

                    /* Pokud jsme na poslednim pixelu screen area */
                    if ( g_gdg.beam_row == DISPLAY_SCREEN_LAST_ROW ) {
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
                if ( g_gdg.beam_row < DISPLAY_VISIBLE_HEIGHT ) {
                    if ( g_gdg.border_changes ) {
                        framebuffer_border_row_fill ( );
                    };
                    g_gdg.last_updated_border_pixel = DISPLAY_VISIBLE_LAST_COLUMN + 1;

                    /* Pokud jsme na poslednim pixelu visible area */
                    if ( g_gdg.beam_row == DISPLAY_VISIBLE_LAST_ROW ) {
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


            case EVENT_GDG_BEAM_ROW_END:
                g_gdg.sts_hsync = HSYN_OFF;
                g_gdg.tempo_divider++;
                /* Na mem MZ800 ma TEMPO pravdepodobne cca 34 Hz - tedy od oka :) */
                if ( g_gdg.tempo_divider == 229 ) {
                    g_gdg.tempo_divider = 0;
                    g_gdg.tempo++;
                };

                /* Muzeme vynulovat update pozici borderu ve framebufferu? */
                /* (Pokud ma jinou hodnotu, tak to znamena, ze pres OUT doslo ke zmene radku, ktery teprve nastane.) */
                if ( g_gdg.last_updated_border_pixel == DISPLAY_VISIBLE_LAST_COLUMN + 1 ) {
                    g_gdg.last_updated_border_pixel = 0;
                };

                g_gdg.screen_need_update_from = 0;

                if ( g_gdg.beam_row < BEAM_TOTAL_ROWS - 1 ) {

                    g_gdg.beam_row++;

                } else {

#if 0
                    if ( pool_events_time ) {
                        iface_sdl_pool_window_events ( );
                        pool_events_time = 0;
                    };
#endif
                    static unsigned last_make_picture_time;
                    if ( ( g_mz800.emulation_speed == 0 ) || ( last_make_picture_time != make_picture_time ) ) {
                        last_make_picture_time = make_picture_time;

                        debugger_animation ( );

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
#if 0

                        } else if ( g_mz800.status_changed ) {
                            iface_sdl_render_status_line ( );
#endif
                        };
                        audio_buffer_fill ( );
                        audio_sdl_wait_to_cycle_done ( );
                        //make_picture_time = 0;
                    };

                    g_gdg.screens_counter++;
                    g_gdg.screen_ticks_elapsed -= ( PICTURE_TICKS - 1 );
                    g_gdg.beam_row = 0;

                    audio_sdl_start_cycle ( );

                };
                break;

                /* tyto eventy neni potreba zde resit */
            case EVENT_USER_INTERFACE:
            case EVENT_MZ800_INTERRUPT:
                break;
        };


        /* Presun na dalsi GDG event */
        if ( g_gdg.event.event_name == EVENT_GDG_BEAM_ROW_END ) {
            g_gdg.event.event_name = 0;
        } else {
            g_gdg.event.event_name++;
        };

        while ( 1 ) {
            if ( ( g_gdgevent[ g_gdg.event.event_name ].start_row <= g_gdg.beam_row ) && ( ( g_gdgevent [ g_gdg.event.event_name ].start_row + g_gdgevent[ g_gdg.event.event_name ].num_rows - 1 ) >= g_gdg.beam_row ) ) {
                g_gdg.event.ticks = g_gdg.beam_row * BEAM_TOTAL_COLS + g_gdgevent[ g_gdg.event.event_name ].event_column;
                break;
            };
            g_gdg.event.event_name++;
        };

        g_mz800.event.event_name = g_gdg.event.event_name;
        g_mz800.event.ticks = g_gdg.event.ticks;
    };
}


/* TODO: casovani (MZ700 VRAM casovani) neni jeste uplne OK - viz FX soundtrack 3 - border pri zmacknute klavese je v trochu jine poloze, nez na realnem HW */
void mz800_sync_inside_cpu ( en_INSIDEOP insideop ) {

#ifdef MZ800_DEBUGGER
    if ( TEST_DEBUGGER_MEMOP_CALL ) return;
#endif

    g_mz800.event_locked = 1;

    if ( g_mz800.event.event_name >= EVENT_MZ800_INTERRUPT ) {
        g_mz800.event.event_name = g_gdg.event.event_name;
        g_mz800.event.ticks = g_gdg.event.ticks;
    };

    unsigned insideop_ticks = 0;

    if ( insideop == INSIDEOP_MZ700_NOTHBLN_VRAM_MREQ ) {

        unsigned gdg_ticks_to_hbln = BEAM_HBLN_START_COLUMN - BEAM_COL ( g_gdg.screen_ticks_elapsed );
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



#if SLOW_CTC0_v1
    g_gdg.screen_ticks_elapsed -= g_gdg.ctc0clk;
    insideop_ticks += g_gdg.ctc0clk;
    while ( insideop_ticks > GDGCLK_1M1_DIVIDER - 1 ) {

        g_gdg.screen_ticks_elapsed += GDGCLK_1M1_DIVIDER;
        insideop_ticks -= GDGCLK_1M1_DIVIDER;

        ctc8253_clkfall ( CTC_CS0, g_gdg.screen_ticks_elapsed );

        /* TODO: prozatim si sem povesime i pomaly cmt_step() */
        cmt_step ( );

    };
    g_gdg.ctc0clk = insideop_ticks;
    g_gdg.screen_ticks_elapsed += insideop_ticks;
#else
    g_gdg.screen_ticks_elapsed += insideop_ticks;
#endif

    if ( g_gdg.screen_ticks_elapsed >= g_gdg.event.ticks ) {
        mz800_sync ( );
    };

    /* Tohle je potreba, aby nas to vyhnalo z instrukcni smycky do obsluhy interruptu */
    if ( g_mz800.interrupt ) {
        g_mz800.event.event_name = EVENT_MZ800_INTERRUPT;
        g_mz800.event.ticks = 0;
    };

    g_mz800.event_locked = 0;
}


void mz800_main ( void ) {

    //            FILE *fp;
    //            if ( ! ( fp = fopen ( "output.txt", "w" ) ) ) {
    //                fprintf ( stderr, "nejde otevrit" );
    //            };

    mz800_reset ( );

    SDL_AddTimer ( INTERRUPT_TIMER_MS, screens_counter_flush, NULL );

    g_mz800.event.event_name = g_gdg.event.event_name;
    g_mz800.event.ticks = g_gdg.event.ticks;


    while ( 1 ) {

        //                fprintf ( fp, "0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );
        //        printf ( "0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );

#if 0
        if ( g_mz800.debug_pc ) {
            printf ( "0x%04x\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );
            //        } else if ( z80ex_get_reg ( g_mz800.cpu, regPC ) == 0x2f49 ) {
            //            printf ( "regA: 0x%02x\n", z80ex_get_reg ( g_mz800.cpu, regAF ) >> 8 );
            //_mz800.debug_pc = 1;
        };
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

        //        instruction_ticks -=  - g_mz800.synchronised_insideop_GDG_ticks;

        //        g_gdg.screen_ticks_elapsed += instruction_ticks;


#if SLOW_CTC0_v1
        g_gdg.screen_ticks_elapsed -= g_gdg.ctc0clk;
        instruction_ticks += g_gdg.ctc0clk;
        while ( instruction_ticks > GDGCLK_1M1_DIVIDER - 1 ) {
            g_gdg.screen_ticks_elapsed += GDGCLK_1M1_DIVIDER;
            instruction_ticks -= GDGCLK_1M1_DIVIDER;

            ctc8253_clkfall ( CTC_CS0, g_gdg.screen_ticks_elapsed );

            /* TODO: prozatim si sem povesime i pomaly cmt_step() */
            cmt_step ( );

        };
        g_gdg.ctc0clk = instruction_ticks;
        g_gdg.screen_ticks_elapsed += instruction_ticks;
#else
        g_gdg.screen_ticks_elapsed += instruction_ticks;
#endif




        if ( g_gdg.screen_ticks_elapsed >= g_mz800.event.ticks ) {


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

#if SLOW_CTC0_v1
                    g_gdg.screen_ticks_elapsed -= g_gdg.ctc0clk;
                    instruction_ticks += g_gdg.ctc0clk;
                    while ( instruction_ticks > GDGCLK_1M1_DIVIDER - 1 ) {
                        g_gdg.screen_ticks_elapsed += GDGCLK_1M1_DIVIDER;
                        instruction_ticks -= GDGCLK_1M1_DIVIDER;

                        ctc8253_clkfall ( CTC_CS0, g_gdg.screen_ticks_elapsed );
                    };
                    g_gdg.ctc0clk = instruction_ticks;
                    g_gdg.screen_ticks_elapsed += instruction_ticks;
#else
                    g_gdg.screen_ticks_elapsed += instruction_ticks;
#endif
                    if ( g_gdg.screen_ticks_elapsed >= g_mz800.event.ticks ) {
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

                //printf ( "Emulation paused - PC: 0x%04x.\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );

                /* TODO: nastavit framebuffer a display tak, aby to odpovidalo aktualni pozici paprsku */

#ifdef MZ800_DEBUGGER
                debugger_update_all ( );
                debugger_step_call ( 0 );

                iface_sdl_render_status_line ( );

                while ( TEST_EMULATION_PAUSED && ( !TEST_DEBUGGER_STEP_CALL ) ) {
#else          
                while ( TEST_EMULATION_PAUSED ) {
#endif

                    iface_sdl_pool_all_events ( );
                    ui_iteration ( );

                    if ( g_iface_sdl.redraw_full_screen_request ) {
                        iface_sdl_update_window ( );
                    };
                };

#ifdef MZ800_DEBUGGER
                if ( TEST_DEBUGGER_STEP_CALL ) {
                    SET_MZ800_EVENT ( EVENT_USER_INTERFACE, 0 );
                };
#endif                

                /* Konec pauzy */

                //            } else if ( update_debugger_time == 1 ) {
                //                update_debugger_time = 0;
                //                debugger_update_all ( );
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
                if ( ( g_gdg.beam_row >= DISPLAY_SCREEN_FIRST_ROW ) && ( g_gdg.beam_row <= DISPLAY_SCREEN_LAST_ROW ) ) {
                    if ( !DMD_TEST_MZ700 ) {
                        framebuffer_MZ800_screen_row_fill ( DISPLAY_SCREEN_WIDTH );
                    } else {
                        framebuffer_update_MZ700_screen_row ( );
                    };
                };
                g_gdg.screen_need_update_from = 0;
            };

            if ( g_gdg.border_changes ) {
                if ( g_gdg.beam_row <= DISPLAY_VISIBLE_LAST_ROW ) {
                    framebuffer_border_row_fill ( );
                    g_gdg.last_updated_border_pixel = 0;
                };
            };

            if ( g_gdg.beam_row < BEAM_TOTAL_ROWS - 1 ) {
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


void mz800_set_cpu_speed ( unsigned value ) {

    value &= 1;
    if ( g_mz800.emulation_speed == value ) return;

    g_mz800.emulation_speed = value;
    if ( g_mz800.emulation_speed ) {
        printf ( "Fast emulation sped.\n" );
    } else {
        printf ( "Slow emulation sped.\n" );
    };
    ui_main_update_cpu_speed_menu ( g_mz800.emulation_speed );
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
}
