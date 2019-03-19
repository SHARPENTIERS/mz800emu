/* 
 * File:   memory_mz1500.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. února 2019, 15:58
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


#ifndef MEMORY_MZ1500_H
#define MEMORY_MZ1500_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ1500

#include "z80ex/include/z80ex.h"
#include "rom.h"
#include "memory.h"

    /*
     *
     * Velikost jednotlivych pameti
     * 
     */
#define MEMORY_MZ1500_SIZE_VRAM         0x1000

#define MEMORY_MZ1500_SIZE_PCG_BANK     0x2000
#define MEMORY_MZ1500_SIZE_PCG          ( MEMORY_MZ1500_SIZE_PCG_BANK * 3 )

#define MEMORY_MZ1500_SIZE_VRAM_TOTAL   ( MEMORY_MZ1500_SIZE_VRAM + MEMORY_MZ1500_SIZE_PCG )

    /*
     * 
     *  Flagy mapovani pameti
     *
     */

#define MEMORY_MZ1500_MAP_FLAG_ROM_MASK ( 3 << 0 )

#define MEMORY_MZ1500_MAP_FLAG_ROM_0000 ( 1 << 0 )
#define MEMORY_MZ1500_MAP_FLAG_ROM_UPPER ( 1 << 1 )  /* 
                                                        Toto rozlozeni plati jen tehdy, pokud
                                                        od 0xd000 - 0xefff neni namap0vana zadna pamet
                                                        (MEMORY_MZ1500_MAP_FLAG_D000 == 0)

                                                        0xd000 - 0xd7ff znakova VRAM (VRAM_I)
                                                        0xd800 - 0xdfff atributova VRAM (VRAM_I)
                                                        0xe000 - 0xe008 porty
                                                        0xe009 - 0xe00f 0xff
                                                        0xe010 - 0xe7ff 0xff (zrejme vyhrazeno pro ROM z periferii)
                                                        0xe800 - 0xffff horni ROM
                                                     */

#define MEMORY_MZ1500_FLAG_SPEC_BITPOS    2

#define MEMORY_MZ1500_MAP_D000_MASK ( 7 << MEMORY_MZ1500_FLAG_SPEC_BITPOS )      /*
                                                        0xd000 - 0xefff spec. pamet
                                                        0xf000 - 0xffff 0xff

                                                        Tento flag ma vzdy vyssi prioritu, nez E000
                                                    */

#define MEMORY_MZ1500_MAP_D000_NONE ( 0 << MEMORY_MZ1500_FLAG_SPEC_BITPOS ) /* 0xd000 - 0xffff VRAM + PORTS + ROM */
#define MEMORY_MZ1500_MAP_D000_CGROM ( 1 << MEMORY_MZ1500_FLAG_SPEC_BITPOS ) /* 0xd000 - 0xdfff a 0xe000 - 0xefff duplicitne namapovano CGROM */
#define MEMORY_MZ1500_MAP_D000_PCG1 ( 2 << MEMORY_MZ1500_FLAG_SPEC_BITPOS ) /* 0xd000 - 0xefff PCG_PLANE1 */
#define MEMORY_MZ1500_MAP_D000_PCG2 ( 3 << MEMORY_MZ1500_FLAG_SPEC_BITPOS ) /* 0xd000 - 0xefff PCG_PLANE2 */
#define MEMORY_MZ1500_MAP_D000_PCG3 ( 4 << MEMORY_MZ1500_FLAG_SPEC_BITPOS ) /* 0xd000 - 0xefff PCG_PLANE3 */


    /* Memory map porty pro IORQ - PWRITE */
    typedef enum en_MMAP_MZ1500_PWRITE {
        MMAP_MZ1500_PWRITE_E0 = 0xe0, /* memory unmap ROM 0000 */
        MMAP_MZ1500_PWRITE_E1 = 0xe1, /* memory unmap ROM E000, VRAM D000 */
        MMAP_MZ1500_PWRITE_E2 = 0xe2, /* memory map ROM 0000 */
        MMAP_MZ1500_PWRITE_E3 = 0xe3, /* memory map ROM E000, VRAM D000 */
        MMAP_MZ1500_PWRITE_E4 = 0xe4, /* memory map ROM 0000, ROM E000, VRAM D000 */
        MMAP_MZ1500_PWRITE_E5 = 0xe5, /* map SPEC D000 - EFFF, a F000 - FFFF = 0xff */
        MMAP_MZ1500_PWRITE_E6 = 0xe6, /* unmap SPEC */
    } en_MMAP_MZ1500_PWRITE;


    /*
     * 
     *  Fyzicke umisteni vsech pameti
     *
     */
    typedef struct st_MEMORY_MZ1500 {
        Z80EX_BYTE map;
        Z80EX_BYTE *memram_read [ MEMORY_MEMRAM_POINTS ];
        Z80EX_BYTE *memram_write [ MEMORY_MEMRAM_POINTS ];
        Z80EX_BYTE ROM [ ROM_SIZE_TOTAL ];
        Z80EX_BYTE RAM [ MEMORY_SIZE_RAM ];
        Z80EX_BYTE VRAM [ MEMORY_MZ1500_SIZE_VRAM_TOTAL ];
    } st_MEMORY_MZ1500;


    extern st_MEMORY_MZ1500 g_memory_mz1500;

    extern Z80EX_BYTE *g_memory_mz1500_VRAM;
    extern Z80EX_BYTE *g_memory_mz1500_PCG1;
    extern Z80EX_BYTE *g_memory_mz1500_PCG2;
    extern Z80EX_BYTE *g_memory_mz1500_PCG3;

    /* z map urcime SPEC id: 1, 2, 3, 4 ( 0 => neni pripojeno spec, tzn, ze na D000 je RAM, nebo VRAM ) */
#define MEMORY_MZ1500_SPEC_ID    ( ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) >> MEMORY_MZ1500_FLAG_SPEC_BITPOS )

    /* z map urcime PCG id: 0, 1, 2 */
#define MEMORY_MZ1500_PCG_ID    ( MEMORY_MZ1500_SPEC_ID - 2 )

    /*
     * 
     *  Testy mapovacich stavu
     *
     */
#define MEMORY_MZ1500_MAP_TEST_ROM_0000 ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_FLAG_ROM_0000 )
#define MEMORY_MZ1500_MAP_TEST_RAM_0000 ( !MEMORY_MZ1500_MAP_TEST_ROM_0000 )

#define MEMORY_MZ1500_MAP_TEST_ROM_UPPER ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_FLAG_ROM_UPPER )
#define MEMORY_MZ1500_MAP_TEST_RAM_UPPER ( !MEMORY_MZ1500_MAP_TEST_ROM_UPPER )

#define MEMORY_MZ1500_MAP_TEST_D000_VRAM ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER & ( ! ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) ) )
#define MEMORY_MZ1500_MAP_TEST_E000_PORTS ( MEMORY_MZ1500_MAP_TEST_D000_VRAM )
#define MEMORY_MZ1500_MAP_TEST_E800_ROM ( MEMORY_MZ1500_MAP_TEST_D000_VRAM )

#define MEMORY_MZ1500_MAP_TEST_D000_SPEC ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER & ( ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) ) )
#define MEMORY_MZ1500_MAP_TEST_D000_CGROM ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER & ( MEMORY_MZ1500_MAP_D000_CGROM == ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) ) )
#define MEMORY_MZ1500_MAP_TEST_D000_PCG1 ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER & ( MEMORY_MZ1500_MAP_D000_PCG1 == ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) ) )
#define MEMORY_MZ1500_MAP_TEST_D000_PCG2 ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER & ( MEMORY_MZ1500_MAP_D000_PCG2 == ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) ) )
#define MEMORY_MZ1500_MAP_TEST_D000_PCG3 ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER & ( MEMORY_MZ1500_MAP_D000_PCG3 == ( g_memory_mz1500.map & MEMORY_MZ1500_MAP_D000_MASK ) ) )


    /*
     * 
     *  Obsluzne funkce pameti
     *
     */


    /* Tepla inicializace pameti */
    extern void memory_mz1500_reset ( void );

    /* Studena inicializace pameti */
    extern void memory_mz1500_init ( void );

    /* Zmena pripojeni bank memextu */
    extern void memory_mz1500_reconnect_ram ( void );

    /* Callback pro cteni bajtu */
    extern Z80EX_BYTE memory_mz1500_read_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    /* Callback pro cteni bajtu + debugovaci informace o vykonavanych bajtech */
    extern Z80EX_BYTE memory_mz1500_read_with_history_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data );
#endif

    /* Callback pro zapis bajtu */
    extern void memory_mz1500_write_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data );

    /* Nastaveni mapy pameti */
    extern void memory_mz1500_map_pwrite ( en_MMAP_MZ1500_PWRITE mmap_port, Z80EX_BYTE value );

    /* Cteni z aktualne mapovane pameti - bez synchronizace */
    extern Z80EX_BYTE memory_mz1500_read_byte ( Z80EX_WORD addr );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    /* Zapis do aktualne mapovane pameti - bez synchronizace */
    extern void memory_mz1500_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );
#endif

    /* Nacteni datoveho bloku do pameti */
    extern void memory_mz1500_load_block ( uint8_t *data, Z80EX_WORD addr, Z80EX_WORD size, en_MEMORY_LOAD type );


#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED


    static inline int memory_mz1500_test_addr_is_vram ( Z80EX_WORD addr ) {
        if ( !MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) return 0;
        int a = addr >> 12;
        if ( a == 0x0d ) return 1;
        if ( ( a == 0x0e ) && ( MEMORY_MZ1500_SPEC_ID > 1 ) ) return 1;
        return 0;
    }

#endif 

#ifdef MEMORY_MAKE_STATISTICS
    void memory_mz1500_write_memory_statistics ( void );
#endif

#endif


#ifdef __cplusplus
}
#endif

#endif /* MEMORY_MZ1500_H */

