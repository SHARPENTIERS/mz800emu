/* 
 * File:   rom.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 22. února 2016, 18:30
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
#include <string.h>

#include "z80ex/include/z80ex.h"

#include "rom.h"
#include "memory.h"
#include "cmt/cmthack.h"

#include "cfgmain.h"

#include "ui/ui_rom.h"

st_ROM g_rom;


void rom_install_predefined ( const Z80EX_BYTE *mz700rom, const Z80EX_BYTE *cgrom, const Z80EX_BYTE *mz800rom ) {
    Z80EX_BYTE *ROM;

    ROM = g_memory.ROM;
    memcpy ( ROM, mz700rom, MEMORY_SIZE_ROM_MZ700 );
    g_rom.mz700rom = mz700rom;

    ROM += sizeof (Z80EX_BYTE ) * MEMORY_SIZE_ROM_CGROM;
    memcpy ( ROM, cgrom, MEMORY_SIZE_ROM_CGROM );
    g_rom.cgrom = cgrom;

    ROM += sizeof (Z80EX_BYTE ) * MEMORY_SIZE_ROM_CGROM;
    memcpy ( ROM, mz800rom, MEMORY_SIZE_ROM_MZ800 );
    g_rom.mz800rom = mz800rom;
}


void rom_install ( en_ROMTYPE romtype ) {

    switch ( romtype ) {

        case ROMTYPE_STANDARD:
            rom_install_predefined ( c_ROM_MZ700, c_ROM_CGROM, c_ROM_MZ800 );
            break;

        case ROMTYPE_JSS103:
            rom_install_predefined ( c_ROM_JSS103_MZ700, c_ROM_JSS103_CGROM, c_ROM_JSS103_MZ800 );
            break;

        case ROMTYPE_JSS105C:
            rom_install_predefined ( c_ROM_JSS105C_MZ700, c_ROM_JSS105C_CGROM, c_ROM_JSS105C_MZ800 );
            break;

        case ROMTYPE_JSS106A:
            rom_install_predefined ( c_ROM_JSS106A_MZ700, c_ROM_JSS106A_CGROM, c_ROM_JSS106A_MZ800 );
            break;

        case ROMTYPE_JSS108C:
            rom_install_predefined ( c_ROM_JSS108C_MZ700, c_ROM_JSS108C_CGROM, c_ROM_JSS108C_MZ800 );
            break;

        case ROMTYPE_WILLY_EN:
            rom_install_predefined ( c_ROM_WILLY_MZ700, c_ROM_WILLY_en_CGROM, c_ROM_WILLY_en_MZ800 );
            break;

        case ROMTYPE_WILLY_GE:
            rom_install_predefined ( c_ROM_WILLY_MZ700, c_ROM_WILLY_ge_CGROM, c_ROM_WILLY_ge_MZ800 );
            break;

        case ROMTYPE_WILLY_JAP:
            rom_install_predefined ( c_ROM_WILLY_MZ700, c_ROM_WILLY_jap_CGROM, c_ROM_WILLY_jap_MZ800 );
            break;

        default:
            printf ( "Unsupported ROM type! %d\n", romtype );
            rom_install_predefined ( c_ROM_MZ700, c_ROM_CGROM, c_ROM_MZ800 );
            romtype = ROMTYPE_STANDARD;
            break;

    };

    g_rom.type = romtype;
    ui_rom_menu_update ( );
}


void rom_reinstall ( en_ROMTYPE romtype ) {
    rom_install ( romtype );
    cmthack_reinstall_rom_patch ( );
}


void rom_init ( void ) {

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "ROM" );

    CFGELM *elm;

    if ( g_mz800.development_mode == DEVELMODE_YES ) {
        elm = cfgmodule_register_new_element ( cmod, "rom_type", CFGENTYPE_KEYWORD, ROMTYPE_STANDARD,
                ROMTYPE_STANDARD, "STANDARD",
                ROMTYPE_JSS103, "JSS103",
                ROMTYPE_JSS105C, "JSS105C",
                ROMTYPE_JSS106A, "JSS106A",
                ROMTYPE_JSS108C, "JSS108C",
                ROMTYPE_WILLY_EN, "WILLY_EN",
                ROMTYPE_WILLY_GE, "WILLY_GE",
                ROMTYPE_WILLY_JAP, "WILLY_JAP",
                ROMTYPE_USER_DEFINED, "USER_DEFINED",
                -1 );
    } else {
        elm = cfgmodule_register_new_element ( cmod, "rom_type", CFGENTYPE_KEYWORD, ROMTYPE_STANDARD,
                ROMTYPE_STANDARD, "STANDARD",
                ROMTYPE_JSS106A, "JSS106A",
                ROMTYPE_JSS108C, "JSS108C",
                ROMTYPE_WILLY_EN, "WILLY_EN",
                ROMTYPE_WILLY_GE, "WILLY_GE",
                ROMTYPE_WILLY_JAP, "WILLY_JAP",
                ROMTYPE_USER_DEFINED, "USER_DEFINED",
                -1 );
    };

    cfgelement_set_handlers ( elm, (void*) &g_rom.type, (void*) &g_rom.type );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    rom_install ( (en_ROMTYPE) cfgelement_get_keyword_value ( elm ) );
}
