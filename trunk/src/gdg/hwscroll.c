/* 
 * File:   hwscroll.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 19:44
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
 *		reg. SSA (0x04cf):
 *			- pocatecni adresa HW scrollu, hodnoty 0x00 - 0x78, increment po 5
 *
 *		reg. SEA (0x05cf):
 *			- koncova adresa HW scrollu, hodnoty 0x05 - 0x7d, increment po 5
 *
 *		reg. SW (0x03cf):
 *			- sirka HW scrollu, hodnoty 0x05 - 0x7d, increment po 5
 *
 *		reg. SOF1 (0x01cf) - 8 bitu:
 *		reg. SOF2 (0x02cf) - 2 bity: 
 *			- 10 bitovy registr, kterym je urcen offset scrollu, hodnoty 0x00 - 0x3e8, increment po 5
 *			Pri hodnote 0x00 se neprovadi zadny scroll.
 *
 *
 *		Scroll je dostupny pouze v rezimech MZ800.
 *		Scroll je aktivni pokud jsou splneny vsechny naslednujici podminky:
 *
 *		SOF > 0x00
 *              SSA <= 0x78 ( << 6 = 0x1e00 )
 *		SEA >= 0x05 ( << 6 = 0x0140 )
 *              SW > SOF
 *		SEA > SSA 
 *              SW = SEA - SSA
 * 
 *      Update - tohle se zrejme nekontroluje:
 * 
 *              SOF <= 0x3e8 ( << 3 = 0x1f40 )
 *              SEA <= 0x7d ( << 6 = 0x1f40 )
 * 
 */

#include "z80ex/include/z80ex.h"

#include "hwscroll.h"
#include "gdg.h"
#include "framebuffer.h"

//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"

st_HWSCROLL g_hwscroll;


void hwscroll_init ( void ) {
    g_hwscroll.enabled = 0;
}


void hwscroll_reset ( void ) {
    g_hwscroll.regSSA = 0;
    g_hwscroll.regSEA = 125 << 6;
    g_hwscroll.regSW = 125 << 6;
    g_hwscroll.regSOF = 0;
    g_hwscroll.enabled = 0;
}


void hwscroll_set_reg ( int addr, Z80EX_BYTE value ) {

    //DBGPRINTF ( DBGINF, "addr: 0x%02x, value: 0x%02x, PC: 0x%04x\n", addr, value, z80ex_get_reg ( g_mz800.cpu, regPC ) );

    if ( !DMD_TEST_MZ700 ) {
        /* TODO: bylo by dobre volat update az ve chvili, kdy vime, ze se opravdu neco zmenilo */
        framebuffer_MZ800_screen_changed ( );
    };

    switch ( addr ) {

            /* nastaveni SOF_LO: 0x01cf */
        case 0x01:
            g_hwscroll.regSOF &= 0x03 << 11;
            g_hwscroll.regSOF |= ( value & 0xff ) << 3;
            break;


            /* nastaveni SOF_HI: 0x02cf */
        case 0x02:
            g_hwscroll.regSOF &= 0xff << 3;
            g_hwscroll.regSOF |= ( value & 0x03 ) << 11;
            break;


            /* nastaveni SW: 0x03cf */
        case 0x03:
            g_hwscroll.regSW = ( value & 0x7f ) << 6;
            break;


            /* nastaveni SSA: 0x04cf */
        case 0x04:
            g_hwscroll.regSSA = ( value & 0x7f ) << 6;
            break;


            /* nastaveni SEA: 0x05cf */
        case 0x05:
            g_hwscroll.regSEA = ( value & 0x7f ) << 6;
            break;
    };

    /* jsou splneny vsechny podminky pro scroll? */
    if ( ( g_hwscroll.regSOF > 0 ) &&
         //( g_hwscroll.regSOF <= 0x1f40 ) &&
         ( g_hwscroll.regSSA <= 0x1e00 ) &&
         ( g_hwscroll.regSEA >= 0x0140 ) &&
         //( g_hwscroll.regSEA <= 0x1f40 ) &&
         ( g_hwscroll.regSW > g_hwscroll.regSOF ) &&
         ( g_hwscroll.regSEA > g_hwscroll.regSSA ) &&
         ( g_hwscroll.regSW == ( g_hwscroll.regSEA - g_hwscroll.regSSA ) ) ) {

        g_hwscroll.enabled = 1;
        DBGPRINTF ( DBGINF, "SOF: 0x%04x, SSA: 0x%04x, SEA: 0x%04x, SW: 0x%04x, HW scroll is ENABLED\n", g_hwscroll.regSOF, g_hwscroll.regSSA, g_hwscroll.regSEA, g_hwscroll.regSW );

    } else {
        g_hwscroll.enabled = 0;
        DBGPRINTF ( DBGINF, "SOF: 0x%04x, SSA: 0x%04x, SEA: 0x%04x, SW: 0x%04x, HW scroll is DISABLED\n", g_hwscroll.regSOF, g_hwscroll.regSSA, g_hwscroll.regSEA, g_hwscroll.regSW );
#if ( DBGLEVEL & DBGINF )
        if ( !( g_hwscroll.regSOF > 0 ) ) {
            printf ( "\t! regSOF > 0\n" );
        };
        //        if ( ! ( g_hwscroll.regSOF <= 0x1f40 ) ) {
        //            printf ( "\t! regSOF <= 0x1f40\n" );
        //        };
        if ( !( g_hwscroll.regSSA <= 0x1e00 ) ) {
            printf ( "\t! regSSA <= 0x1e00\n" );
        };
        if ( !( g_hwscroll.regSEA >= 0x0140 ) ) {
            printf ( "\t! regSEA >= 0x0140\n" );
        };
        //        if ( ! ( g_hwscroll.regSEA <= 0x1f40 ) ) {
        //            printf ( "\t! regSEA <= 0x1f40\n" );
        //        };        
        if ( !( g_hwscroll.regSEA > g_hwscroll.regSSA ) ) {
            printf ( "\t! regSEA > regSSA\n" );
        };
        if ( !( g_hwscroll.regSW > g_hwscroll.regSOF ) ) {
            printf ( "\t! regSW > regSOF\n" );
        };
        if ( !( g_hwscroll.regSW == ( g_hwscroll.regSEA - g_hwscroll.regSSA ) ) ) {
            printf ( "\t! regSW == ( regSEA - regSSA )\n" );
        };
#endif
    };

}

#if 0


unsigned hwscroll_shift_addr ( unsigned addr ) {

    if ( g_hwscroll.enabled ) {

        /* nachazime se v oblasti, ktera ma byt scrollovana? */
        if ( ( addr >= g_hwscroll.regSSA ) && ( addr < g_hwscroll.regSEA ) ) {

            if ( addr >= ( g_hwscroll.regSEA - g_hwscroll.regSOF ) ) {
                return ( addr + g_hwscroll.regSOF - g_hwscroll.regSW );
            };
            return ( addr + g_hwscroll.regSOF );
        };
    };

    return addr;
}

#endif
