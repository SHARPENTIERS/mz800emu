/* 
 * File:   debugger.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. srpna 2015, 16:18
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

#ifndef DEBUGGER_H
#define	DEBUGGER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"

    typedef enum en_DEBUGGER_ANIMATED_UPDATES {
        DEBUGGER_ANIMATED_UPDATES_DISABLED = 0, // ukazeme spinner
        DEBUGGER_ANIMATED_UPDATES_ENABLED, // animujeme
    } en_DEBUGGER_ANIMATED_UPDATES;
    
    typedef struct st_DEBUGGER {
        unsigned active;
        unsigned step_call;
        unsigned memop_call;
        en_DEBUGGER_ANIMATED_UPDATES animated_updates;
        unsigned auto_save_breakpoints;
    } st_DEBUGGER;

    extern st_DEBUGGER g_debugger;

#define TEST_DEBUGGER_MEMOP_CALL        ( g_debugger.memop_call != 0 )
#define TEST_DEBUGGER_STEP_CALL         ( g_debugger.step_call != 0 )
#define TEST_DEBUGGER_ACTIVE            ( g_debugger.active != 0 )

    
    extern void debugger_step_call ( unsigned value );
    extern void debugger_init ( void );
    extern void debugger_exit ( void );
    extern void debugger_show_main_window ( void );
    extern void debugger_hide_main_window ( void );
    extern void debugger_show_hide_main_window ( void );
    extern void debugger_update_all ( void );
    extern void debugger_animation ( void );
    extern Z80EX_BYTE debugger_dasm_read_cb ( Z80EX_WORD addr, void *user_data );
    extern void debugger_memory_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );
    extern void debugger_mmap_mount ( unsigned value );
    extern void debugger_mmap_umount ( unsigned value );
    extern void debugger_change_z80_flagbit ( unsigned flagbit, unsigned value );
    extern void debugger_change_z80_register ( Z80_REG_T reg, Z80EX_WORD value );
    extern void debugger_change_dmd ( Z80EX_BYTE value );
    extern Z80EX_WORD debuger_text_to_z80_word ( const char *txt );
    
#define debugger_memory_read_byte(addr) debugger_dasm_read_cb ( addr, NULL )

#ifdef	__cplusplus
}
#endif

#endif	/* DEBUGGER_H */

