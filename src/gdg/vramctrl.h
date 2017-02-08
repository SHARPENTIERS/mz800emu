/* 
 * File:   vramctrl.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 19:12
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

#ifndef VRAMCTRL_H
#define VRAMCTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"


    typedef enum {
        GDG_WF_MODE_SINGLE = 0,
        GDG_WF_MODE_EXOR,
        GDG_WF_MODE_OR,
        GDG_WF_MODE_RESET,
        GDG_WF_MODE_REPLACE,
        GDG_WF_MODE_PSET = 6
    } WFR_MODE;


    typedef struct st_VRAMCTRL {
        unsigned regWF_PLANE;
        WFR_MODE regWF_MODE;

        unsigned regRF_PLANE;
        unsigned regRF_SEARCH;
        unsigned regWFRF_VBANK;
        unsigned mz700_wr_latch_is_used;
    } st_VRAMCTRL;

    extern st_VRAMCTRL g_vramctrl;


    extern void vramctrl_reset ( void );

    extern void vramctrl_set_reg ( int addr, Z80EX_BYTE value );

    extern Z80EX_BYTE vramctrl_mz700_memop_read_sync ( Z80EX_WORD addr );
    extern void vramctrl_mz700_memop_write_sync ( Z80EX_WORD addr, Z80EX_BYTE value );

    extern Z80EX_BYTE vramctrl_mz700_memop_read_byte ( Z80EX_WORD addr );
    extern void vramctrl_mz700_memop_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );

    extern Z80EX_BYTE vramctrl_mz800_memop_read_byte ( Z80EX_WORD addr );
    extern void vramctrl_mz800_memop_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );


#ifdef __cplusplus
}
#endif

#endif /* VRAMCTRL_H */

