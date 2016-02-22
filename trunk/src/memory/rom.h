/* 
 * File:   rom.h
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

#ifndef ROM_H
#define	ROM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"


    extern const Z80EX_BYTE c_ROM_MZ700 [];
    extern const Z80EX_BYTE c_ROM_CGROM [];
    extern const Z80EX_BYTE c_ROM_MZ800 [];


    extern const Z80EX_BYTE c_ROM_JSS103_MZ700 [];
    extern const Z80EX_BYTE c_ROM_JSS103_CGROM [];
    extern const Z80EX_BYTE c_ROM_JSS103_MZ800 [];


    extern const Z80EX_BYTE c_ROM_JSS105C_MZ700 [];
    extern const Z80EX_BYTE c_ROM_JSS105C_CGROM [];
    extern const Z80EX_BYTE c_ROM_JSS105C_MZ800 [];


    extern const Z80EX_BYTE c_ROM_JSS106A_MZ700 [];
    extern const Z80EX_BYTE c_ROM_JSS106A_CGROM [];
    extern const Z80EX_BYTE c_ROM_JSS106A_MZ800 [];


    extern const Z80EX_BYTE c_ROM_WILLY_MZ700 [];

    extern const Z80EX_BYTE c_ROM_WILLY_en_CGROM [];
    extern const Z80EX_BYTE c_ROM_WILLY_en_MZ800 [];

    extern const Z80EX_BYTE c_ROM_WILLY_ge_CGROM [];
    extern const Z80EX_BYTE c_ROM_WILLY_ge_MZ800 [];

    extern const Z80EX_BYTE c_ROM_WILLY_jap_CGROM [];
    extern const Z80EX_BYTE c_ROM_WILLY_jap_MZ800 [];

    typedef enum en_ROMTYPE {
        ROMTYPE_STANDARD = 0,
        ROMTYPE_JSS103,
        ROMTYPE_JSS105C,
        ROMTYPE_JSS106A,
        ROMTYPE_WILLY_EN,
        ROMTYPE_WILLY_GE,
        ROMTYPE_WILLY_JAP,
        ROMTYPE_USER_DEFINED,
    } en_ROMTYPE;

    typedef struct st_ROM {
        en_ROMTYPE type;
        const Z80EX_BYTE *mz700rom;
        const Z80EX_BYTE *cgrom;
        const Z80EX_BYTE *mz800rom;
    } st_ROM;

    extern st_ROM g_rom;

    extern void rom_init ( void );
    extern void rom_reinstall ( en_ROMTYPE romtype );

#ifdef	__cplusplus
}
#endif

#endif	/* ROM_H */

