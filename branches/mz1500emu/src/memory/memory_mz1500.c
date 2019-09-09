/* 
 * File:   memory_mz1500.c
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

#include "mz800emu_cfg.h"

#include <stdio.h>
#include <string.h>

#ifdef MACHINE_EMU_MZ1500

#include "z80ex/include/z80ex.h"

#include "mz800.h"
#include "memext.h"
#include "memory_mz1500.h"
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




st_MEMORY_MZ1500 g_memory_mz1500;

#ifdef MEMORY_MAKE_STATISTICS
st_MEMORY_STATS g_memory_mz1500_statistics;
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#define MEMORY_MZ1500_STATISTIC_DAT_FILE   "memstats_mz1500.dat"
#define MEMORY_MZ1500_STATISTIC_TXT_FILE   "memstats_mz1500.txt"
#endif

Z80EX_BYTE *g_memory_mz1500_VRAM = g_memory_mz1500.VRAM;
Z80EX_BYTE *g_memory_mz1500_PCG1 = &g_memory_mz1500.VRAM[MEMORY_MZ1500_SIZE_VRAM];
Z80EX_BYTE *g_memory_mz1500_PCG2 = &g_memory_mz1500.VRAM[MEMORY_MZ1500_SIZE_VRAM + MEMORY_MZ1500_SIZE_PCG_BANK];
Z80EX_BYTE *g_memory_mz1500_PCG3 = &g_memory_mz1500.VRAM[MEMORY_MZ1500_SIZE_VRAM + ( MEMORY_MZ1500_SIZE_PCG_BANK * 2 )];

#define MEMORY_MZ1500_ROM_READ_BYTE                g_memory_mz1500.ROM [ addr & 0x3fff ]

#define MEMORY_MZ1500_CGROM_READ_BYTE              g_memory_mz1500.ROM [ addr & 0x0fff ]

//#define MEMORY_RAM_READ_BYTE                g_memory.RAM [ addr ]
//#define MEMORY_RAM_WRITE_BYTE               g_memory.RAM [ addr ] = value;
#define MEMORY_MZ1500_RAM_READ_BYTE             ( g_memory_mz1500.memram_read[( addr >> 12 )] ) [ (addr & 0x0fff) ]
#define MEMORY_MZ1500_RAM_WRITE_BYTE            ( g_memory_mz1500.memram_write[( addr >> 12 )] ) [ (addr & 0x0fff) ] = value;

#define MEMORY_MZ1500_VRAM_READ_BYTE_SYNC       vramctrl_mz1500_read_byte_sync ( addr & 0x0fff )
#define MEMORY_MZ1500_VRAM_WRITE_BYTE_SYNC      vramctrl_mz1500_write_byte_sync ( addr & 0x0fff, value )

#define MEMORY_MZ1500_VRAM_READ_BYTE            vramctrl_mz1500_read_byte ( addr & 0x0fff )
#define MEMORY_MZ1500_VRAM_WRITE_BYTE           vramctrl_mz1500_write_byte ( addr & 0x0fff, value )

#define MEMORY_MZ1500_PCG_READ_BYTE_SYNC        vramctrl_mz1500_read_byte_sync ( ( pcg_id * MEMORY_MZ1500_SIZE_PCG_BANK ) + ( ( addr & 0x3fff ) ) )
#define MEMORY_MZ1500_PCG_WRITE_BYTE_SYNC       vramctrl_mz1500_write_byte_sync ( ( ( pcg_id * MEMORY_MZ1500_SIZE_PCG_BANK ) + ( ( addr & 0x3fff ) ) ), value )

#define MEMORY_MZ1500_PCG_READ_BYTE             vramctrl_mz1500_read_byte ( ( pcg_id * MEMORY_MZ1500_SIZE_PCG_BANK ) + ( ( addr & 0x3fff ) ) )
#define MEMORY_MZ1500_PCG_WRITE_BYTE            vramctrl_mz1500_write_byte ( ( ( pcg_id * MEMORY_MZ1500_SIZE_PCG_BANK ) + ( ( addr & 0x3fff ) ) ), value )

/*******************************************************************************
 *
 * Mapovani MZ-1500 pameti ROM_0000, ROM_E000
 * 
 ******************************************************************************/


/**
 * OUT 0xE0 - memory unmap ROM 0000
 */
static inline void memory_mz1500_mmap_rom_bottom_off ( void ) {
    DBGPRINTF ( DBGINF, "pwrite = 0xe0 (0000 - 0FFF: RAM), PC = 0x%04x\n", g_mz800.instruction_addr );
    g_memory_mz1500.map &= ~( MEMORY_MZ1500_MAP_FLAG_ROM_0000 );
}


/**
 * OUT 0xE1 - memory unmap ROM E800, VRAM na D000, PORTS E000
 */
static inline void memory_mz1500_mmap_rom_upper_off ( void ) {
    DBGPRINTF ( DBGINF, "pwrite = 0xe1 (D000 - FFFF: RAM), PC = 0x%04x\n", g_mz800.instruction_addr );
    g_memory_mz1500.map &= ~( MEMORY_MZ1500_MAP_FLAG_ROM_UPPER );
}


/**
 * OUT 0xE2 - memory map ROM 0000
 */
static inline void memory_mz1500_mmap_rom_0000_on ( void ) {
    DBGPRINTF ( DBGINF, "pwrite = 0xe2 (0000 - 0FFF: ROM), PC = 0x%04x\n", g_mz800.instruction_addr );
    g_memory_mz1500.map |= MEMORY_MZ1500_MAP_FLAG_ROM_0000;
}


/**
 * OUT 0xE3 - memory map ROM E800, VRAM na D000, PORTS E000
 */
static inline void memory_mz1500_mmap_rom_upper_on ( void ) {
    DBGPRINTF ( DBGINF, "pwrite = 0xe3 (D000 - FFFF: ROM), PC = 0x%04x\n", g_mz800.instruction_addr );
    g_memory_mz1500.map |= MEMORY_MZ1500_MAP_FLAG_ROM_UPPER;
    g_memory_mz1500.map &= ~( MEMORY_MZ1500_MAP_D000_MASK );
}


/**
 * OUT 0xE4 - memory map ROM 0000, ROM E000
 * MZ700: unmap CGROM, CGRAM
 * MZ800: map CGROM, VRAM
 */
static inline void memory_mz1500_mmap_all_on ( void ) {
    DBGPRINTF ( DBGINF, "pwrite = 0xe4 (D000 - FFFF: ROM, D000 - FFFF: ROM), PC = 0x%04x\n", g_mz800.instruction_addr );
    g_memory_mz1500.map = ( MEMORY_MZ1500_MAP_FLAG_ROM_0000 | MEMORY_MZ1500_MAP_FLAG_ROM_UPPER );
}


/**
 * OUT 0xE5 - memory map SPEC D000
 */
static inline void memory_mz1500_mmap_spec_on ( Z80EX_BYTE value ) {
    value &= 0x03;
    DBGPRINTF ( DBGINF, "pwrite = 0xe5, mount SPEC - value: 0x%02x, PC = 0x%04x\n", value, g_mz800.instruction_addr );
    g_memory_mz1500.map &= ~( MEMORY_MZ1500_MAP_D000_MASK );
    g_memory_mz1500.map |= ( value + 1 ) << MEMORY_MZ1500_FLAG_SPEC_BITPOS;
    g_memory_mz1500.map |= MEMORY_MZ1500_MAP_FLAG_ROM_UPPER;
}


/**
 * OUT 0xE6 - memory umap SPEC D000
 */
static inline void memory_mz1500_mmap_spec_off ( void ) {
    DBGPRINTF ( DBGINF, "pwrite = 0xe6, umount SPEC, PC = 0x%04x\n", g_mz800.instruction_addr );
    g_memory_mz1500.map &= ~( MEMORY_MZ1500_MAP_D000_MASK );
}


/**
 * Mapovani pameti pres IORQ - pwrite.
 * 
 * @param mmap_port
 */
void memory_mz1500_map_pwrite ( en_MMAP_MZ1500_PWRITE mmap_port, Z80EX_BYTE value ) {

    //printf ( "memory map: 0x%02x, 0x%02x\n", mmap_port, value );

    switch ( mmap_port ) {
        case MMAP_MZ1500_PWRITE_E0:
            memory_mz1500_mmap_rom_bottom_off ( );
            break;

        case MMAP_MZ1500_PWRITE_E1:
            memory_mz1500_mmap_rom_upper_off ( );
            break;

        case MMAP_MZ1500_PWRITE_E2:
            memory_mz1500_mmap_rom_0000_on ( );
            break;

        case MMAP_MZ1500_PWRITE_E3:
            memory_mz1500_mmap_rom_upper_on ( );
            break;

        case MMAP_MZ1500_PWRITE_E4:
            memory_mz1500_mmap_all_on ( );
            break;

        case MMAP_MZ1500_PWRITE_E5:
            memory_mz1500_mmap_spec_on ( value );
            break;

        case MMAP_MZ1500_PWRITE_E6:
            memory_mz1500_mmap_spec_off ( );
            break;
    };
}


/*******************************************************************************
 *
 * Inicializace pameti RAM, MEMEXT, VRAM a EXVRAM
 * 
 ******************************************************************************/

void memory_mz1500_reconnect_ram ( void ) {
    if ( TEST_MEMEXT_CONNECTED ) {
        int i;
        for ( i = 0; i < MEMORY_MEMRAM_POINTS; i++ ) {
            g_memory_mz1500.memram_read[i] = memext_get_ram_read_pointer_by_addr_point ( i );
            g_memory_mz1500.memram_write[i] = memext_get_ram_write_pointer_by_addr_point ( i );
        };
    } else {
        int i;
        for ( i = 0; i < MEMORY_MEMRAM_POINTS; i++ ) {
            g_memory_mz1500.memram_read[i] = &g_memory_mz1500.RAM[( i << 12 )];
            g_memory_mz1500.memram_write[i] = &g_memory_mz1500.RAM[( i << 12 )];
        };
    };

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    if ( TEST_EMULATION_PAUSED ) {
        ui_main_debugger_windows_refresh ( );
    };
#endif
}


/**
 * 
 * Inicializace obsahu DRAM na mem MZ-1500:
 * 
 * RAM:
 * 128x 0xff 0xff 0x00 0x00
 * 128x 0x00 0x00 0xff 0xff
 * 
 * Pravdepodobne ma vsak inicializacni obsah vypadat takhle:
 * 
 * 0x0000 FF 00 FF 00 FF 00 FF 00
 * 0x0008 FF 00 FF 00 FF 00 FF 00
 * ...
 * 0x0080 00 FF 00 FF 00 FF 00 FF
 * 0x0088 00 FF 00 FF 00 FF 00 FF
 * ...
 * 
 * TODO: inicializaci VRAM zatim nezname...
 * 
 */
void memory_mz1500_init ( void ) {

    DBGPRINTF ( DBGINF, "\n" );

    uint32_t i;
    uint16_t *addr;

#if 0
    // inicializace mojeho pocitace
    for ( i = 0; i < 0xffff; i += 4 ) {
        addr = ( uint16_t* ) & g_memory_mz1500.RAM [ i ];
        if ( i & 0x100 ) {
            *addr++ = 0x0000;
            *addr = 0xffff;
        } else {
            *addr++ = 0xffff;
            *addr = 0x0000;
        };
    };
#else
    // pravdepodobne standardni inicializace
    uint16_t value = 0x00ff;

    for ( i = 0; i < 0xffff; i += 4 ) {
        addr = ( uint16_t* ) & g_memory_mz1500.RAM [ i ];
        if ( ( i & 0x7f ) == 0x00 ) {
            value = ~value;
        };
        *addr++ = value;
        *addr = value;
    };
#endif

    for ( i = 0; i < MEMORY_MZ1500_SIZE_VRAM; i += 2 ) {
        addr = ( uint16_t* ) & g_memory_mz1500.VRAM [ i ];
        *addr = 0x00ff;
    };

    rom_init ( );

    g_memory_mz1500.map = 0;

    memext_init ( );
    memory_mz1500_reconnect_ram ( );

#if 0
    uint8_t *a = &g_memory_mz1500.RAM[0x2000];
    // out (0xe3),a
    *a++ = 0xd3;
    *a++ = 0xe3;

    // out (0xe6),a
    *a++ = 0xd3;
    *a++ = 0xe6;

    // ld a, znak
    *a++ = 0x3e;
    *a++ = 0x42;

    // ld (0xd000), a
    *a++ = 0x32;
    *a++ = 0x00;
    *a++ = 0xd0;

    // ld a, attr
    *a++ = 0x3e;
    *a++ = 0x70;

    // ld (0xd800), a
    *a++ = 0x32;
    *a++ = 0x00;
    *a++ = 0xd8;

    // ld a, pcg_lo
    *a++ = 0x3e;
    *a++ = 0x00;

    // ld (0xd800), a
    *a++ = 0x32;
    *a++ = 0x00;
    *a++ = 0xd4;

    // ld a, pcg_hi
    *a++ = 0x3e;
    *a++ = 0x08;

    // ld (0xd800), a
    *a++ = 0x32;
    *a++ = 0x00;
    *a++ = 0xdc;

    // ld a, dmd
    *a++ = 0x3e;
    *a++ = 0x03;

    // out (0xf0), a
    *a++ = 0xd3;
    *a++ = 0xf0;

    // ld a, mount_pcg2
    *a++ = 0x3e;
    *a++ = 0x02;

    // out (0xe5), a
    *a++ = 0xd3;
    *a++ = 0xe5;

    /* vyrobime primitivni pcg pattern */
    // ld b, 0xff
    *a++ = 0x06;
    *a++ = 0xff;

    // ld hl, 0xd000
    *a++ = 0x21;
    *a++ = 0x00;
    *a++ = 0xd0;

    // loop:
    // ld (hl), b
    *a++ = 0x70;

    // inc hl
    *a++ = 0x23;

    // djnz loop
    *a++ = 0x10;
    *a++ = 0xfc;

    // ld a, pal0 | zelena
    *a++ = 0x3e;
    *a++ = 0x04;

    // out (0xf1), a
    *a++ = 0xd3;
    *a++ = 0xf1;

    // di
    *a++ = 0xf3;

    // halt
    //*a++ = 0x76;

    // jr -1
    *a++ = 0x18;
    *a++ = 0xfe;

#endif

#ifdef MEMORY_MAKE_STATISTICS
    memset ( &g_memory_mz1500_statistics, 0x00, sizeof (g_memory_mz1500_statistics ) );

    FILE *fp;

    if ( ui_utils_file_access ( MEMORY_MZ1500_STATISTIC_DAT_FILE, F_OK ) != -1 ) {
        if ( ( fp = ui_utils_file_open ( MEMORY_MZ1500_STATISTIC_DAT_FILE, "rb" ) ) ) {
            ui_utils_file_read ( &g_memory_mz1500_statistics, 1, sizeof (g_memory_mz1500_statistics ), fp );
        } else {
            ui_show_error ( "%s() - Can't open file '%s': %s", __func__, MEMORY_MZ1500_STATISTIC_DAT_FILE, strerror ( errno ) );
        };
        fclose ( fp );
    } else {
        memory_mz1500_write_memory_statistics ( );
    };

    printf ( "Actual memory statistics (from file %s):\n", MEMORY_MZ1500_STATISTIC_DAT_FILE );
    int j;
    for ( j = 0; j <= 0x0f; j++ ) {
        printf ( "READ 0x%02x: %llu, WRITE 0x%02x: %llu\n", j, g_memory_mz1500_statistics.read[j], j, g_memory_mz1500_statistics.write[j] );
    };
    printf ( "\n\n" );


#endif
}


#if 0

#define MEMORY_DUMP_VRAM_FILE "vram.dat"


void memory_write_vram ( void ) {
    FILE *fp;

    if ( !( fp = ui_utils_file_open ( MEMORY_DUMP_VRAM_FILE, "wb" ) ) ) {
        ui_show_error ( "Can't open file '%s': %s\n", MEMORY_DUMP_VRAM_FILE, strerror ( errno ) );
        return;
    };

    if ( sizeof (g_memory_mz1500.VRAM ) != ui_utils_file_write ( &g_memory_mz1500.VRAM, 1, sizeof (g_memory_mz1500.VRAM ), fp ) ) {
        ui_show_error ( "Can't write to file '%s': %s\n", MEMORY_DUMP_VRAM_FILE, strerror ( errno ) );
    };

    if ( sizeof (g_memory_mz1500.EXVRAM ) != ui_utils_file_write ( &g_memory_mz1500.EXVRAM, 1, sizeof (g_memory_mz1500.VRAM ), fp ) ) {
        ui_show_error ( "Can't write to file '%s': %s\n", MEMORY_DUMP_VRAM_FILE, strerror ( errno ) );
    };

    fclose ( fp );
}

#endif


#ifdef MEMORY_MAKE_STATISTICS


void memory_mz1500_write_memory_statistics ( void ) {
    FILE *fp;

    if ( !( fp = ui_utils_file_open ( MEMORY_MZ1500_STATISTIC_DAT_FILE, "wb" ) ) ) {
        ui_show_error ( "Can't open file '%s': %s\n", MEMORY_MZ1500_STATISTIC_DAT_FILE, strerror ( errno ) );
        return;
    };

    if ( sizeof (g_memory_mz1500_statistics ) != ui_utils_file_write ( &g_memory_mz1500_statistics, 1, sizeof (g_memory_mz1500_statistics ), fp ) ) {
        ui_show_error ( "Can't write to file '%s': %s\n", MEMORY_MZ1500_STATISTIC_DAT_FILE, strerror ( errno ) );
    };

    fclose ( fp );

    if ( !( fp = ui_utils_file_open ( MEMORY_MZ1500_STATISTIC_TXT_FILE, "wt" ) ) ) {
        ui_show_error ( "Can't open file '%s': %s\n", MEMORY_MZ1500_STATISTIC_TXT_FILE, strerror ( errno ) );
        return;
    };

    fprintf ( fp, "Actual memory statistics (from file %s):\r\n\r\n", MEMORY_MZ1500_STATISTIC_DAT_FILE );
    int j;
    for ( j = 0; j <= 0x0f; j++ ) {
        fprintf ( fp, "READ 0x%02x: %llu, WRITE 0x%02x: %llu\r\n", j, g_memory_mz1500_statistics.read[j], j, g_memory_mz1500_statistics.write[j] );
    };
    fprintf ( fp, "\r\n\r\n" );

    fclose ( fp );

    //memory_write_vram ( );
}
#endif


/**
 * implicitni mapovani pameti po resetu
 */
void memory_mz1500_reset ( void ) {
    DBGPRINTF ( DBGINF, "\n" );
    g_memory_mz1500.map = MEMORY_MZ1500_MAP_FLAG_ROM_0000 | MEMORY_MZ1500_MAP_FLAG_ROM_UPPER;

    memext_reset ( );
    memory_reconnect_ram ( );
}


/*******************************************************************************
 *
 * Cteni z aktualne mapovane pameti
 * 
 ******************************************************************************/

/**
 * Makra pro cteni z prislusne casti pameti a nasledny return - v zavislosti na mapovani.
 * 
 * Pokud je precteno, tak makro provede return pri kterem vrati prectenou hodnotu, 
 * jinak se pokracuje a nasleduje dalsi radek z volajici funkce.
 * 
 * a = addr >> 12
 * 
 * @param addr
 * @return
 */

/*
 * 0x0000 - 0x0FFF
 * 
 * - RAM
 * - ROM
 */
#define memory_mz1500_internal_read_0000_0fff(a) { if ( 0x00 == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_0000 ) return MEMORY_MZ1500_ROM_READ_BYTE; return MEMORY_MZ1500_RAM_READ_BYTE; } }

/* 0xD000 - 0xDFFF
 * 
 * - RAM
 * - VRAM - sync.
 * - CGROM
 * - PCG1 - sync.
 * - PCG2 - sync.
 * - PCG3 - sync.
 */
#define memory_mz1500_internal_read_d000_dfff(a) { if ( 0x0d == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { return MEMORY_MZ1500_VRAM_READ_BYTE; } else if ( spec_id == 1 ) { return MEMORY_MZ1500_CGROM_READ_BYTE; } else { int pcg_id = spec_id - 2; return MEMORY_MZ1500_PCG_READ_BYTE; }; } else { return MEMORY_MZ1500_RAM_READ_BYTE; }; }; }
#define memory_mz1500_internal_read_d000_dfff_sync(a) { if ( 0x0d == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { return MEMORY_MZ1500_VRAM_READ_BYTE_SYNC; } else if ( spec_id == 1 ) { return MEMORY_MZ1500_CGROM_READ_BYTE; } else { int pcg_id = spec_id - 2; return MEMORY_MZ1500_PCG_READ_BYTE_SYNC; }; } else { return MEMORY_MZ1500_RAM_READ_BYTE; }; }; }

/* 0xE000 - 0xEFFF
 * 
 * - RAM
 * - PORTS - sync., ROM
 * - CGROM ( duplikovana )
 * - PCG1 - sync.
 * - PCG2 - sync.
 * - PCG3 - sync.
 */
#define memory_mz1500_internal_read_e000_efff(a) { if ( 0x0e == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { return memory_mz1500_internal_read_rom_e000_efff ( addr ); } else if ( spec_id == 1 ) { return MEMORY_MZ1500_CGROM_READ_BYTE; } else { int pcg_id = spec_id - 2; return MEMORY_MZ1500_PCG_READ_BYTE; }; } else { return MEMORY_MZ1500_RAM_READ_BYTE; }; }; }
#define memory_mz1500_internal_read_e000_efff_sync(a) { if ( 0x0e == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { return memory_mz1500_internal_read_rom_e000_efff_sync ( addr ); } else if ( spec_id == 1 ) { return MEMORY_MZ1500_CGROM_READ_BYTE; } else { int pcg_id = spec_id - 2; return MEMORY_MZ1500_PCG_READ_BYTE_SYNC; }; } else { return MEMORY_MZ1500_RAM_READ_BYTE; }; }; }

/* 0xF000 - 0xFFFF
 * 
 * - RAM
 * - ROM
 * - 0xff
 */
#define memory_mz1500_internal_read_f000_ffff(a) { if ( 0x0f == a ) { if ( MEMORY_MZ1500_MAP_TEST_RAM_UPPER ) { return MEMORY_MZ1500_RAM_READ_BYTE; } else { if ( !MEMORY_MZ1500_MAP_TEST_D000_SPEC ) return MEMORY_MZ1500_ROM_READ_BYTE; return 0xff; } } }


/**
 * Synchronizovane cteni z 0xe000 - 0xefff
 * 
 * @param addr
 * @return 
 */
static inline Z80EX_BYTE memory_mz1500_internal_read_rom_e000_efff_sync ( Z80EX_WORD addr ) {

    unsigned addr_low = addr & 0x0fff;

    /* cteni z horni rom */
    if ( addr_low > 0x0f ) return MEMORY_MZ1500_ROM_READ_BYTE;

    /* cteni z E009 - E00F */
    if ( addr_low > 0x08 ) return g_mz800.regDBUS_latch;

    /* cteni z E008 ( regDMD ) */
    if ( 0x08 == addr_low ) {
        mz800_sync_insideop_mreq_e00x ( );
        return gdg_mz1500_read_dmd_status ( );
    };

    /* cteni z CTC8253 */
    if ( addr_low & 0x04 ) {

        /* Kontrol registr cist nelze */
        if ( 0x07 == addr_low ) {
            return g_mz800.regDBUS_latch;
        };
        mz800_sync_insideop_mreq_e00x ( );
        return ctc8253_read_byte ( addr_low & 0x03 );
    };

    /* cteni z PIO8255 */
    mz800_sync_insideop_mreq_e00x ( );
    return pio8255_read ( addr & 0x03 );
}


/**
 * Cteni z 0xe000 - 0xefff - bez synchronizace
 * 
 * @param addr
 * @return 
 */
static inline Z80EX_BYTE memory_mz1500_internal_read_rom_e000_efff ( Z80EX_WORD addr ) {
    unsigned addr_low = addr & 0x0fff;

    /* cteni z horni rom */
    if ( addr_low > 0x0f ) return MEMORY_MZ1500_ROM_READ_BYTE;

    /* cteni z E009 - E00F */
    if ( addr_low > 0x08 ) return g_mz800.regDBUS_latch;

    /* cteni z E008 ( regDMD ) */
    if ( 0x08 == addr_low ) return gdg_mz1500_read_dmd_status ( );


    /* cteni z CTC8253 */
    if ( addr_low & 0x04 ) {

        /* Kontrol registr cist nelze */
        if ( 0x07 == addr_low ) {
            return g_mz800.regDBUS_latch;
        };
        // TODO: prozatim vracime 0x00
        return 0x00;
    };

    /* cteni z PIO8255 */
    return pio8255_read ( addr & 0x03 );
}


Z80EX_BYTE memory_mz1500_internal_read_sync ( Z80EX_WORD addr ) {

    unsigned addr_high = addr >> 12;

#ifdef MEMORY_MAKE_STATISTICS
    g_memory_mz1500_statistics.read[addr_high]++;
#endif

    memory_mz1500_internal_read_0000_0fff ( addr_high );
    memory_mz1500_internal_read_d000_dfff_sync ( addr_high );
    memory_mz1500_internal_read_e000_efff_sync ( addr_high );
    memory_mz1500_internal_read_f000_ffff ( addr_high );
    return MEMORY_MZ1500_RAM_READ_BYTE;
}


/**
 * Cteni z aktualne mapovane pameti se zachovanim synchronizace u VRAM a 0xe00x periferii.
 * 
 * @param cpu
 * @param addr
 * @param m1_state
 * @param user_data
 * @return 
 */
Z80EX_BYTE memory_mz1500_read_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data ) {
    Z80EX_BYTE retval = memory_mz1500_internal_read_sync ( addr );
    g_mz800.regDBUS_latch = retval;
    return retval;
}



#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED


/**
 * Cteni z aktualne mapovane pameti se zachovanim synchronizace u VRAM a 0xe00x periferii.
 * + debugging = ukladani historie poslednich vykonanych bajtu
 * 
 * @param cpu
 * @param addr
 * @param m1_state
 * @param user_data
 * @return 
 */
Z80EX_BYTE memory_mz1500_read_with_history_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data ) {
    Z80EX_BYTE retval = memory_mz1500_internal_read_sync ( addr );
    g_mz800.regDBUS_latch = retval;

    if ( ( m1_state ) && ( g_mz800.cpu->prefix == 0x00 ) ) {
        g_debugger_history.position++;
        int position = debugger_history_position ( g_debugger_history.position );
        g_debugger_history.row[position].addr = addr;
        g_debugger_history.row[position].byte[0] = retval;
        g_debugger_history.byte_position = 1;
    } else if ( g_debugger_history.byte_position < DEBUGGER_MAX_INSTR_BYTES ) {
        int position = debugger_history_position ( g_debugger_history.position );
        g_debugger_history.row[position].byte[g_debugger_history.byte_position++] = retval;
    };

    return retval;
}
#endif


/**
 * Cteni z aktualne mapovane pameti - bez synchronizace.
 * 
 * @param addr
 * @return 
 */
Z80EX_BYTE memory_mz1500_read_byte ( Z80EX_WORD addr ) {
    unsigned addr_high = addr >> 12;
    memory_mz1500_internal_read_0000_0fff ( addr_high );
    memory_mz1500_internal_read_d000_dfff ( addr_high );
    memory_mz1500_internal_read_e000_efff ( addr_high );
    memory_mz1500_internal_read_f000_ffff ( addr_high );
    return MEMORY_MZ1500_RAM_READ_BYTE;
}



/*******************************************************************************
 *
 * Zapis do aktualne mapovane pameti
 * 
 ******************************************************************************/


/**
 * Makra pro zapis do prislusne casti pameti a nasledny return - v zavislosti na mapovani.
 * 
 * Pokud je zapsano, tak makro provede return,
 * jinak se pokracuje a nasleduje dalsi radek z volajici funkce.
 * 
 * a = addr >> 12
 * 
 * @param addr
 * @param value
 */

/*
 * 0x0000 - 0x0FFF
 * 
 * - RAM
 * - ROM
 */
#define memory_mz1500_internal_write_0000_0fff(a) { if ( 0x00 == a ) { if ( ! MEMORY_MZ1500_MAP_TEST_ROM_0000 ) MEMORY_MZ1500_RAM_WRITE_BYTE; return; } }

/*
 * 0x1000 - 0xCFFF
 * 
 * - RAM
 * - ROM
 */
#define memory_mz1500_internal_write_1000_cfff(a) { if ( ( 0x01 <= a ) && ( 0x0c >= a ) ) { MEMORY_MZ1500_RAM_WRITE_BYTE; return; } }

/* 0xD000 - 0xDFFF
 * 
 * - RAM
 * - VRAM - sync.
 * - CGROM
 * - PCG1 - sync.
 * - PCG2 - sync.
 * - PCG3 - sync.
 */
#define memory_mz1500_internal_write_d000_dfff(a) { if ( 0x0d == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { MEMORY_MZ1500_VRAM_WRITE_BYTE; return; } else if ( spec_id == 1 ) { return; } else { int pcg_id = spec_id - 2; MEMORY_MZ1500_PCG_WRITE_BYTE_SYNC; return; }; } else { MEMORY_MZ1500_RAM_WRITE_BYTE; return; }; }; }
#define memory_mz1500_internal_write_d000_dfff_sync(a) { if ( 0x0d == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { MEMORY_MZ1500_VRAM_WRITE_BYTE_SYNC; return; } else if ( spec_id == 1 ) { return; } else { int pcg_id = spec_id - 2; MEMORY_MZ1500_PCG_WRITE_BYTE_SYNC; return; }; } else { MEMORY_MZ1500_RAM_WRITE_BYTE; return; }; }; }

/* 0xE000 - 0xEFFF
 * 
 * - RAM
 * - PORTS - sync., ROM
 * - CGROM ( duplikovana )
 * - PCG1 - sync.
 * - PCG2 - sync.
 * - PCG3 - sync.
 */
#define memory_mz1500_internal_write_e000_efff(a) { if ( 0x0e == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { memory_mz1500_internal_write_rom_e000 ( addr, value ); return; } else if ( spec_id == 1 ) { return; } else { int pcg_id = spec_id - 2; MEMORY_MZ1500_PCG_WRITE_BYTE; return; }; } else { MEMORY_MZ1500_RAM_WRITE_BYTE; return; }; }; }
#define memory_mz1500_internal_write_e000_efff_sync(a) { if ( 0x0e == a ) { if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) { int spec_id = MEMORY_MZ1500_SPEC_ID; if ( spec_id == 0 ) { memory_mz1500_internal_write_rom_e000_sync ( addr, value ); return; } else if ( spec_id == 1 ) { return; } else { int pcg_id = spec_id - 2; MEMORY_MZ1500_PCG_WRITE_BYTE_SYNC; return; }; } else { MEMORY_MZ1500_RAM_WRITE_BYTE; return; }; }; }

/* 0xF000 - 0xFFFF
 * 
 * - RAM
 * - ROM
 * - 0xff
 */
#define memory_mz1500_internal_write_f000_ffff(a) { if ( 0x0f == a ) { if ( MEMORY_MZ1500_MAP_TEST_RAM_UPPER ) { MEMORY_MZ1500_RAM_WRITE_BYTE; return; } else {  return; } } }


static inline void memory_mz1500_internal_write_rom_e000_sync ( Z80EX_WORD addr, Z80EX_BYTE value ) {

    if ( addr > 0xe008 ) return;

    mz800_sync_insideop_mreq_e00x ( );

    if ( addr == 0xe008 ) {
        gdg_mz1500_write_byte ( addr, value );
    } else if ( addr & 0x04 ) {
        ctc8253_write_byte ( addr & 0x03, value );
    } else {
        pio8255_write ( addr & 0x03, value );
    };
}


static inline void memory_mz1500_internal_write_rom_e000 ( Z80EX_WORD addr, Z80EX_BYTE value ) {

    if ( addr > 0xe008 ) return;

    if ( addr == 0xe008 ) {
        gdg_mz1500_write_byte ( addr, value );
    } else if ( addr & 0x04 ) {
        ctc8253_write_byte ( addr & 0x03, value );
    } else {
        pio8255_write ( addr & 0x03, value );
    };
}


/**
 * Zapis do aktualne mapovane pameti s ohledem na synchronizaci.
 * 
 * @param cpu
 * @param addr
 * @param value
 * @param user_data
 */
void memory_mz1500_write_cb ( Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data ) {
    unsigned addr_high = addr >> 12;

#ifdef MEMORY_MAKE_STATISTICS
    g_memory_mz1500_statistics.write[addr_high]++;
#endif


    memory_mz1500_internal_write_0000_0fff ( addr_high );
    memory_mz1500_internal_write_1000_cfff ( addr_high );

    if ( addr == 0xd813 ) {
        //printf ( "WR 0xd813: 0x%02x, mmap: 0x%02x, spec_id: 0x%02x, pcg_id: 0x%02x\n", value, g_memory_mz1500.map, MEMORY_MZ1500_SPEC_ID, MEMORY_MZ1500_SPEC_ID - 2 );
        if ( 0x0d == addr_high ) {
            if ( MEMORY_MZ1500_MAP_TEST_ROM_UPPER ) {
                int spec_id = MEMORY_MZ1500_SPEC_ID;
                if ( spec_id == 0 ) {
                    MEMORY_MZ1500_VRAM_WRITE_BYTE_SYNC;
                    return;
                } else if ( spec_id == 1 ) {
                    return;
                } else {
                    int pcg_id = spec_id - 2;
                    MEMORY_MZ1500_PCG_WRITE_BYTE_SYNC;
                    return;
                };
            } else {
                MEMORY_MZ1500_RAM_WRITE_BYTE;
                return;
            };
        };
    } else {
        memory_mz1500_internal_write_d000_dfff_sync ( addr_high );
    };

    memory_mz1500_internal_write_e000_efff_sync ( addr_high );
    memory_mz1500_internal_write_f000_ffff ( addr_high );
}


/**
 * Zapis do aktualne mapovane pameti - bez synchronizace.
 * 
 * @param addr
 * @param value
 */
void memory_mz1500_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value ) {
    unsigned addr_high = addr >> 12;
    memory_mz1500_internal_write_0000_0fff ( addr_high );
    memory_mz1500_internal_write_1000_cfff ( addr_high );
    memory_mz1500_internal_write_d000_dfff ( addr_high );
    memory_mz1500_internal_write_e000_efff ( addr_high );
    memory_mz1500_internal_write_f000_ffff ( addr_high );
}


#if 0
// nenaimplementovano v MZ1500EMU


/**
 * Nacteni datoveho bloku do pameti.
 * 
 * @param data
 * @param addr
 * @param size
 * @param type - MEMORY_LOAD_MAPPED, MEMORY_LOAD_RAMONLY
 */
void memory_mz1500_load_block ( uint8_t *data, Z80EX_WORD addr, Z80EX_WORD size, en_MEMORY_LOAD type ) {
    Z80EX_WORD src_addr = 0;

    while ( size ) {
        //uint8_t *dst = &g_memory.RAM[addr];
        uint8_t *dst = g_memory_mz1500.memram_write[( addr ) >> 12] + ( addr & 0x0fff );
        uint8_t *src = &data[src_addr];
        uint32_t total_size = addr + size;
        uint32_t load_size;

        if ( type == MEMORY_LOAD_RAMONLY ) {
            uint32_t limit = ( ( addr >> 12 ) + 1 ) << 12;
            load_size = ( total_size < limit ) ? size : ( limit - addr );
            memcpy ( dst, src, load_size );
        } else {
            if ( addr < 0x1000 ) {
                load_size = ( total_size < 0x1000 ) ? size : ( 0x1000 - addr );
                if ( !MEMORY_MZ800_MAP_TEST_ROM_0000 ) {
                    memcpy ( dst, src, load_size );
                };
            } else if ( addr < 0x2000 ) {
                load_size = ( total_size < 0x2000 ) ? size : ( 0x2000 - addr );
                if ( !MEMORY_MZ800_MAP_TEST_ROM_1000 ) {
                    memcpy ( dst, src, load_size );
                };
            } else if ( addr < 0x8000 ) {
                uint32_t limit = ( ( addr >> 12 ) + 1 ) << 12;
                load_size = ( total_size < limit ) ? size : ( limit - addr );
                memcpy ( dst, src, load_size );
            } else if ( addr < 0xa000 ) {
                load_size = ( total_size < 0xa000 ) ? size : ( 0xa000 - addr );
                if ( !MEMORY_MZ800_MAP_TEST_VRAM_8000 ) {
                    uint32_t limit = ( ( addr >> 12 ) + 1 ) << 12;
                    load_size = ( total_size < limit ) ? size : ( limit - addr );
                    memcpy ( dst, src, load_size );
                } else {
                    int i;
                    for ( i = 0; i < load_size; i++ ) {
                        vramctrl_mz800_mode800_memop_write_byte ( ( addr + i ) & 0x3fff, data[( src_addr + i )] );
                    };
                };
            } else if ( addr < 0xc000 ) {
                load_size = ( total_size < 0xc000 ) ? size : ( 0xc000 - addr );
                if ( !MEMORY_MZ800_MAP_TEST_VRAM_A000 ) {
                    uint32_t limit = ( ( addr >> 12 ) + 1 ) << 12;
                    load_size = ( total_size < limit ) ? size : ( limit - addr );
                    memcpy ( dst, src, load_size );
                } else {
                    int i;
                    for ( i = 0; i < load_size; i++ ) {
                        vramctrl_mz800_mode800_memop_write_byte ( ( addr + i ) & 0x3fff, data[( src_addr + i )] );
                    };
                };
            } else if ( addr < 0xd000 ) {
                load_size = ( total_size < 0xd000 ) ? size : ( 0xd000 - addr );
                if ( MEMORY_MZ800_MAP_TEST_CGRAM ) {
                    dst = &g_memory_mz800_VRAM_I[( addr & 0x0fff )];
                };
                memcpy ( dst, src, load_size );
            } else if ( addr < 0xe000 ) {
                load_size = ( total_size < 0xe000 ) ? size : ( 0xe000 - addr );
                if ( MEMORY_MZ800_MAP_TEST_VRAM_D000 ) {
                    dst = &g_memory_mz800_VRAM_I[( 0x1000 | ( addr & 0x0fff ) )];
                };
                memcpy ( dst, src, load_size );
            } else {
                uint32_t limit = ( ( addr >> 12 ) + 1 ) << 12;
                load_size = ( total_size < limit ) ? size : ( limit - addr );
                if ( !MEMORY_MZ800_MAP_TEST_ROM_E000 ) {
                    memcpy ( dst, src, load_size );
                };
            };
        };

        size -= load_size;
        src_addr += load_size;
        addr += load_size;
    };
}
#endif
#endif
