/* 
 * File:   debugger.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. srpna 2015, 16:19
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

#ifdef MZ800_DEBUGGER


#include "debugger.h"
#include "ui/debugger/ui_debugger.h"
#include "mz800.h"
#include "z80ex/include/z80ex.h"
#include "memory/memory.h"
#include "breakpoints.h"

st_DEBUGGER g_debugger;


void debugger_step_call ( unsigned value ) {

    value &= 1;
    if ( value == g_debugger.step_call ) return;

    if ( value ) {
#if 0
        if ( !TEST_EMULATION_PAUSED ) return;
#else
        /* Emulator neni v pauze, takze neprovedeme step, ale prozatim jen zastavime emulaci */
        if ( !TEST_EMULATION_PAUSED ) {
            //mz800_pause_emulation ( 1 );
            ui_debugger_pause_emulation ( );
            return;
        };
#endif
        g_debugger.step_call = 1;
        //printf ( "Debugger step call - PC: 0x%04x.\n", z80ex_get_reg ( g_mz800.cpu, regPC ) );
    } else {
        g_debugger.step_call = 0;
    };
}


void debugger_init ( void ) {
    g_debugger.active = 0;
    g_debugger.memop_call = 0;
    debugger_step_call ( 0 );

    breakpoints_init ( );
}


void debugger_show_main_window ( void ) {
    ui_debugger_show_main_window ( );
    debugger_step_call ( 0 );
    g_debugger.active = 1;
}


void debugger_hide_main_window ( void ) {
    ui_debugger_hide_main_window ( );
    debugger_step_call ( 0 );
    g_debugger.active = 0;
}


void debugger_show_hide_main_window ( void ) {
    if ( g_debugger.active == 0 ) {
        debugger_show_main_window ( );
    } else {
        debugger_hide_main_window ( );
    };
}


void debugger_update_all ( void ) {
    if ( g_debugger.active == 0 ) return;
    ui_debugger_update_all ( );
}


void debugger_animation ( void ) {
    if ( g_debugger.active == 0 ) return;
    ui_debugger_update_animated ( );
}


Z80EX_BYTE debugger_dasm_read_cb ( Z80EX_WORD addr, void *user_data ) {
    g_debugger.memop_call = 1;
    Z80EX_BYTE retval = memory_read_cb ( g_mz800.cpu, addr, 0, NULL );
    g_debugger.memop_call = 0;
    return retval;
}


void debugger_memory_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value ) {
    g_debugger.memop_call = 1;
    memory_write_cb ( g_mz800.cpu, addr, value, NULL );
    g_debugger.memop_call = 0;
}


void debugger_mmap_mount ( unsigned value ) {
    g_memory.map |= value;

    ui_debugger_update_mmap ( );
    ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
    ui_debugger_update_stack ( );
}


void debugger_mmap_umount ( unsigned value ) {
    g_memory.map &= ( ~value ) & 0x0f;

    ui_debugger_update_mmap ( );
    ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
    ui_debugger_update_stack ( );
}


void debugger_change_z80_flagbit ( unsigned flagbit, unsigned value ) {

    if ( !TEST_EMULATION_PAUSED ) {
        /* Toggle button neni potreba nastavovat zpet, protoze po pauze dojde k update */
        ui_debugger_pause_emulation ( );
        return;
    };

    Z80EX_WORD af_value = z80ex_get_reg ( g_mz800.cpu, regAF );

    if ( value ) {
        af_value |= 1 << flagbit;
    } else {
        af_value &= ~( 1 << flagbit );
    };

    z80ex_set_reg ( g_mz800.cpu, regAF, af_value );

    ui_debugger_update_registers ( );
}


void debugger_change_z80_register ( Z80_REG_T reg, Z80EX_WORD value ) {

    if ( !TEST_EMULATION_PAUSED ) {
        /* Hodnoty neni potreba nastavovat zpet, protoze po pauze dojde k update */
        ui_debugger_pause_emulation ( );
        return;
    };

    if ( reg == regR ) {
        z80ex_set_reg ( g_mz800.cpu, regR, value & 0x7f );
        z80ex_set_reg ( g_mz800.cpu, regR7, value & 0x80 );
    } else {
        z80ex_set_reg ( g_mz800.cpu, reg, value );
    };

    ui_debugger_update_registers ( );

    if ( reg == regAF ) {
        ui_debugger_update_flag_reg ( );
    } else if ( reg == regPC ) {
        ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
    } else if ( reg == regSP ) {
        ui_debugger_update_stack ( );
    };
}


void debugger_change_dmd ( Z80EX_BYTE value ) {

    if ( !TEST_EMULATION_PAUSED ) {
        /* Hodnotu comboboxu neni potreba nastavovat zpet, protoze po pauze dojde k update */
        ui_debugger_pause_emulation ( );
        return;
    };

    gdg_write_byte ( 0xce, value );

    ui_debugger_update_mmap ( );
    ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
    ui_debugger_update_stack ( );
}


static Z80EX_WORD debugger_a2hex ( char c ) {

    if ( c >= '0' && c <= '9' ) {
        return ( c - '0' );
    };

    if ( c >= 'a' && c <= 'f' ) {
        return ( c - 'a' + 0x0a );
    };

    if ( c >= 'A' && c <= 'F' ) {
        return ( c - 'A' + 0x0a );
    };

    return 0;
}


Z80EX_WORD debuger_text_to_z80_word ( const char *txt ) {

    unsigned length;
    unsigned i;
    Z80EX_WORD value = 0;

    length = strlen ( txt );

    if ( length == 0 ) {
        return 0;
    };

    i = 0;
    while ( length ) {
        value += debugger_a2hex ( txt [ length - 1 ] ) << i;
        length--;
        i += 4;
    };

    return value;
}

#endif

