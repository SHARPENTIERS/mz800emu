/* 
 * File:   ctc8253.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. června 2015, 11:47
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


#include "z80ex/include/z80ex.h"

#include "ctc8253/ctc8253.h"
#include "gdg/gdg.h"
#include "gdg/video.h"
#include "mz800.h"
#include "pioz80/pioz80.h"
#include "pio8255/pio8255.h"
#include "audio.h"

#ifdef MZ800_DEBUGGER
#include "debugger/debugger.h"
#endif

//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"

struct st_CTC8253 g_ctc8253[3];


void ctc8253_ctc0_output_event ( unsigned value, unsigned event_ticks ) {
    //DBGPRINTF ( DBGINF, "CTC0 output event! (%d)\n", value );
    pioz80_port_event ( PIOZ80_PORT_A, 4, ~value & 0x01 ); /* pripojeno pres invertor !!! */

    /* Bugfix pro hru Ralye (Tatra-sys HD cpm disk 5) - nastavi ctc0 mode: 3, preset: 2 a povoli audio (pc00) - na Sharpu ten zvuk zrejme neprojde filtrem */
    if ( !( ( g_ctc8253[CTC_CS0].mode == CTC_MODE3 ) && ( g_ctc8253[CTC_CS0].preset_value == 2 ) ) ) {
        audio_ctc0_changed ( ( value & CTC_AUDIO_MASK ), event_ticks );
    };
}


void ctc8253_ctc1_output_event ( unsigned value, unsigned event_ticks ) {
    //DBGPRINTF ( DBGINF, "CTC1 output event! (%d)\n", value );
    if ( value != 0 ) return;
    ctc8253_clkfall ( CTC_CS2, event_ticks );
}


void ctc8253_ctc2_output_event ( unsigned value, unsigned event_ticks ) {
    DBGPRINTF ( DBGINF, "CTC2 output event! (%d)\n", value );
    mz800_ctc2_interrupt_handle ( );
}


void ctc8253_set_out ( unsigned cs, unsigned value, unsigned event_ticks ) {
    /* zadna zmena */
    if ( g_ctc8253 [ cs ].out == value ) return;

    g_ctc8253 [ cs ].out = value;

    if ( g_ctc8253 [ cs ].output_cb != NULL ) {
        g_ctc8253 [ cs ].output_cb ( value, event_ticks );
    };
}


void ctc8253_init ( void ) {
    g_ctc8253 [ 0 ].output_cb = ctc8253_ctc0_output_event;
    g_ctc8253 [ 1 ].output_cb = ctc8253_ctc1_output_event;
    g_ctc8253 [ 2 ].output_cb = ctc8253_ctc2_output_event;
    ctc8253_gate ( 0, 0, 0 );
    ctc8253_gate ( 1, 1, 0 );
    ctc8253_gate ( 2, 1, 0 );

    unsigned cs;
    for ( cs = 0; cs < CTC_CS_ILLEGAL; cs++ ) {
        g_ctc8253 [ cs ].state = CTC_STATE_INIT_DONE;
        g_ctc8253 [ cs ].load_done = 0;
        g_ctc8253 [ cs ].mode = CTC_MODE0;
        g_ctc8253 [ cs ].out = 0;
        g_ctc8253 [ cs ].value = 0;
        g_ctc8253 [ cs ].preset_value = 0xffff;
        g_ctc8253 [ cs ].rl_byte = 0;
        g_ctc8253 [ cs ].latch_op = 0;
        g_ctc8253 [ cs ].rlf = CTC_RLF_LSBMSB;
        g_ctc8253 [ cs ].bcd = 0;
    };

}


Z80EX_BYTE ctc8253_read_byte ( unsigned cs ) {

    Z80EX_BYTE retval = 0;

    unsigned value = ( g_ctc8253 [ cs ].latch_op == 1 ) ? g_ctc8253 [ cs ].read_latch : g_ctc8253 [ cs ].value;

    switch ( g_ctc8253 [ cs ].rlf ) {

        case CTC_RLF_LSB:
#ifdef MZ800_DEBUGGER
            if ( !TEST_DEBUGGER_MEMOP_CALL ) {
#endif
                g_ctc8253 [ cs ].latch_op = 0;
#ifdef MZ800_DEBUGGER
            };
#endif
            retval = ( value & 0xff );
            break;

        case CTC_RLF_MSB:
            
            
#ifdef MZ800_DEBUGGER
            if ( !TEST_DEBUGGER_MEMOP_CALL ) {
#endif
                g_ctc8253 [ cs ].latch_op = 0;
#ifdef MZ800_DEBUGGER
            };
#endif
            
            retval = ( value >> 8 ) & 0xff;
            break;

        case CTC_RLF_LSBMSB:
            if ( g_ctc8253 [ cs ].rl_byte == 0 ) {

                
#ifdef MZ800_DEBUGGER
                if ( !TEST_DEBUGGER_MEMOP_CALL ) {
#endif
                    g_ctc8253 [ cs ].rl_byte = 1;
#ifdef MZ800_DEBUGGER
                };
#endif

                retval = ( value & 0xff );
            } else {


#ifdef MZ800_DEBUGGER
                if ( !TEST_DEBUGGER_MEMOP_CALL ) {
#endif
                    g_ctc8253 [ cs ].rl_byte = 0;
                    g_ctc8253 [ cs ].latch_op = 0;
#ifdef MZ800_DEBUGGER
                };
#endif

                retval = ( value >> 8 ) & 0xff;
            };
            break;
    };

    //DBGPRINTF ( DBGINF, "Read CTC addr: %d, value: 0x%02x\n", cs, retval );

    return retval;
}


void ctc8253_write_byte ( unsigned addr, Z80EX_BYTE value ) {

    addr = addr & 0x03;

    //printf ( "WR 8253 - addr: %d, value: 0x%02x, PC: 0x%04x\n", addr, value, z80ex_get_reg ( g_mz800.cpu, regPC ) );

    if ( addr == CTCADDR_CWREG ) {
        /* Zapis do CW registru */

        en_CTC_CS cs = value >> 6;

        /* Nepovolena adresa - nereagujeme */
        if ( cs == CTC_CS_ILLEGAL ) return;

        unsigned rlf = ( value >> 4 ) & 0x03;

        g_ctc8253 [ cs ].rl_byte = 0;

        /* LatchOp - priprava na cteni */
        if ( rlf == 0 ) {
            //DBGPRINTF ( DBGINF, "LatchOP CTC: %d\n", cs );
            g_ctc8253 [ cs ].latch_op = 1;
            g_ctc8253 [ cs ].read_latch = g_ctc8253 [ cs ].value;
            /* TODO: proverit jak se chova read latch, zmeni se pokud se dokoncil countdown, nebo pokud prisel trigger, atp. ? */
            return;
        };

        g_ctc8253 [ cs ].latch_op = 0;
        g_ctc8253 [ cs ].rlf = rlf;

        en_CTC_MODE mode = ( value >> 1 ) & 0x07;
        if ( mode > CTC_MODE5 ) {
            mode -= 2;
        };
        g_ctc8253 [ cs ].mode = mode;
        g_ctc8253 [ cs ].bcd = value & 0x01;
        g_ctc8253 [ cs ].state = CTC_STATE_INIT;
        g_ctc8253 [ cs ].load_done = 0;
        /* skutecny 8253 zrejme pri initu na value nesaha */
        //g_ctc8253 [ cs ].value = 0;

        DBGPRINTF ( DBGINF, "INIT CTC - 0x%02x - addr: %d, RLF: %d, MODE: %d, BCD: %d\n", value, cs, g_ctc8253 [ cs ].rlf, g_ctc8253 [ cs ].mode, g_ctc8253 [ cs ].bcd );

        unsigned output_state = ( g_ctc8253 [ cs ].mode == CTC_MODE0 ) ? 0 : 1;
        ctc8253_set_out ( cs, output_state, g_gdg.screen_ticks_elapsed );

#if ( DBGLEVEL & DBGWAR )
        if ( g_ctc8253 [ cs ].mode > CTC_MODE3 ) {
            DBGPRINTF ( DBGWAR, "Unsupported mode: %d on CTC: %d\n", g_ctc8253 [ cs ].mode, cs );
        };
#endif        

    } else {

        /* Zapis do citace */
        en_CTC_CS cs = addr;

        g_ctc8253 [ cs ].latch_op = 0;

        /* Zapocali jsme LOAD v MODE 0 */
        if ( g_ctc8253 [ cs ].mode == CTC_MODE0 ) {
            if ( g_ctc8253 [ cs ].state > CTC_STATE_INIT_DONE ) {
                g_ctc8253 [ cs ].state = CTC_STATE_LOAD;
                ctc8253_set_out ( cs, 0, g_gdg.screen_ticks_elapsed );
            };
        };
        DBGPRINTF ( DBGINF, "LOAD CTC addr: %d, value: 0x%02x, PC = 0x%04x\n", cs, value, z80ex_get_reg ( g_mz800.cpu, regPC ) );


        switch ( g_ctc8253 [ cs ].rlf ) {

            case CTC_RLF_LSB:
                g_ctc8253 [ cs ].preset_latch = value;
                break;

            case CTC_RLF_MSB:
                g_ctc8253 [ cs ].preset_latch = value << 8;
                break;

            case CTC_RLF_LSBMSB:
                if ( g_ctc8253 [ cs ].rl_byte == 0 ) {
                    g_ctc8253 [ cs ].preset_latch = value;
                    g_ctc8253 [ cs ].rl_byte = 1;
                    return;
                } else {
                    g_ctc8253 [ cs ].preset_latch |= value << 8;
                    g_ctc8253 [ cs ].rl_byte = 0;
                };
                break;
        };

        g_ctc8253 [ cs ].preset_value = ( g_ctc8253 [ cs ].preset_latch == 0 ) ? 0x10000 : g_ctc8253 [ cs ].preset_latch;

        if ( g_ctc8253 [ cs ].mode == CTC_MODE3 ) {

            if ( g_ctc8253 [ cs ].preset_value == 1 ) {
                g_ctc8253 [ cs ].preset_value = 0x10001;
            };

            g_ctc8253 [ cs ].mode3_half_value = g_ctc8253 [ cs ].preset_value;
            if ( g_ctc8253 [ cs ].mode3_half_value & 1 ) {
                g_ctc8253 [ cs ].mode3_half_value++;
            };
            g_ctc8253 [ cs ].mode3_half_value >>= 1;
        };

        /* Dokoncen LOAD */

        if ( g_ctc8253 [ cs ].state < CTC_STATE_LOAD_DONE ) {
            if ( g_ctc8253 [ cs ].state == CTC_STATE_INIT ) {
                g_ctc8253 [ cs ].load_done = 1;
            } else {
                g_ctc8253 [ cs ].state = CTC_STATE_LOAD_DONE;
            };
        } else if ( g_ctc8253 [ cs ].state == CTC_STATE_MODE1_TRIGGER_ERROR ) {
            g_ctc8253 [ cs ].state = CTC_STATE_PRESET32;
        };

    };
}


void ctc8253_clkfall ( unsigned cs, unsigned event_ticks ) {

    switch ( g_ctc8253 [ cs ].mode ) {

        case CTC_MODE0:
            if ( g_ctc8253 [ cs ].state >= CTC_STATE_COUNTDOWN ) {
                g_ctc8253 [ cs ].value--;
                if ( g_ctc8253 [ cs ].value == 0x0000 ) {
                    ctc8253_set_out ( cs, 1, event_ticks );
                    g_ctc8253 [ cs ].state = CTC_STATE_BLIND_COUNT;
                    g_ctc8253 [ cs ].value = 0xffff;
                };
                return;

            } else if ( g_ctc8253 [ cs ].state == CTC_STATE_LOAD_DONE ) {

                g_ctc8253 [ cs ].value = g_ctc8253 [ cs ].preset_value;

                if ( g_ctc8253 [ cs ].gate == 1 ) {
                    g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                } else {
                    g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                };
                return;
            };
            break;


        case CTC_MODE1:
            if ( g_ctc8253 [ cs ].state == CTC_STATE_BLIND_COUNT ) {
                g_ctc8253 [ cs ].value--;
                if ( g_ctc8253 [ cs ].value == 0x0000 ) {
                    g_ctc8253 [ cs ].value = 0xffff;
                };
                return;

            } else if ( g_ctc8253 [ cs ].state == CTC_STATE_COUNTDOWN ) {
                g_ctc8253 [ cs ].value--;
                if ( g_ctc8253 [ cs ].value == 0x0000 ) {
                    ctc8253_set_out ( cs, 1, event_ticks );
                    if ( g_ctc8253 [ cs ].gate == 1 ) {
                        g_ctc8253 [ cs ].state = CTC_STATE_BLIND_COUNT;
                    } else {
                        g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                    };
                };
                return;

            } else if ( g_ctc8253 [ cs ].state == CTC_STATE_LOAD_DONE ) {
                if ( g_ctc8253 [ cs ].gate == 1 ) {
                    g_ctc8253 [ cs ].state = CTC_STATE_BLIND_COUNT;
                } else {
                    g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                };
                return;

            } else if ( g_ctc8253 [ cs ].state == CTC_STATE_PRESET ) {
                g_ctc8253 [ cs ].value = g_ctc8253 [ cs ].preset_value;
                ctc8253_set_out ( cs, 0, event_ticks );
                g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                return;

            } else if ( g_ctc8253 [ cs ].state == CTC_STATE_PRESET32 ) {
                g_ctc8253 [ cs ].value = 32;
                g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                return;
            };
            break;


        case CTC_MODE2:
            if ( g_ctc8253 [ cs ].state == CTC_STATE_COUNTDOWN ) {

                g_ctc8253 [ cs ].value--;

                if ( g_ctc8253 [ cs ].value == 0x0001 ) {
                    ctc8253_set_out ( cs, 0, event_ticks );
                    g_ctc8253 [ cs ].state = CTC_STATE_PRESET;
                };
                return;

            } else if ( ( g_ctc8253 [ cs ].state == CTC_STATE_PRESET ) || ( g_ctc8253 [ cs ].state == CTC_STATE_LOAD_DONE ) ) {

                ctc8253_set_out ( cs, 1, event_ticks );

                g_ctc8253 [ cs ].value = g_ctc8253 [ cs ].preset_value;

                if ( g_ctc8253 [ cs ].value == 0x0001 ) {
                    g_ctc8253 [ cs ].state = CTC_STATE_PRESET_ERROR;
                } else {
                    if ( g_ctc8253 [ cs ].gate == 1 ) {
                        g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                    } else {
                        g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                    };
                };
                return;

            };
            break;


        case CTC_MODE3:

            if ( g_ctc8253 [ cs ].state == CTC_STATE_COUNTDOWN ) {

                g_ctc8253 [ cs ].value--;

                if ( g_ctc8253 [ cs ].value == g_ctc8253 [ cs ].mode3_destination_value ) {

                    if ( g_ctc8253 [ cs ].out == 1 ) {

                        ctc8253_set_out ( cs, 0, event_ticks );

                        g_ctc8253 [ cs ].value = g_ctc8253 [ cs ].mode3_half_value;
                        g_ctc8253 [ cs ].mode3_destination_value = 0;

                    } else {

                        ctc8253_set_out ( cs, 1, event_ticks );

                        g_ctc8253 [ cs ].value = g_ctc8253 [ cs ].preset_value;
                        g_ctc8253 [ cs ].mode3_destination_value = g_ctc8253 [ cs ].mode3_half_value;

                        if ( g_ctc8253 [ cs ].gate == 1 ) {
                            g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                        } else {
                            g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                        };

                    };
                };
                return;

            } else if ( ( g_ctc8253 [ cs ].state == CTC_STATE_PRESET ) || ( g_ctc8253 [ cs ].state == CTC_STATE_LOAD_DONE ) ) {
                ctc8253_set_out ( cs, 1, event_ticks );

                g_ctc8253 [ cs ].value = g_ctc8253 [ cs ].preset_value;
                g_ctc8253 [ cs ].mode3_destination_value = g_ctc8253 [ cs ].mode3_half_value;

                if ( g_ctc8253 [ cs ].gate == 1 ) {
                    g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                } else {
                    g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                };
                return;
            };
            break;


        case CTC_MODE4:
        case CTC_MODE5:
            //DBGPRINTF ( DBGWARN, "Unsupported mode: %d, on CTC: %d\n", g_ctc8253 [ cs ].mode, cs );
            return;
            break;
    };


    if ( g_ctc8253 [ cs ].state == CTC_STATE_INIT ) {
        if ( g_ctc8253 [ cs ].load_done == 1 ) {
            g_ctc8253 [ cs ].state = CTC_STATE_LOAD_DONE;
            g_ctc8253 [ cs ].load_done = 0;
        } else {
            g_ctc8253 [ cs ].state = CTC_STATE_INIT_DONE;
        }
    };

}


void ctc8253_gate ( unsigned cs, unsigned gate, unsigned event_screen_ticks ) {
    gate = gate & 0x01;

    /* Gate se nezmenila - jdeme pryc */
    if ( g_ctc8253 [ cs ].gate == gate ) return;

    g_ctc8253 [ cs ].gate = gate;

    if ( g_ctc8253 [ cs ].state == CTC_STATE_INIT ) return;

    switch ( g_ctc8253 [ cs ].mode ) {

        case CTC_MODE0:
            if ( g_ctc8253 [ cs ].gate == 0 ) {
                g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
            } else {
                if ( g_ctc8253 [ cs ].out == 0 ) {
                    g_ctc8253 [ cs ].state = CTC_STATE_COUNTDOWN;
                } else {
                    g_ctc8253 [ cs ].state = CTC_STATE_BLIND_COUNT;
                };
            };
            break;


        case CTC_MODE1:
            if ( g_ctc8253 [ cs ].gate == 1 ) {
                if ( ( g_ctc8253 [ cs ].state == CTC_STATE_LOAD_DONE ) || ( g_ctc8253 [ cs ].state == CTC_STATE_WAIT_GATE1 ) || ( g_ctc8253 [ cs ].state == CTC_STATE_COUNTDOWN ) ) {
                    g_ctc8253 [ cs ].state = CTC_STATE_PRESET;
                } else if ( g_ctc8253 [ cs ].state == CTC_STATE_INIT_DONE ) {
                    /* Nabezna GATE prisla drive, nez byl dokoncen LOAD */
                    ctc8253_set_out ( cs, 0, event_screen_ticks );
                    g_ctc8253 [ cs ].state = CTC_STATE_MODE1_TRIGGER_ERROR;
                };
            } else if ( g_ctc8253 [ cs ].state == CTC_STATE_BLIND_COUNT ) {
                /* v tuto chvili by se melo jednat o sestupnou hranu GATE */
                g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
            };
            break;

        case CTC_MODE2:
        case CTC_MODE3:
            if ( g_ctc8253 [ cs ].gate == 0 ) {
                if ( ( g_ctc8253 [ cs ].state == CTC_STATE_COUNTDOWN ) || ( g_ctc8253 [ cs ].state == CTC_STATE_PRESET ) ) {
                    ctc8253_set_out ( cs, 1, event_screen_ticks );
                    g_ctc8253 [ cs ].state = CTC_STATE_WAIT_GATE1;
                };
            } else if ( ( g_ctc8253 [ cs ].state == CTC_STATE_WAIT_GATE1 ) && ( g_ctc8253 [ cs ].gate == 1 ) ) {
                g_ctc8253 [ cs ].state = CTC_STATE_PRESET;
            };

        case CTC_MODE4:
        case CTC_MODE5:
            //DBGPRINTF ( DBGWARN, "Unsupported mode: %d, on CTC: %d\n", g_ctc8253 [ cs ].mode, cs );
            break;
    };
}

