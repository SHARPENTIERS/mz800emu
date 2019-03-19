/* 
 * File:   vramctrl_mz800.h
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

#ifndef VRAMCTRL_MZ800_H
#define VRAMCTRL_MZ800_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ800
#include "z80ex/include/z80ex.h"


    typedef enum {
        GDG_WF_MODE_SINGLE = 0,
        GDG_WF_MODE_EXOR,
        GDG_WF_MODE_OR,
        GDG_WF_MODE_RESET,
        GDG_WF_MODE_REPLACE,
        GDG_WF_MODE_PSET = 6
    } WFR_MODE;


    typedef struct st_VRAMCTRL_MZ800 {
        unsigned regWF_PLANE;
        WFR_MODE regWF_MODE;

        unsigned regRF_PLANE;
        unsigned regRF_SEARCH;
        unsigned regWFRF_VBANK;
        unsigned mode700_wr_latch_is_used;
    } st_VRAMCTRL_MZ800;

    extern st_VRAMCTRL_MZ800 g_vramctrl_mz800;


    extern void vramctrl_mz800_reset ( void );

    extern void vramctrl_mz800_set_wf_rf_reg ( int addr, Z80EX_BYTE value );

#if 0
    extern Z80EX_BYTE vramctrl_mz800_mode700_memop_read_byte_sync ( Z80EX_WORD addr );
    extern void vramctrl_mz800_mode700_memop_write_byte_sync ( Z80EX_WORD addr, Z80EX_BYTE value );

    extern Z80EX_BYTE vramctrl_mz800_mode700_memop_read_byte ( Z80EX_WORD addr );
    extern void vramctrl_mz800_mode700_memop_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );
#endif

    extern Z80EX_BYTE vramctrl_mz800_mode800_memop_read_byte_sync ( Z80EX_WORD addr );
    extern Z80EX_BYTE vramctrl_mz800_mode800_memop_read_byte ( Z80EX_WORD addr );
    extern void vramctrl_mz800_mode800_memop_write_byte ( Z80EX_WORD addr, Z80EX_BYTE value );

#include "gdg.h"
#include "memory/memory.h"
    /*******************************************************************************
     *
     * VRAM kontroler pro rezimy MZ-700
     * 
     ******************************************************************************/

    /**
     * Cteni z MZ-700 VRAM bez ohledu na synchronizaci
     * 
     * @param addr
     * @return 
     */
#define vramctrl_mz800_mode700_memop_read_byte( addr ) g_memory_VRAM [ addr ]


    /**
     * Cteni z MZ-700 VRAM s ohledem na synchronizaci
     * 
     * @param addr
     * @return 
     */
    static inline Z80EX_BYTE vramctrl_mz800_mode700_memop_read_byte_sync ( Z80EX_WORD addr ) {
        mz800_sync_insideop_mreq ( );
        if ( GDG_SIGNAL_HBLNK ) {
            mz800_sync_insideop_mreq_mz700_vramctrl ( );
        };
        return vramctrl_mz800_mode700_memop_read_byte ( addr );
    }


    /**
     * Interni subrutina fyzickeho zapisu do MZ-700 VRAM - generuje vsak zmenu ve screenu
     * 
     * @param addr
     * @param value
     */
#define vramctrl_mz800_mode700_memop_write_byte_internal( addr, value ) {\
        /* TODO: meli by jsme nastavovat jen pokud se zapisuje do CG-RAM, nebo do VRAM, ktera je opravdu videt */\
        g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;\
        g_memory_VRAM [ addr ] = value;\
}


    /**
     * Zapis do MZ-700 VRAM s ohledem na synchronizaci
     * 
     * @param addr
     * @param value
     */
#define vramctrl_mz800_mode700_memop_write_byte_sync( addr, value ) {\
        mz800_sync_insideop_mreq ( );\
        if ( GDG_SIGNAL_HBLNK ) {\
            if ( g_vramctrl_mz800.mode700_wr_latch_is_used++ != 0 ) {\
                mz800_sync_insideop_mreq_mz700_vramctrl ( );\
            };\
        };\
        vramctrl_mz800_mode700_memop_write_byte_internal ( addr, value );\
}


    /**
     * Zapis do MZ-700 VRAM bez synchronizace - generuje vsak zmenu ve screenu
     * 
     * @param addr
     * @param value
     */
#define vramctrl_mz800_mode700_memop_write_byte( addr, value ) vramctrl_mz800_mode700_memop_write_byte_internal ( addr, value )

#endif /* MZ800EMU */

#ifdef __cplusplus
}
#endif

#endif /* VRAMCTRL_MZ800_H */

