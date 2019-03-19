/* 
 * File:   memory_mz800.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. února 2019, 18:12
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


#ifndef MEMORY_MZ800_H
#define MEMORY_MZ800_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ800

#include "z80ex/include/z80ex.h"
#include "rom.h"
#include "memory.h"
    // delame zde testy podle DMD
#include "gdg/gdg.h"

    /*
     *
     * Velikost jednotlivych pameti
     * 
     */
#define MEMORY_MZ800_SIZE_VRAM_BANK   0x2000
#define MEMORY_MZ800_SIZE_VRAM        ( MEMORY_MZ800_SIZE_VRAM_BANK * 2 )

    /*
     * 
     *  Flagy mapovani pameti
     *
     */

#define MEMORY_MZ800_MAP_FLAG_ROM_0000 ( 1 << 0 )
#define MEMORY_MZ800_MAP_FLAG_ROM_1000 ( 1 << 1 )
#define MEMORY_MZ800_MAP_FLAG_CGRAM_VRAM ( 1 << 2 )  /* MZ700: 0xc000 - 0xcfff (CGRAM) 
                                                       MZ800: 0x8000 - 0x9fff | 0xbfff (VRAM) */
#define MEMORY_MZ800_MAP_FLAG_ROM_E000 ( 1 << 3 )  /* + MZ700: 0xd000 - 0xdfff (atributova VRAM) */


    /* Memory map porty pro IORQ - PWRITE */
    typedef enum en_MMAP_MZ800_PWRITE {
        MMAP_MZ800_PWRITE_E0 = 0xe0, /* memory unmap ROM 0000 , CGROM */
        MMAP_MZ800_PWRITE_E1 = 0xe1, /* memory unmap ROM E000, coz v MZ700 znamena i VRAM na D000 */
        MMAP_MZ800_PWRITE_E2 = 0xe2, /* memory map ROM 0000 */
        MMAP_MZ800_PWRITE_E3 = 0xe3, /* memory map ROM E000, coz v MZ700 znamena i VRAM na D000 */
        MMAP_MZ800_PWRITE_E4 = 0xe4, /* memory map ROM 0000, ROM E000, MZ700: unmap CGROM, CGRAM, MZ800: map CGROM, VRAM */
        /* pozustatky z MZ-700 - v MZ-800 ponekud nefunkcni */
        MMAP_MZ800_PWRITE_E5 = 0xe5, /* map EXROM */
        MMAP_MZ800_PWRITE_E6 = 0xe6, /* unmap EXROM */
    } en_MMAP_MZ800_PWRITE;


    /* Memory map porty pro IORQ - PREAD */
    typedef enum en_MMAP_MZ800_PREAD {
        MMAP_MZ800_PREAD_E0 = 0xe0, /* memory map CG-ROM, CG-RAM, VRAM - podle mode */
        MMAP_MZ800_PREAD_E1 = 0xe1, /* memory unmap CG-ROM, CG-RAM, VRAM - podle mode */
    } en_MMAP_MZ800_PREAD;


    /*
     * 
     *  Fyzicke umisteni vsech pameti
     *
     */
    typedef struct st_MEMORY_MZ800 {
        Z80EX_BYTE map;
        Z80EX_BYTE *memram_read [ MEMORY_MEMRAM_POINTS ];
        Z80EX_BYTE *memram_write [ MEMORY_MEMRAM_POINTS ];
        Z80EX_BYTE ROM [ ROM_SIZE_TOTAL ];
        Z80EX_BYTE RAM [ MEMORY_SIZE_RAM ];
        Z80EX_BYTE VRAM [ MEMORY_MZ800_SIZE_VRAM ];
        Z80EX_BYTE EXVRAM [ MEMORY_MZ800_SIZE_VRAM ];
    } st_MEMORY_MZ800;


    extern st_MEMORY_MZ800 g_memory_mz800;

    extern uint8_t *g_memory_mz800_VRAM;
    extern uint8_t *g_memory_mz800_VRAM_I;
    extern uint8_t *g_memory_mz800_VRAM_II;
    extern uint8_t *g_memory_mz800_VRAM_III;
    extern uint8_t *g_memory_mz800_VRAM_IV;


    /*
     * 
     *  Testy mapovacich stavu
     *
     */
#define MEMORY_MZ800_MAP_TEST_ROM_0000 ( g_memory_mz800.map & MEMORY_MZ800_MAP_FLAG_ROM_0000 )
#define MEMORY_MZ800_MAP_TEST_ROM_1000 ( g_memory_mz800.map & MEMORY_MZ800_MAP_FLAG_ROM_1000 )
#define MEMORY_MZ800_MAP_TEST_ROM_E000 ( g_memory_mz800.map & MEMORY_MZ800_MAP_FLAG_ROM_E000 )
#define MEMORY_MZ800_MAP_TEST_VRAM  ( g_memory_mz800.map & MEMORY_MZ800_MAP_FLAG_CGRAM_VRAM )
#define MEMORY_MZ800_MAP_TEST_CGRAM  ( GDG_MZ800_TEST_DMD_MODE700 && MEMORY_MZ800_MAP_TEST_VRAM )
#define MEMORY_MZ800_MAP_TEST_VRAM_D000 ( GDG_MZ800_TEST_DMD_MODE700 && MEMORY_MZ800_MAP_TEST_ROM_E000 )

#define MEMORY_MZ800_MAP_TEST_VRAM_8000 ( ( ! GDG_MZ800_TEST_DMD_MODE700 ) && MEMORY_MZ800_MAP_TEST_VRAM )
#define MEMORY_MZ800_MAP_TEST_VRAM_A000 ( MEMORY_MZ800_MAP_TEST_VRAM_8000 && GDG_MZ800_TEST_DMD_SCRW640 )


    /*
     * 
     *  Obsluzne funkce pameti
     *
     */


    /* Tepla inicializace pameti */
    extern void memory_mz800_reset ( void );

    /* Studena inicializace pameti */
    extern void memory_mz800_init ( void );

    /* Zmena pripojeni bank memextu */
    extern void memory_mz800_reconnect_ram ( void );

    /* Callback pro cteni bajtu */
    extern Z80EX_BYTE memory_mz800_read_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    /* Callback pro cteni bajtu + debugovaci informace o vykonavanych bajtech */
    extern Z80EX_BYTE memory_mz800_read_with_history_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data );
#endif

    /* Callback pro zapis bajtu */
    extern void memory_mz800_write_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data );

    /* Nastaveni mapy pameti */
    extern void memory_mz800_map_pwrite ( en_MMAP_MZ800_PWRITE mmap_port );
    extern void memory_mz800_map_pread ( en_MMAP_MZ800_PREAD mmap_port );

    /* Cteni z aktualne mapovane pameti - bez synchronizace */
    extern Z80EX_BYTE memory_mz800_read_byte ( Z80EX_WORD addr );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    /* Zapis do aktualne mapovane pameti - bez synchronizace */
    extern void memory_mz800_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );
#endif 

    /* Nacteni datoveho bloku do pameti */
    extern void memory_mz800_load_block ( uint8_t *data, Z80EX_WORD addr, Z80EX_WORD size, en_MEMORY_LOAD type );


#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED


    static inline int memory_mz800_test_addr_is_vram ( Z80EX_WORD addr ) {
        int a = addr >> 12;
        if (
             ( ( MEMORY_MZ800_MAP_TEST_CGRAM ) && ( a == 0x0c ) ) ||
             ( ( MEMORY_MZ800_MAP_TEST_VRAM_D000 ) && ( a == 0x0d ) ) ||
             ( ( MEMORY_MZ800_MAP_TEST_VRAM_8000 ) && ( a == 0x08 ) ) ||
             ( ( MEMORY_MZ800_MAP_TEST_VRAM_A000 ) && ( a == 0x0a ) )
             ) {
            return 1;
        };
        return 0;
    }
    
#endif 

#ifdef MEMORY_MAKE_STATISTICS
    extern void memory_mz800_write_memory_statistics ( void );
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* MEMORY_MZ800_H */

