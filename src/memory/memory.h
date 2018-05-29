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
#define	MEMORY_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "z80ex/include/z80ex.h"
#include "gdg/gdg.h"
    /*
     *
     * Velikost jednotlivych pameti
     * 
     */
#define MEMORY_SIZE_ROM_MZ700	0x1000
#define MEMORY_SIZE_ROM_CGROM	0x1000
#define MEMORY_SIZE_ROM_MZ800	0x2000

#define MEMORY_SIZE_ROM		MEMORY_SIZE_ROM_MZ700 + MEMORY_SIZE_ROM_CGROM + MEMORY_SIZE_ROM_MZ800
#define MEMORY_SIZE_RAM		0x10000

#define MEMORY_SIZE_VRAM_BANK	0x2000
#define MEMORY_SIZE_VRAM        MEMORY_SIZE_VRAM_BANK * 2


    /*
     * 
     *  Flagy mapovani pameti
     *
     */
#define MEMORY_MAP_FLAG_ROM_0000	( 1 << 0 )
#define MEMORY_MAP_FLAG_ROM_1000	( 1 << 1 )
#define MEMORY_MAP_FLAG_CGRAM_VRAM	( 1 << 2 )  /* MZ700: 0xc000 - 0xcfff (CGRAM) 
                                                       MZ800: 0x8000 - 0x9fff | 0xbfff (VRAM) */
#define MEMORY_MAP_FLAG_ROM_E000	( 1 << 3 )  /* + MZ700: 0xd000 - 0xdfff (atributova VRAM) */

    /*
     * 
     *  Fyzicke umisteni vsech pameti
     *
     */
    typedef struct st_MEMORY {
        Z80EX_BYTE map;
        Z80EX_BYTE ROM [ MEMORY_SIZE_ROM ];
        Z80EX_BYTE RAM [ MEMORY_SIZE_RAM ];
        Z80EX_BYTE VRAM [ MEMORY_SIZE_VRAM ];
        Z80EX_BYTE EXVRAM [ MEMORY_SIZE_VRAM ];
    } st_MEMORY;


    extern st_MEMORY g_memory;


    extern uint8_t *g_memoryVRAM;
    extern uint8_t *g_memoryVRAM_I;
    extern uint8_t *g_memoryVRAM_II;
    extern uint8_t *g_memoryVRAM_III;
    extern uint8_t *g_memoryVRAM_IV;


    /*
     * 
     *  Testy mapovacich stavu
     *
     */
#define MEMORY_MAP_TEST_ROM_0000	( g_memory.map & MEMORY_MAP_FLAG_ROM_0000 )
#define MEMORY_MAP_TEST_ROM_1000	( g_memory.map & MEMORY_MAP_FLAG_ROM_1000 )
#define MEMORY_MAP_TEST_ROM_E000	( g_memory.map & MEMORY_MAP_FLAG_ROM_E000 )
#define MEMORY_MAP_TEST_VRAM		( g_memory.map & MEMORY_MAP_FLAG_CGRAM_VRAM )
#define MEMORY_MAP_TEST_CGRAM		( DMD_TEST_MZ700 && MEMORY_MAP_TEST_VRAM )
#define MEMORY_MAP_TEST_VRAM_D000	( DMD_TEST_MZ700 && MEMORY_MAP_TEST_ROM_E000 )

#define MEMORY_MAP_TEST_VRAM_8000	( ( ! DMD_TEST_MZ700 ) && MEMORY_MAP_TEST_VRAM )
#define MEMORY_MAP_TEST_VRAM_A000	( MEMORY_MAP_TEST_VRAM_8000 && DMD_TEST_SCRW640 )

    /*
     * 
     *  Definice IO operace pouzite pro mapovani
     *
     */
    typedef enum {
        MEMORY_MAPRQ_IOOP_IN = 0,
        MEMORY_MAP_IOOP_OUT
    } MEMORY_MAP_IOOP;


    
    /*
     * 
     *  Obsluzne funkce pameti
     *
     */

    
    /* Tepla inicializace pameti */
    extern void memory_reset ( void );

    /* Studena inicializace pameti */
    extern void memory_init ( void );

    /* Callback pro cteni bajtu */
    extern Z80EX_BYTE memory_read_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data );

    /* Callback pro zapis bajtu */
    extern void memory_write_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data );

    /* Nastaveni mapy pameti */
    extern void memory_map_set ( MEMORY_MAP_IOOP mmap_method, uint8_t port );




#ifdef	__cplusplus
}
#endif

#endif	/* MEMORY_H */

