/* 
 * File:   memory.c
 * Author: chaky
 *
 * Created on 15. června 2015, 17:33
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
#include <string.h>

#include "z80ex/include/z80ex.h"

#include "mz800.h"
#include "memext.h"
#include "memory.h"
#include "rom.h"
#include "gdg/gdg.h"
#include "gdg/vramctrl.h"
#include "ctc8253/ctc8253.h"
#include "pio8255/pio8255.h"
#include "typedefs.h"

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
#include "debugger/debugger.h"
#include "ui/ui_main.h"
#endif

#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


#ifdef MACHINE_EMU_MZ800
cbtype_memory_reconnect_ram memory_reconnect_ram = memory_mz800_reconnect_ram;
z80ex_mread_cb memory_read_cb = memory_mz800_read_cb;
z80ex_mwrite_cb memory_write_cb = memory_mz800_write_cb;
cbtype_memory_mz800_read_byte memory_read_byte = memory_mz800_read_byte;
Z80EX_BYTE *g_memory_VRAM = g_memory_mz800.VRAM;
Z80EX_BYTE *g_memory_ROM = g_memory_mz800.ROM;

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
Z80EX_BYTE **g_memory_MEMRAM_READ = g_memory_mz800.memram_read;
cbtype_memory_write_byte memory_write_byte = memory_mz800_write_byte;
cbtype_memory_read_with_history memory_read_with_history_cb = memory_mz800_read_with_history_cb;
#endif

#ifdef MEMORY_MAKE_STATISTICS
cbtype_memory_write_memory_statistics memory_write_memory_statistics = memory_mz800_write_memory_statistics;
#endif

#endif

#ifdef MACHINE_EMU_MZ1500
cbtype_memory_reconnect_ram memory_reconnect_ram = memory_mz1500_reconnect_ram;
z80ex_mread_cb memory_read_cb = memory_mz1500_read_cb;
z80ex_mwrite_cb memory_write_cb = memory_mz1500_write_cb;
cbtype_memory_mz800_read_byte memory_read_byte = memory_mz1500_read_byte;
Z80EX_BYTE *g_memory_VRAM = g_memory_mz1500.VRAM;
Z80EX_BYTE *g_memory_ROM = g_memory_mz1500.ROM;

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
Z80EX_BYTE **g_memory_MEMRAM_READ = g_memory_mz1500.memram_read;
cbtype_memory_write_byte memory_write_byte = memory_mz1500_write_byte;
cbtype_memory_read_with_history memory_read_with_history_cb = memory_mz1500_read_with_history_cb;
#endif

#ifdef MEMORY_MAKE_STATISTICS
cbtype_memory_write_memory_statistics memory_write_memory_statistics = memory_mz1500_write_memory_statistics;
#endif

#endif


/**
 * implicitni mapovani pameti po resetu
 */
void memory_reset ( void ) {
#ifdef MACHINE_EMU_MZ800
    memory_mz800_reset ( );
#endif
#ifdef MACHINE_EMU_MZ1500
    memory_mz1500_reset ( );
#endif
}


void memory_init ( void ) {
#ifdef MACHINE_EMU_MZ800
    memory_mz800_init ( );
#endif
#ifdef MACHINE_EMU_MZ1500
    memory_mz1500_init ( );
#endif
}


/**
 * Nacteni datoveho bloku do pameti.
 * 
 * @param data
 * @param addr
 * @param size
 * @param type - MEMORY_LOAD_MAPPED, MEMORY_LOAD_RAMONLY
 */
void memory_load_block ( uint8_t *data, Z80EX_WORD addr, Z80EX_WORD size, en_MEMORY_LOAD type ) {
#ifdef MACHINE_EMU_MZ800
    memory_mz800_load_block ( data, addr, size, type );
#endif
#ifdef MACHINE_EMU_MZ1500
    printf ( "%s():%d - NOT IMPLEMENTED!\n", __func__, __LINE__ );
#endif
}
