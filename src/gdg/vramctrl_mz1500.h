/* 
 * File:   vramctrl_mz1500.h
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

#ifndef VRAMCTRL_MZ1500_H
#define VRAMCTRL_MZ1500_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ1500
#include "z80ex/include/z80ex.h"


    typedef struct st_VRAMCTRL_MZ1500 {
        unsigned wr_latch_is_used;
    } st_VRAMCTRL_MZ1500;

    extern st_VRAMCTRL_MZ1500 g_vramctrl_mz1500;


    extern void vramctrl_mz1500_reset ( void );

#include "gdg.h"
#include "memory/memory.h"

    /*******************************************************************************
     *
     * VRAM kontroler pro rezimy MZ-1500
     * 
     ******************************************************************************/

    /**
     * Cteni z MZ-1500 VRAM bez ohledu na synchronizaci
     * 
     * @param addr
     * @return 
     */
#define vramctrl_mz1500_read_byte( addr ) g_memory_VRAM [ addr ]


    /**
     * Cteni z MZ-1500 VRAM s ohledem na synchronizaci
     * 
     * @param addr
     * @return 
     */
    static inline Z80EX_BYTE vramctrl_mz1500_read_byte_sync ( Z80EX_WORD addr ) {
        mz800_sync_insideop_mreq ( );
        if ( GDG_SIGNAL_HBLNK ) {
            mz800_sync_insideop_mreq_mz700_vramctrl ( );
        };
        return vramctrl_mz1500_read_byte ( addr );
    }


    /**
     * Interni subrutina fyzickeho zapisu do MZ-1500 VRAM - generuje vsak zmenu ve screenu
     * 
     * @param addr
     * @param value
     */
#define vramctrl_mz1500_write_byte_internal( addr, value ) {\
        /* TODO: meli by jsme nastavovat jen pokud se zapisuje do VRAM, ktera je opravdu videt */\
        g_gdg.screen_changes = SCRSTS_THIS_IS_CHANGED;\
        g_memory_VRAM [ addr ] = value;\
}


    /**
     * Zapis do MZ-1500 VRAM s ohledem na synchronizaci
     * 
     * @param addr
     * @param value
     */
#define vramctrl_mz1500_write_byte_sync( addr, value ) {\
        mz800_sync_insideop_mreq ( );\
        if ( GDG_SIGNAL_HBLNK ) {\
            if ( g_vramctrl_mz1500.wr_latch_is_used++ != 0 ) {\
                mz800_sync_insideop_mreq_mz700_vramctrl ( );\
            };\
        };\
        vramctrl_mz1500_write_byte_internal ( addr, value );\
}


    /**
     * Zapis do MZ-1500 VRAM bez synchronizace - generuje vsak zmenu ve screenu
     * 
     * @param addr
     * @param value
     */
#define vramctrl_mz1500_write_byte( addr, value ) vramctrl_mz1500_write_byte_internal ( addr, value )

#endif /* MZ1500EMU */

#ifdef __cplusplus
}
#endif

#endif /* VRAMCTRL_MZ1500_H */

