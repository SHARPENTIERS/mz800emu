/* 
 * File:   hwscroll.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 19:45
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

#ifndef HWSCROLL_H
#define HWSCROLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"


    typedef struct st_HWSCROLL {
        int regSSA;
        int regSEA;
        int regSW;
        int regSOF;
        int enabled;

    } st_HWSCROLL;

    extern st_HWSCROLL g_hwscroll;

    extern void hwscroll_init ( void );
    extern void hwscroll_reset ( void );
    extern void hwscroll_set_reg ( int addr, Z80EX_BYTE value );
    extern unsigned hwscroll_shift_addr ( unsigned addr );

#ifdef __cplusplus
}
#endif

#endif /* HWSCROLL_H */

