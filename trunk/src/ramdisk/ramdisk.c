/* 
 * File:   ramdisk.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 10. srpna 2015, 11:05
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

/*
 *
 *	Standardni ramdisk (max. velikost 256 bank * 64 KB = 16 MB)
 *
 *
 *	Rezim RD:
 *			-w 0xe9 - nastaveni stranky 0x00 - 0xff
 *			rw 0xea - R/W data + increment adresy v RD
 *			-w 0xeb - nastaveni dolnich 16 bitu adresy RD
 *			r- 0xf8 - reset adresy a stranky
 *
 *
 *	Rezim SRAM:
 *			r- 0xf8 - reset adresy
 *			r- 0xf9 - cteni dat + increment adresy
 *			-w 0xfa - zapis dat + increment adresy
 *
 *
 *
 *
 *
 *	Standartni PEZIK:
 *
 *
 *		Pouziva porty 0xe8 - 0xef
 *
 *		Cislo portu vzdy urcuje banku se kterou pracujeme.
 *
 *		Pri IORQ RD se vzdy do latch registru prenese horni cast sbernice.
 *		Po dokonceni RD operace je obsah tohoto latche vystaven jako spodni adresa offsetu.
 *
 */

#ifdef WIN32
#include<windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

#include "mz800.h"
#include "ramdisk.h"

#include "ui/ui_main.h"
#include "ui/ui_ramdisk.h"

#include "cfgmain.h"

#include "ui/ui_utils.h"


//#define DBGLEVEL        ( DBGNON /* | DBGERR | DBGWAR | DBGINF */ )
//#define DBGLEVEL        ( DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"

#define DEF_BANK_SIZE		0x10000

st_RAMDISK g_ramdisk;


void ramdisk_std_save ( void ) {

    FILE *fp;

    if ( ( g_ramdisk.std.connected ) && ( g_ramdisk.std.memory != NULL ) ) {
        if ( g_ramdisk.std.type == RAMDISK_TYPE_SRAM ) {
            if ( ( fp = ui_utils_fopen ( g_ramdisk.std.filepath, "wb" ) ) ) {
                unsigned ramdisksize = ( g_ramdisk.std.size + 1 ) * 0x10000;
                unsigned filesize = fwrite ( g_ramdisk.std.memory, 1, ramdisksize, fp );
                if ( filesize != ramdisksize ) {
                    ui_show_error ( "%s() - saved only %d bytes of %d - file '%s': %s", __func__, filesize, ramdisksize, g_ramdisk.std.filepath, strerror ( errno ) );
                };
                fclose ( fp );

            } else {
                ui_show_error ( "%s() - Can't open file '%s': %s", __func__, g_ramdisk.std.filepath, strerror ( errno ) );
            };
        };
    };
}


void ramdisk_std_disconnect ( void ) {

    ramdisk_std_save ( );

    if ( g_ramdisk.std.memory != NULL ) {
        free ( g_ramdisk.std.memory );
        g_ramdisk.std.memory = NULL;
    };
    g_ramdisk.std.connected = RAMDISK_DISCONNECTED;
}


void ramdisk_std_init ( int connect, en_RAMDISK_TYPE type, en_RAMDISK_BANKMASK size, char *filepath ) {

    FILE *fp;

    DBGPRINTF ( DBGINF, "connect = %d, type = %d, bank_mask = 0x%02x, file = %s\n", connect, type, size, filepath );

    if ( connect ) {
        /* nepovolena kombinace */
        if ( g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].connected ) {
            g_ramdisk.std.connected = RAMDISK_DISCONNECTED;
            return;
        };

        ramdisk_std_save ( );

        if ( g_ramdisk.std.memory == NULL ) {
            g_ramdisk.std.memory = malloc ( ( size + 1 ) * 0x10000 );

            if ( g_ramdisk.std.memory == NULL ) {
                fprintf ( stderr, "%s():%d - Could not allocate memory: %s\n", __func__, __LINE__, strerror ( errno ) );
                main_app_quit ( EXIT_FAILURE );
            };

            /* implicitni obsah nesmi byt 0x00 - jinak se pri bootu zacne nacitat program :) */
            /* TODO: radeji to jeste proverime */
            memset ( g_ramdisk.std.memory, 0xff, ( size + 1 ) * 0x10000 );
        } else {
            g_ramdisk.std.memory = realloc ( g_ramdisk.std.memory, ( size + 1 ) * 0x10000 );

            if ( g_ramdisk.std.memory == NULL ) {
                fprintf ( stderr, "%s():%d - Could not allocate memory: %s\n", __func__, __LINE__, strerror ( errno ) );
                main_app_quit ( EXIT_FAILURE );
            };
        };

        g_ramdisk.std.size = size;
        g_ramdisk.std.type = type;

        if ( g_ramdisk.std.type & RAMDISK_IS_IN_FILE ) {
            if ( filepath[0] != 0x00 ) {
                strncpy ( g_ramdisk.std.filepath, filepath, sizeof ( g_ramdisk.std.filepath ) );
            } else {
                strncpy ( g_ramdisk.std.filepath, RAMDISK_DEFAULT_FILENAME, sizeof ( g_ramdisk.std.filepath ) );
            };
            g_ramdisk.std.filepath [ sizeof ( g_ramdisk.std.filepath ) - 1 ] = 0x00;

            if ( ui_utils_access ( g_ramdisk.std.filepath, F_OK ) != -1 ) {
                if ( ( fp = ui_utils_fopen ( g_ramdisk.std.filepath, "rb" ) ) ) {
                    unsigned ramdisksize = ( g_ramdisk.std.size + 1 ) * 0x10000;
                    unsigned filesize = fread ( g_ramdisk.std.memory, 1, ( g_ramdisk.std.size + 1 ) * 0x10000, fp );
                    if ( filesize != ramdisksize ) {
                        ui_show_warning ( "Your RD file has only %d bytes of requested %d bytes. Peace, this is not problem ... this is only warning :)", filesize, ramdisksize );
                    };
                } else {
                    ui_show_error ( "%s() - Can't open file '%s': %s", __func__, g_ramdisk.std.filepath, strerror ( errno ) );
                };
                fclose ( fp );
            };
        };

    } else {
        ramdisk_std_disconnect ( );
        g_ramdisk.std.size = size;
        g_ramdisk.std.type = type;
        strncpy ( g_ramdisk.std.filepath, filepath, sizeof ( g_ramdisk.std.filepath ) );
        g_ramdisk.std.filepath [ sizeof ( g_ramdisk.std.filepath ) - 1 ] = 0x00;
    };
    g_ramdisk.std.connected = connect;
}


void ramdisk_std_open_file ( void ) {

    char filename [ RAMDISK_FILENAME_LENGTH ];

    filename[0] = 0x00;
    if ( g_ramdisk.std.type == RAMDISK_TYPE_SRAM ) {
        char window_title[] = "Select SRAM disk DAT file to open";
        ui_open_file ( filename, g_ramdisk.std.filepath, sizeof ( filename ), FILETYPE_DAT, window_title, OPENMODE_SAVE );
    } else {
        char window_title[] = "Select ROM disk DAT file to open";
        ui_open_file ( filename, g_ramdisk.std.filepath, sizeof ( filename ), FILETYPE_DAT, window_title, OPENMODE_READ );
    };

    if ( filename[0] != 0x00 ) {
        ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, g_ramdisk.std.size, filename );
    };
}


void ramdisk_pezik_init ( int pezik_type, int connect ) {

    pezik_type &= 0x01;

    if ( !connect ) {
        if ( g_ramdisk.pezik [ pezik_type ].memory != NULL ) {
            free ( g_ramdisk.pezik [ pezik_type ].memory );
            g_ramdisk.pezik [ pezik_type ].memory = NULL;
        };
    } else {
        if ( ( pezik_type == RAMDISK_PEZIK_E8 ) && ( g_ramdisk.std.connected ) ) {
            /* nepovolena kombinace */
            g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ]. connected = RAMDISK_DISCONNECTED;
            return;
        };
        if ( g_ramdisk.std.memory == NULL ) {
            g_ramdisk.pezik [ pezik_type ].memory = malloc ( 8 * 0x10000 );

            if ( g_ramdisk.pezik [ pezik_type ].memory == NULL ) {
                fprintf ( stderr, "%s():%d - Could not allocate memory: %s\n", __func__, __LINE__, strerror ( errno ) );
                main_app_quit ( EXIT_FAILURE );
            };

            memset ( g_ramdisk.pezik [ pezik_type ].memory, 0xff, 8 * 0x10000 );
        };
    };
    g_ramdisk.pezik [ pezik_type ].connected = connect;
}


void ramdisk_propagatecfg ( void *m, void *data ) {

    ramdisk_pezik_init ( RAMDISK_PEZIK_E8, cfgmodule_get_element_bool_value_by_name ( (CFGMOD *) m, "pezik_std_e8_pluged" ) );
    ramdisk_pezik_init ( RAMDISK_PEZIK_68, cfgmodule_get_element_bool_value_by_name ( (CFGMOD *) m, "pezik_std_e8_pluged" ) );

    int mr1r18_pluged = cfgmodule_get_element_bool_value_by_name ( (CFGMOD *) m, "mr1r18_pluged" );
    en_RAMDISK_TYPE mr1r18_type = cfgmodule_get_element_keyword_value_by_name ( (CFGMOD *) m, "mr1r18_type" );
    en_RAMDISK_BANKMASK mr1r18_size = cfgmodule_get_element_keyword_value_by_name ( (CFGMOD *) m, "mr1r18_size" );
    char *mr1r18_filepath = cfgmodule_get_element_text_value_by_name ( (CFGMOD *) m, "mr1r18_filepath" );

    ramdisk_std_init ( mr1r18_pluged, mr1r18_type, mr1r18_size, mr1r18_filepath );

    ui_ramdisk_update_menu ( );
}


void ramdisk_init ( void ) {

    memset ( &g_ramdisk, 0x00, sizeof ( g_ramdisk ) );

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "RAMDISK" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "mr1r18_pluged", CFGENTYPE_BOOL, RAMDISK_CONNECTED );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_ramdisk.std.connected );

    elm = cfgmodule_register_new_element ( cmod, "mr1r18_type", CFGENTYPE_KEYWORD, RAMDISK_TYPE_SRAM,
            RAMDISK_TYPE_STD, "STANDARD",
            RAMDISK_TYPE_SRAM, "SRAM",
            RAMDISK_TYPE_ROM, "ROM",
            -1 );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_ramdisk.std.type );


    elm = cfgmodule_register_new_element ( cmod, "mr1r18_size", CFGENTYPE_KEYWORD, RAMDISK_SIZE_1M,
            RAMDISK_SIZE_64, "64K",
            RAMDISK_SIZE_256, "256K",
            RAMDISK_SIZE_512, "512K",
            RAMDISK_SIZE_1M, "1M",
            RAMDISK_SIZE_16M, "16M",
            -1 );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_ramdisk.std.size );

    elm = cfgmodule_register_new_element ( cmod, "mr1r18_filepath", CFGENTYPE_TEXT, RAMDISK_DEFAULT_FILENAME );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_ramdisk.std.filepath );

    elm = cfgmodule_register_new_element ( cmod, "pezik_std_e8_pluged", CFGENTYPE_BOOL, RAMDISK_DISCONNECTED );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_ramdisk.pezik[RAMDISK_PEZIK_E8].connected );

    elm = cfgmodule_register_new_element ( cmod, "pezik_shifted_68_pluged", CFGENTYPE_BOOL, RAMDISK_DISCONNECTED );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_ramdisk.pezik[RAMDISK_PEZIK_68].connected );

    cfgmodule_set_propagate_cb ( cmod, ramdisk_propagatecfg, NULL );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );
}


void ramdisc_exit ( void ) {
    int i;

    ramdisk_std_save ( );

    if ( ( g_ramdisk.std.connected ) && ( g_ramdisk.std.memory != NULL ) ) {
        free ( g_ramdisk.std.memory );
    };

    for ( i = 0; i < 2; i++ ) {
        if ( ( g_ramdisk.pezik [ i ]. connected ) && ( g_ramdisk.pezik [ i ]. memory != NULL ) ) {
            free ( g_ramdisk.pezik [ i ]. memory );
        };
    };
}


Z80EX_BYTE ramdisk_std_read_byte ( unsigned addr ) {

    int ramdisc_addr;

    DBGPRINTF ( DBGINF, "addr = 0x%02x\n", addr );

    switch ( addr ) {

        case 0xf8:
            g_ramdisk.std.bank = 0x00;
            g_ramdisk.std.offset = 0x0000;
            return g_mz800.regDBUS_latch;

        case 0xea:
        case 0xf9:
            ramdisc_addr = ( g_ramdisk.std.bank << 16 ) | g_ramdisk.std.offset++;
            return g_ramdisk.std.memory [ ramdisc_addr ];
    };
    return 0;
}


void ramdisk_std_write_byte ( unsigned addr, Z80EX_BYTE value ) {

    unsigned ramdisc_addr;

    switch ( addr & 0xff ) {

        case 0xe9:
            g_ramdisk.std.bank = value & g_ramdisk.std.size;
            break;

        case 0xea:
        case 0xfa:
            ramdisc_addr = ( g_ramdisk.std.bank << 16 ) | g_ramdisk.std.offset++;
            if ( !( g_ramdisk.std.type & RAMDISK_IS_READONLY ) ) {
                g_ramdisk.std.memory [ ramdisc_addr ] = value;
            }
            break;

        case 0xeb:
            g_ramdisk.std.offset = ( addr & 0xff00 ) | value;
            break;
    };
}


Z80EX_BYTE ramdisk_pezik_read_byte ( unsigned addr ) {

    int pezik_addr;
    int pezik_type;

    pezik_type = ( addr & 0x80 ) >> 7;
    pezik_addr = ( ( addr & 0x07 ) << 16 ) | ( addr & 0xff00 ) | g_ramdisk.pezik [ pezik_type ] . latch;
    g_ramdisk.pezik [ pezik_type ] . latch = ( addr & 0xff00 ) >> 8;

    return g_ramdisk.pezik [ pezik_type ] . memory [ pezik_addr ];
}


void ramdisk_pezik_write_byte ( unsigned addr, Z80EX_BYTE value ) {

    unsigned pezik_addr;
    unsigned pezik_type;

    pezik_type = ( addr & 0x80 ) >> 7;
    pezik_addr = ( ( addr & 0x07 ) << 16 ) | ( addr & 0xff00 ) | g_ramdisk.pezik [ pezik_type ] . latch;

    g_ramdisk.pezik [ pezik_type ] . memory [ pezik_addr ] = value;
}

