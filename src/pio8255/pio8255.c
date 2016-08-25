/* 
 * File:   pio8255.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 21. června 2015, 23:13
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
#include <string.h>

#include "z80ex/include/z80ex.h"

#include "pio8255.h"
#include "iface_sdl/iface_sdl_keyboard.h"
#include "mz800.h"
#include "gdg/gdg.h"
#include "audio.h"
#include "ctc8253/ctc8253.h"
#include "cmt/cmt.h"

//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


/*
 * Emulace PIO8255
 *
 *
 */


#define DEF_PIO8255_PORTA	0x00
#define DEF_PIO8255_PORTB	0x01
#define DEF_PIO8255_PORTC	0x02
#define DEF_PIO8255_MASTER	0x03


st_PIO8255 g_pio8255;


void pio8255_keyboard_matrix_reset ( void ) {
    memset ( &g_pio8255.keyboard_matrix, 0xff, sizeof ( g_pio8255.keyboard_matrix ) );
}


void pio8255_init ( void ) {
    KEYBOARD_MATRIX_RESET ( );
    g_pio8255.signal_PA = 0x00;
    g_pio8255.signal_PA_keybord_column = 0x00;
    g_pio8255.signal_PC = 0x00;
}


void pio8255_write ( int addr, Z80EX_BYTE value ) {

    int bit_setres;



    switch ( addr ) {

        case DEF_PIO8255_PORTA:
            //DBGPRINTF ( DBGINF, "addr = %d (PORT_A), value = 0x%02x )\n", addr, value );
            g_pio8255.signal_PA = value;
            /* TODO: vzorkovani klavesnice co se stane, kdyz se posle vyssi hodnota, nez 9 ? */
            if ( 9 >= ( value & 0x0f ) ) {
                g_pio8255.signal_PA_keybord_column = value & 0x0f;
            };
            break;

        case DEF_PIO8255_PORTC:

            //DBGPRINTF ( DBGINF, "addr = %d (PORT_C), value = 0x%02x )\n", addr, value );
            //printf ( "WR addr = %d (PORT_C), value = 0x%02x )\n", addr, value );

            /* blokovani CTC0 - zvukovy vystup */
            g_pio8255.signal_pc00 = ( value >> 0 ) & 0x01;
            DBGPRINTF ( DBGINF, "audio ctc0 mask (pc00): %d\n", g_pio8255.signal_pc00 );
            audio_ctc0_changed ( ( CTC8253_OUT ( 0 ) & CTC_AUDIO_MASK ), g_gdg.elapsed_screen_ticks );

            /* data do CMT */
            g_pio8255.signal_pc01 = ( value >> 1 ) & 0x01;

            /* blokovani CTC2 - preruseni z CTC */
            g_pio8255.signal_pc02 = ( value >> 2 ) & 0x01;
            DBGPRINTF ( DBGINF, "interrupt ctc2 mask (pc02): %d\n", g_pio8255.signal_pc02 );
            mz800_ctc2_interrupt_handle ( );

            /* rizeni motoru CMT - nabezna hrana provede zmenu */
            unsigned old_pc03_state = g_pio8255.signal_pc03;
            g_pio8255.signal_pc03 = ( value >> 3 ) & 0x01;
            DBGPRINTF ( DBGINF, "cmt motor driver (pc03): %d\n", g_pio8255.signal_pc03 );
            if ( ( old_pc03_state == 0 ) && ( g_pio8255.signal_pc03 == 1 ) ) {
                g_pio8255.signal_pc04 = ( ~g_pio8255.signal_pc04 ) & 1;
                DBGPRINTF ( DBGINF, "set CMT motor (pc04 - input): %d\n", g_pio8255.signal_pc04 );
            };
            break;

        case DEF_PIO8255_MASTER:

            DBGPRINTF ( DBGINF, "addr = %d (MASTER_PORT), value = 0x%02x\n", addr, value );

            if ( value & 0x80 ) {
                if ( value == 0x8a ) {
                    /* nastaveni MODE0, PortA: out, PortC_up: in, PortB: in, PortC_down: out */
                    /* vystupy jsou nastaveny na 0 */
                    g_pio8255.signal_PA = 0x00;
                    g_pio8255.signal_PA_keybord_column = 0x00;
                    g_pio8255.signal_pc00 = 0;
                    g_pio8255.signal_pc02 = 0;
                    DBGPRINTF ( DBGINF, "reset - pc00 - pc03 = 0x00\n" );

                    audio_ctc0_changed ( ( CTC8253_OUT ( 0 ) & CTC_AUDIO_MASK ), g_gdg.elapsed_screen_ticks );
                    mz800_ctc2_interrupt_handle ( );
#if ( DBGLEVEL & DBGWAR )
                } else {
                    DBGPRINTF ( DBGWAR, "addr = %d, value = 0x%02x - UNSUPORTED MODE! PC: 0x%04x\n", addr, value, z80ex_get_reg ( g_mz800.cpu, regPC ) );
#endif
                };
            } else {
                bit_setres = ( value >> 1 ) & 0x07;

                if ( bit_setres == 0 ) {
                    g_pio8255.signal_pc00 = value & 0x01;
                    DBGPRINTF ( DBGINF, "audio ctc0 mask (pc00): %d\n", g_pio8255.signal_pc00 );
                    audio_ctc0_changed ( ( CTC8253_OUT ( 0 ) & CTC_AUDIO_MASK ), g_gdg.elapsed_screen_ticks );

                } else if ( bit_setres == 1 ) {
                    g_pio8255.signal_pc01 = value & 0x01;

                } else if ( bit_setres == 2 ) {
                    g_pio8255.signal_pc02 = value & 0x01;
                    DBGPRINTF ( DBGINF, "interrupt ctc2 mask (pc02): %d\n", g_pio8255.signal_pc02 );
                    mz800_ctc2_interrupt_handle ( );

                } else if ( bit_setres == 3 ) {
                    /* rizeni motoru CMT - nabezna hrana provede zmenu */
                    unsigned old_pc03_state = g_pio8255.signal_pc03;
                    g_pio8255.signal_pc03 = value & 0x01;
                    DBGPRINTF ( DBGINF, "cmt motor driver (pc03): %d\n", g_pio8255.signal_pc03 );
                    if ( ( old_pc03_state == 0 ) && ( g_pio8255.signal_pc03 == 1 ) ) {
                        g_pio8255.signal_pc04 = ( ~g_pio8255.signal_pc04 ) & 1;
                        DBGPRINTF ( DBGINF, "set CMT motor (pc04 - input): %d\n", g_pio8255.signal_pc04 );
                    };

                } else {
                    /* TODO: ostatni bity nas budou zajimat pozdeji:) */
                    printf ( "NOT IMPL. - bit: %d, value: %d\n", bit_setres, value & 1 );
                };
            };
            break;

    };

#if 0
    static unsigned int last_pc01_change = 0;
    static int last_pc01_state = 0;

    if ( g_pio8255.signal_pc01 != last_pc01_state ) {
        printf ( "CMT: %d, %d\n", last_pc01_state, g_timer.counter_gdg - last_pc01_change );
        last_pc01_state = g_pio8255.signal_pc01;
        last_pc01_change = g_timer.counter_gdg;
    };
#endif
}


Z80EX_BYTE pio8255_read ( int addr ) {

    Z80EX_BYTE retval;

    switch ( addr ) {

        case DEF_PIO8255_PORTA:
            return g_pio8255.signal_PA;
            break;

        case DEF_PIO8255_PORTB:

            iface_sdl_keybord_scan ( );
            //BGPRINTF ( DBGINF, "addr = %d (PORT_B - key scan: %d ), value = 0x%02x, PC = 0x%04x\n", addr, g_pio8255.signal_PA_keybord_column, g_pio8255.keyboard_matrix [ g_pio8255.signal_PA_keybord_column ], z80ex_get_reg ( g_mz800.cpu, regPC ) );
            return g_pio8255.keyboard_matrix [ g_pio8255.signal_PA_keybord_column ];

            /* TODO: prozatim mame jen ty nejpodstatnejsi bity */
        case DEF_PIO8255_PORTC:
            /*
             * 0. blokovani CTC0_OUT
             * 1. data do CMT
             * 2. zakaz preruseni z CTC
             * 3. rizeni motoru CMT 0 - 1 - 0
             * 4. test stavu motoru CMT
             * 5. data z CMT
             * 6. cursor timer
             * 7. VBLNK
             *
             */

            retval = 0x00;
            retval |= SIGNAL_GDG_VBLNK ? 1 << 7 : 0;
            retval |= ( ( g_gdg.elapsed_total_screens / 25 ) & 1 ) << 6;
            retval |= g_cmt.output_signal << 5;
            //retval |= g_pio8255.signal_pc04 << 4;
            retval |= SIGNAL_GDG_VBLNK << 4; /* simulujeme promenlivy stav motoru (jinak neni mozne spustut hru "24" - musel by se manualne menit stav motoru ) */
            retval |= g_pio8255.signal_pc02 << 2;
            retval |= g_pio8255.signal_pc00;

            //DBGPRINTF ( "PIO8255_PORTC read - retval = 0x%02x\n", retval );
            //printf ( "PIO8255_PORTC read - retval = 0x%02x, PC = 0x%04x\n", retval & 0x10, z80ex_get_reg ( g_mz800.cpu, regPC ) );

            return retval;
            break;

        default:
            DBGPRINTF ( DBGWAR, "addr = %d - unsupported addr\n", addr );
            break;
    };
    return 0x00;
}

