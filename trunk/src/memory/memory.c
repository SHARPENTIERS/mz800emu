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

//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


st_MEMORY g_memory;


Z80EX_BYTE *g_memoryVRAM = g_memory.VRAM;
Z80EX_BYTE *g_memoryVRAM_I = g_memory.VRAM;
Z80EX_BYTE *g_memoryVRAM_II = &g_memory.VRAM [ MEMORY_SIZE_VRAM_BANK ];
Z80EX_BYTE *g_memoryVRAM_III = g_memory.EXVRAM;
Z80EX_BYTE *g_memoryVRAM_IV = &g_memory.EXVRAM [ MEMORY_SIZE_VRAM_BANK ];



#define MEMORY_READ_ROM_BYTE            g_memory.ROM [ addr & 0x3fff ]

#define MEMORY_READ_RAM_BYTE            g_memory.RAM [ addr ]
#define MEMORY_WRITE_RAM_BYTE           g_memory.RAM [ addr ] = value;

#define VRAMCTRL_MZ700_READ_BYTE	vramctrl_mz700_memop_read ( addr & ~0xe000, m1_state )
#define VRAMCTRL_MZ700_WRITE_BYTE	vramctrl_mz700_memop_write ( addr & ~0xe000, value )

#define VRAMCTRL_MZ800_READ_BYTE	vramctrl_mz800_memop_read ( addr & 0x3fff )
#define VRAMCTRL_MZ800_WRITE_BYTE	vramctrl_mz800_memop_write ( addr & 0x3fff, value )

#define CTC8253_READ_BYTE               ctc8253_read_byte ( addr & 0x03 )
#define CTC8253_WRITE_BYTE              ctc8253_write_byte ( addr & 0x03, value )

#define PIO8255_READ_BYTE               pio8255_read ( addr & 0x03 )
#define PIO8255_WRITE_BYTE              pio8255_write ( addr & 0x03, value )

#define GDG_DMD_READ_BYTE               gdg_read_dmd_status ( )
#define GDG_STS_WRITE_BYTE              gdg_write_byte ( addr, value )


/*
 * Studena inicializace pameti:
 *
 *  - vytvorime inicializacni obsah RAM, VRAM, EXVRAM
 *
 */
void memory_init ( void ) {

    uint32_t i;
    uint16_t *addr;

    for ( i = 0; i < 0xffff; i += 4 ) {
        addr = ( uint16_t* ) & g_memory.RAM [ i ];
        *addr++ = 0xffff;
        *addr = 0x0000;
    };

    for ( i = 0; i < MEMORY_SIZE_VRAM; i += 2 ) {
        addr = ( uint16_t* ) & g_memory.VRAM [ i ];
        *addr = 0x00ff;
    };

    for ( i = 0; i < MEMORY_SIZE_VRAM; i += 4 ) {
        addr = ( uint16_t* ) & g_memory.EXVRAM [ i ];
        if ( i & 0x80 ) {
            *addr++ = 0x0000;
            *addr = 0xffff;
        } else {
            *addr++ = 0xffff;
            *addr = 0x0000;
        };
    };

    rom_init ( );

#if 0
    //    g_memory.RAM [ 0x2000 ] = 0x3e;
    //    g_memory.RAM [ 0x2001 ] = 0x01;
    g_memory.RAM [ 0x2000 ] = 0x3c;
    g_memory.RAM [ 0x2001 ] = 0x00;
    g_memory.RAM [ 0x2002 ] = 0x01;
    g_memory.RAM [ 0x2003 ] = 0xcf;
    g_memory.RAM [ 0x2004 ] = 0x06;
    g_memory.RAM [ 0x2005 ] = 0xed;
    g_memory.RAM [ 0x2006 ] = 0x79;
    //    g_memory.RAM [ 0x2007 ] = 0xc9;
    g_memory.RAM [ 0x2007 ] = 0xc3;
    g_memory.RAM [ 0x2008 ] = 0x00;
    g_memory.RAM [ 0x2009 ] = 0x20;
#endif
}


/*
 * Reset - implicitni mapovani pameti po resetu
 */
void memory_reset ( void ) {
    g_memory.map = MEMORY_MAP_FLAG_ROM_0000 | MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_ROM_E000;
    //    g_memory.map = 0;
}


/*
 * Nastaveni mapovani pameti podle IORQ operace IN n, (port) / OUT (port), n
 *
 */
void memory_map_set ( MEMORY_MAP_IOOP mmap_method, Z80EX_BYTE port ) {

    if ( mmap_method == MEMORY_MAP_IOOP_OUT ) {

        switch ( port ) {

                /* memory unmap ROM 0000 , CGROM */
            case 0xe0:
                g_memory.map &= ~( MEMORY_MAP_FLAG_ROM_0000 | MEMORY_MAP_FLAG_ROM_1000 );
                break;

                /* memory unmap ROM E000 ( coz v MZ700 znamena i VRAM na D000 ) */
            case 0xe1:
                g_memory.map &= ~( MEMORY_MAP_FLAG_ROM_E000 );
                break;

                /* memory map ROM 0000 */
            case 0xe2:
                g_memory.map |= MEMORY_MAP_FLAG_ROM_0000;
                break;

                /* memory map ROM E000 ( coz v MZ700 znamena i VRAM na D000 ) */
            case 0xe3:
                g_memory.map |= MEMORY_MAP_FLAG_ROM_E000;
                break;

                /* memory map ROM 0000, ROM E000 */
                /* MZ700: unmap CGROM, CGRAM */
                /* MZ800: map CGROM, VRAM */
            case 0xe4:
                g_memory.map |= ( MEMORY_MAP_FLAG_ROM_0000 | MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_CGRAM_VRAM | MEMORY_MAP_FLAG_ROM_E000 );
                if ( DMD_TEST_MZ700 ) {
                    g_memory.map &= ~( MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_CGRAM_VRAM );
                };
                break;

            case 0xe5: /* map EXROM */
            case 0xe6: /* unmap EXROM */
                DBGPRINTF ( DBGWAR, "EXROM mapping is ignorred!\n" );
                break;
        };

    } else { /* MMAP_METHOD_IN */

        if ( 0xe0 == port ) {
            g_memory.map |= MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_CGRAM_VRAM;
        } else if ( 0xe1 == port ) {
            g_memory.map &= ~( MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_CGRAM_VRAM );
        };

    };

    /*  DEBUGGER_MMAP_FULL_UPDATE ( ); */
}


Z80EX_BYTE memory_read_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data ) {

    int addr_high;
    int addr_low;
    Z80EX_BYTE retval;

    addr_high = addr >> 12;

    switch ( addr_high ) {

        case 0x00:
            if ( MEMORY_MAP_TEST_ROM_0000 ) {
                retval = MEMORY_READ_ROM_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x01:
            if ( MEMORY_MAP_TEST_ROM_1000 ) {
                retval = MEMORY_READ_ROM_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x08:
        case 0x09:
            if ( MEMORY_MAP_TEST_VRAM_8000 ) {
                retval = VRAMCTRL_MZ800_READ_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x0a:
        case 0x0b:
            if ( MEMORY_MAP_TEST_VRAM_A000 ) {
                retval = VRAMCTRL_MZ800_READ_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x0c:
            if ( MEMORY_MAP_TEST_CGRAM ) {
                mz800_sync_inside_cpu ( INSIDEOP_IORQ_RD | ( m1_state << 2 ) );
                retval = VRAMCTRL_MZ700_READ_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x0d:
            if ( MEMORY_MAP_TEST_VRAM_D000 ) {
                mz800_sync_inside_cpu ( INSIDEOP_MREQ_RD | ( m1_state << 2 ) );
                retval = VRAMCTRL_MZ700_READ_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x0e:
            if ( MEMORY_MAP_TEST_ROM_E000 ) {

                addr_low = addr & 0x0fff;

                if ( addr_low > 0x0f ) {

                    retval = MEMORY_READ_ROM_BYTE;

                } else {

                    /* cteni z E008 ( regDMD ) */
                    if ( 0x08 == ( addr & 0x000f ) ) {
                        mz800_sync_inside_cpu ( INSIDEOP_MREQ_RD | ( m1_state << 2 ) );
                        retval = GDG_DMD_READ_BYTE;

                        /* cteni z 8255, nebo 8253 */
                    } else if ( 0x00 == ( addr & 0x0008 ) ) {
                        if ( addr & 0x04 ) {
                            if ( addr == 0xe007 ) {
                                /* Kontrol registr cist nelze */
                                retval = g_mz800.regDBUS_latch;
                            } else {
                                mz800_sync_inside_cpu ( INSIDEOP_MREQ_RD | ( m1_state << 2 ) );
                                retval = CTC8253_READ_BYTE;
                            };
                        } else {
                            mz800_sync_inside_cpu ( INSIDEOP_MREQ_RD | ( m1_state << 2 ) );
                            retval = PIO8255_READ_BYTE;
                        };

                        /* cteni z E009 - E00F */
                    } else {
                        retval = g_mz800.regDBUS_latch;
                    };
                };
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        case 0x0f:
            if ( MEMORY_MAP_TEST_ROM_E000 ) {
                retval = MEMORY_READ_ROM_BYTE;
            } else {
                retval = MEMORY_READ_RAM_BYTE;
            };
            break;

        default:
            retval = MEMORY_READ_RAM_BYTE;
            break;
    };

    g_mz800.regDBUS_latch = retval;
    return retval;
}


void memory_write_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data ) {
    int addr_high;
    int addr_low;

    addr_high = addr >> 12;

    switch ( addr_high ) {

        case 0x00:
            if ( MEMORY_MAP_TEST_ROM_0000 ) {
                return;
            };
            break;

        case 0x01:
            if ( MEMORY_MAP_TEST_ROM_1000 ) {
                return;
            };
            break;

        case 0x08:
        case 0x09:
            if ( MEMORY_MAP_TEST_VRAM_8000 ) {
                VRAMCTRL_MZ800_WRITE_BYTE;
                return;
            };
            break;

        case 0x0a:
        case 0x0b:
            if ( MEMORY_MAP_TEST_VRAM_A000 ) {
                VRAMCTRL_MZ800_WRITE_BYTE;
                return;
            };
            break;

        case 0x0c:
            if ( MEMORY_MAP_TEST_CGRAM ) {
                mz800_sync_inside_cpu ( INSIDEOP_MREQ_WR );
                VRAMCTRL_MZ700_WRITE_BYTE;
                return;
            };
            break;

        case 0x0d:
            if ( MEMORY_MAP_TEST_VRAM_D000 ) {
                mz800_sync_inside_cpu ( INSIDEOP_MREQ_WR );
                VRAMCTRL_MZ700_WRITE_BYTE;
                return;
            };
            break;

        case 0x0e:
            if ( MEMORY_MAP_TEST_ROM_E000 ) {

                addr_low = addr & 0x0fff;

                if ( addr_low > 0x07 ) {
                    if ( addr_low == 0x08 ) {

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
                        if ( TEST_DEBUGGER_MEMOP_CALL ) {
                            ui_show_error ( "MZ-800 Debugger can't write into memory mapped port 0x%04x", addr );
                            return;
                        };
#endif

                        /* Zapis do MZ-700 status registru */
                        mz800_sync_inside_cpu ( INSIDEOP_MREQ_WR );
                        GDG_STS_WRITE_BYTE;
                    };
                    return;
                };

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
                if ( TEST_DEBUGGER_MEMOP_CALL ) {
                    ui_show_error ( "MZ-800 Debugger can't write into memory mapped port 0x%04x", addr );
                    return;
                };
#endif

                if ( addr & 0x04 ) {
                    mz800_sync_inside_cpu ( INSIDEOP_MREQ_WR );
                    CTC8253_WRITE_BYTE;
                } else {
                    mz800_sync_inside_cpu ( INSIDEOP_IORQ_WR );
                    PIO8255_WRITE_BYTE;
                };

                return;
            };
            break;

        case 0x0f:
            if ( MEMORY_MAP_TEST_ROM_E000 ) {
                return;
            };
            break;

        default:
            break;
    };

    MEMORY_WRITE_RAM_BYTE;
}
