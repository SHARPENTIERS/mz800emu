/* 
 * File:   memory.h
 * Author: chaky
 *
 * Created on 15. června 2015, 17:34
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

#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#include "z80ex/include/z80ex.h"

#ifdef MEMORY_MAKE_STATISTICS


    typedef struct st_MEMORY_STATS {
        unsigned long long read[16];
        unsigned long long write[16];
    } st_MEMORY_STATS;

#endif

#define MEMORY_SIZE_RAM         0x10000
#define MEMORY_MEMRAM_POINTS    0x10

    /* Tepla inicializace pameti */
    extern void memory_reset ( void );

    /* Studena inicializace pameti */
    extern void memory_init ( void );


    typedef enum en_MEMORY_LOAD {
        MEMORY_LOAD_MAPED = 0,
        MEMORY_LOAD_RAMONLY,
    } en_MEMORY_LOAD;

    /* Nacteni datoveho bloku do pameti */
    extern void memory_load_block ( uint8_t *data, Z80EX_WORD addr, Z80EX_WORD size, en_MEMORY_LOAD type );


    typedef void ( *cbtype_memory_reconnect_ram )(void);
    typedef Z80EX_BYTE ( *cbtype_memory_mz800_read_byte )( Z80EX_WORD addr );

    extern cbtype_memory_reconnect_ram memory_reconnect_ram;
    extern z80ex_mread_cb memory_read_cb;
    extern z80ex_mwrite_cb memory_write_cb;
    extern cbtype_memory_mz800_read_byte memory_read_byte;

    extern Z80EX_BYTE *g_memory_VRAM;
    extern Z80EX_BYTE *g_memory_ROM;

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    extern Z80EX_BYTE **g_memory_MEMRAM_READ;


    static inline Z80EX_BYTE memory_pure_ram_read_byte ( Z80EX_WORD addr ) {
        //return g_memory.RAM [ addr ];
        return ( g_memory_MEMRAM_READ[( addr >> 12 )] ) [ ( addr & 0x0fff ) ];
    }

    /* Zapis do aktualne mapovane pameti - bez synchronizace */
    typedef void ( *cbtype_memory_write_byte )( Z80EX_WORD addr, Z80EX_BYTE value );
    extern cbtype_memory_write_byte memory_write_byte;


    /* Callback pro cteni bajtu + debugovaci informace o vykonavanych bajtech */
    typedef Z80EX_BYTE ( *cbtype_memory_read_with_history )( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data );
    extern cbtype_memory_read_with_history memory_read_with_history_cb;
#endif

#ifdef MEMORY_MAKE_STATISTICS
    typedef void ( *cbtype_memory_write_memory_statistics ) (void);
    extern cbtype_memory_write_memory_statistics memory_write_memory_statistics;
#endif

#ifdef MACHINE_EMU_MZ800
#include "memory_mz800.h"
#endif

#ifdef MACHINE_EMU_MZ1500
#include "memory_mz1500.h"
#endif



#ifdef __cplusplus
}
#endif

#endif /* MEMORY_H */

