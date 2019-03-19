/* 
 * File:   hwscroll_mz800.h
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

#ifndef HWSCROLL_MZ800_H
#define HWSCROLL_MZ800_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ800

#include "z80ex/include/z80ex.h"


    typedef struct st_HWSCROLL_MZ800 {
        int regSSA;
        int regSEA;
        int regSW;
        int regSOF;
        int enabled;

    } st_HWSCROLL_MZ800;

    extern st_HWSCROLL_MZ800 g_hwscroll_mz800;

    extern void hwscroll_mz800_init ( void );
    extern void hwscroll_mz800_reset ( void );
    extern void hwscroll_mz800_set_reg ( int addr, Z80EX_BYTE value );

    extern int hwscroll_mz800_get_ssa ( void );
    extern int hwscroll_mz800_get_sea ( void );
    extern int hwscroll_mz800_get_sw ( void );
    extern int hwscroll_mz800_get_sof ( void );

    extern void hwscroll_mz800_set_ssa ( int value );
    extern void hwscroll_mz800_set_sea ( int value );
    extern void hwscroll_mz800_set_sw ( int value );
    extern void hwscroll_mz800_set_sof ( int value );

#if 1
#define TEST_HWSCRL_MZ800_ENABLED (g_hwscroll_mz800.enabled)
#define TEST_HWSCRL_MZ800_ADDR_IN_SCRL_AREA(addr) ( ( addr >= g_hwscroll_mz800.regSSA ) && ( addr < g_hwscroll_mz800.regSEA ) )
#define TEST_HWSCRL_MZ800_ADDR_IN_SCRL_AREA_UPDOWN(addr) ( addr >= ( g_hwscroll_mz800.regSEA - g_hwscroll_mz800.regSOF ) )
#define HWSCRL_MZ800_SHIFT_UP(addr) ( addr + g_hwscroll_mz800.regSOF )
#define HWSCRL_MZ800_SHIFT_DOWN(addr) ( addr + g_hwscroll_mz800.regSOF - g_hwscroll_mz800.regSW )
#define HWSCRL_MZ800_SHIFT_UPDOWN(addr) ( ( TEST_HWSCRL_MZ800_ADDR_IN_SCRL_AREA_UPDOWN(addr) ) ? HWSCRL_MZ800_SHIFT_DOWN(addr) : HWSCRL_MZ800_SHIFT_UP(addr) )

    //#define hwscroll_shift_addr_new(addr) ( ( TEST_HWSCRL_ENABLED && TEST_HWSCRL_ADDR_IN_SCRL_AREA(addr) ) ? HWSCRL_SHIFT_UPDOWN(addr) : addr )
#endif

#if 1


    static inline unsigned hwscroll_mz800_shift_addr ( unsigned addr ) {

        if ( TEST_HWSCRL_MZ800_ENABLED ) {

            /* nachazime se v oblasti, ktera ma byt scrollovana? */
            if ( TEST_HWSCRL_MZ800_ADDR_IN_SCRL_AREA ( addr ) ) {

                if ( TEST_HWSCRL_MZ800_ADDR_IN_SCRL_AREA_UPDOWN ( addr ) ) {
                    return HWSCRL_MZ800_SHIFT_DOWN ( addr );
                };
                return HWSCRL_MZ800_SHIFT_UP ( addr );
            };
        };

        return addr;
    }
#endif

#if 0
    extern unsigned hwscroll_mz800_shift_addr ( unsigned addr );
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* HWSCROLL_MZ800_H */

