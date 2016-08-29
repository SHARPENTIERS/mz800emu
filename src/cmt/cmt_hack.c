/* 
 * File:   cmt_hack.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 2. července 2015, 20:49
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
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "cmt_hack.h"

#include "z80ex/include/z80ex.h"
#include "memory/memory.h"
#include "memory/rom.h"
#include "mz800.h"

#include "ui/ui_main.h"
#include "ui/ui_cmt.h"

#include "cfgmain.h"

#include "ui/ui_utils.h"

st_CMTHACK g_cmthack;

typedef enum en_LOADRET {
    LOADRET_OK = 0,
    LOADRET_ERROR,
    LOADRET_BREAK
} en_LOADRET;


void cmthack_install_rom_patch ( void ) {

    /* ROM hack */

    /* 
     * Precteni hlavicky z CMT
     *
     *	RHEAD:
     *
     *	Vystup: CY = 1  doslo k chybe
     *			A = 1  chyba kontrolniho souctu
     *			A = 2  detekovano BREAK
     *		CY = 0  O.K.
     *
     */
    g_memory.ROM [ 0x04d8 ] = 0xe5; /* push HL */
    g_memory.ROM [ 0x04d9 ] = 0x21; /* ld HL, 0x10f0 */
    g_memory.ROM [ 0x04da ] = 0xf0;
    g_memory.ROM [ 0x04db ] = 0x10;
    g_memory.ROM [ 0x04dc ] = 0xd3; /* out (0x01), a */
    g_memory.ROM [ 0x04dd ] = 0x01;
    g_memory.ROM [ 0x04de ] = 0xe1; /* pop HL */
    g_memory.ROM [ 0x04df ] = 0xc9; /* ret */


    /*
     *	RDATA:
     *
     * Precte program z CMT podle informaci
     *
     *	vystup:  CY s vyznamem jako u RHEAD
     *
     */
    g_memory.ROM [ 0x04f8 ] = 0xe5; /* push HL */
    g_memory.ROM [ 0x04f9 ] = 0xc5; /* push BC */
    g_memory.ROM [ 0x04fa ] = 0x2a; /* ld HL, (0x1104) */
    g_memory.ROM [ 0x04fb ] = 0x04;
    g_memory.ROM [ 0x04fc ] = 0x11;
    g_memory.ROM [ 0x04fd ] = 0xed; /* ld BC, (0x1102) */
    g_memory.ROM [ 0x04fe ] = 0x4b;
    g_memory.ROM [ 0x04ff ] = 0x02;
    g_memory.ROM [ 0x0500 ] = 0x11;
    g_memory.ROM [ 0x0501 ] = 0xd3; /* out (0x02), A */
    g_memory.ROM [ 0x0502 ] = 0x02;
    g_memory.ROM [ 0x0503 ] = 0xc1; /* pop BC */
    g_memory.ROM [ 0x0504 ] = 0xe1; /* pop BC */
    g_memory.ROM [ 0x0505 ] = 0xc9; /* ret */


    /*
     * 
     * JSS 1.3 a 1.6A na adrese 0xf3bb (0xf3b8) provadi kontrolu horni a dolni ROM:
     * 
     * xor a, nasledne xor 0x0000 - 0x0xfff, 0xe010 - 0xffff
     * 
     * obsah porovna s tim co je na adrese 0xe840 ( == 0xed)
     * 
     * Bohuzel nikdo uz asi nikdy nezjisti na ktere adrese dela JSS korekci, aby 
     * byl vysledny XOR = 0xed
     * 
     */

    g_memory.ROM [ 0x0506 ] = 0x59; /* pokud zde dame 0x00, tak XOR vychazi 0xb4, proto: 0xb4 ^ 0xed = 0x59 */

}


void cmthack_reinstall_rom_patch ( void ) {
    if ( g_cmthack.load_patch_installed ) {
        cmthack_install_rom_patch ( );
    };
}


void cmthack_load_rom_patch ( unsigned enabled ) {

    if ( enabled ) {
        cmthack_install_rom_patch ( );
    } else {
        memcpy ( &g_memory.ROM [ 0x04d8 ], &g_rom.mz700rom [ 0x04d8 ], 8 );
        memcpy ( &g_memory.ROM [ 0x04f8 ], &g_rom.mz700rom [ 0x04f8 ], 15 );
    };
    g_cmthack.load_patch_installed = enabled & 1;

    ui_cmt_hack_menu_update ( );
}


void cmthack_reset ( void ) {
    /* pokud jsme meli otevreny MZF soubor, tak ho zavrem */
    if ( g_cmthack.fp ) {
        fclose ( g_cmthack.fp );
        g_cmthack.fp = NULL;
    };
}


void cmthack_exit ( void ) {
    /* pokud jsme meli otevreny MZF soubor, tak ho zavrem */
    if ( g_cmthack.fp ) {
        fclose ( g_cmthack.fp );
        g_cmthack.fp = NULL;
    };
}


#define DEFAULT_CMT_HACK_ENABLE   1


void cmthack_propagatecfg_load_rom_patch ( void *e, void *data ) {
    cmthack_load_rom_patch ( cfgelement_get_bool_value ( (CFGELM *) e ) );
}


void cmthack_init ( void ) {

    g_cmthack.fp = NULL;
    g_cmthack.filename[0] = 0x00;
    g_cmthack.load_patch_installed = 0;


    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "CMTHACK" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "enable", CFGENTYPE_BOOL, DEFAULT_CMT_HACK_ENABLE );
    cfgelement_set_propagate_cb ( elm, cmthack_propagatecfg_load_rom_patch, NULL );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_cmthack.load_patch_installed );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );
}


void cmthack_result ( en_LOADRET result ) {

    Z80EX_WORD reg_af = z80ex_get_reg ( g_mz800.cpu, regAF );

    if ( result == LOADRET_OK ) {
        reg_af &= ~0x01; /* reset CARRY flag */
    } else {
        reg_af |= 0x01; /* set CARRY flag */
        reg_af |= ( (Z80EX_WORD) result << 8 ); /* result do regA */
        reg_af &= ( (Z80EX_WORD) result << 8 ) | 0xff;
    };
    z80ex_set_reg ( g_mz800.cpu, regAF, reg_af );
}


/* 
 * 
 * Pozadavek na precteni headeru z CMT
 * 
 */
void cmthack_load_file ( void ) {

    mz800_flush_full_screen ( );

    char filename [ CMTHACK_FILENAME_LENGTH ];

    char window_title[] = "Select MZF file to open";
    filename[0] = 0x00;
    if ( UIRET_OK != ui_open_file ( filename, g_cmthack.filename, sizeof ( filename ), FILETYPE_MZF, window_title, OPENMODE_READ ) ) {
        /* Zruseno: nastavit Err + Break */
        cmthack_result ( LOADRET_BREAK );
        return;
    };
    
    cmthack_load_filename ( filename );
}


void cmthack_load_filename ( char *filename ) {

    unsigned reg_hl;

    if ( g_cmthack.fp != NULL ) {
        fclose ( g_cmthack.fp );
        g_cmthack.fp = NULL;
    };

    if ( !( g_cmthack.fp = ui_utils_fopen ( filename, "rb" ) ) ) {
        /* Nejde otevrit soubor: nastavit Err + Break */
        ui_show_error ( "Can't open MZF file '%s': %s\n", filename, strerror ( errno ) );
        cmthack_result ( LOADRET_BREAK );
        return;
    };

    strncpy ( g_cmthack.filename, filename, sizeof ( g_cmthack.filename ) );

    /* precteme prvnich 128 bajtu z MZF souboru a ulozime je do RAM na adresu z regHL */

    reg_hl = z80ex_get_reg ( g_mz800.cpu, regHL );
    if ( 0x80 != ui_utils_fread ( &g_memory.RAM [ reg_hl ], 1, 0x80, g_cmthack.fp ) ) {
        /* Vadny soubor: nastavit Err + Checksum */
        cmthack_result ( LOADRET_ERROR );
        fclose ( g_cmthack.fp );
        g_cmthack.fp = NULL;
        return;
    };

    /* Nacteno OK */
    cmthack_result ( LOADRET_OK );
}


/* 
 * 
 * Pozadavek na precteni tela souboru
 * 
 */
void cmthack_read_body ( void ) {

    unsigned reg_hl;
    unsigned reg_bc;

    /* Mame otevren nejaky MZF? */
    if ( !g_cmthack.fp ) {
        /* Zruseno: nastavit Err + Break */
        cmthack_result ( LOADRET_BREAK );
        return;
    };

    /* precteme prvnich regBC bajtu z MZF souboru a ulozime je do RAM na adresu z regHL */
    reg_hl = z80ex_get_reg ( g_mz800.cpu, regHL );
    reg_bc = z80ex_get_reg ( g_mz800.cpu, regBC );
    
    while ( reg_bc ) {

        unsigned length = 0xffff - reg_hl;

        if ( length > reg_bc ) {
            length = reg_bc;
        };

        if ( length != ui_utils_fread ( &g_memory.RAM [ reg_hl ], 1, length, g_cmthack.fp ) ) {
            /* Vadny soubor: nastavit Err + Checksum */
            cmthack_result ( LOADRET_ERROR );
            fclose ( g_cmthack.fp );
            g_cmthack.fp = NULL;
            return;
        };

        reg_hl = 0x0000;
        reg_bc -= length;
    };

    /* Nacteno OK */
    cmthack_result ( LOADRET_OK );
    fclose ( g_cmthack.fp );
    g_cmthack.fp = NULL;
}

