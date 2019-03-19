/* 
 * File:   gdg_mz1500.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 3. března 2019, 14:36
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


#ifndef GDG_MZ1500_H
#define GDG_MZ1500_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ1500

#include <stdint.h>

#include "gdg.h"
#include "video.h"

    //#define GDGCLK_BASE                 14318180
#define GDGCLK_BASE                 ( VIDEO_SCREEN_TICKS * 60 )   /* 262 * 912 * 60 = 14 336 640 */
#define GDGCLK2CPU_DIVIDER          4

#define GDG_MZ1500_DMODE_BITPOS     0
#define GDG_MZ1500_PMODE_BITPOS     1
    
#define GDG_MZ1500_DMODE_BIT        ( 1 << GDG_MZ1500_DMODE_BITPOS )  /* Display Mode */
#define GDG_MZ1500_PMODE_BIT        ( 1 << GDG_MZ1500_PMODE_BITPOS )  /* Priority Mode */

#define GDG_MZ1500_FLAG_DMODE_700   ( 0 << GDG_MZ1500_DMODE_BITPOS )
#define GDG_MZ1500_FLAG_DMODE_1500  ( 1 << GDG_MZ1500_DMODE_BITPOS )

#define GDG_MZ1500_FLAG_PMODE_BPF   ( 0 << GDG_MZ1500_PMODE_BITPOS )
#define GDG_MZ1500_FLAG_PMODE_BFP   ( 1 << GDG_MZ1500_PMODE_BITPOS )


    typedef struct st_GDG_MZ1500 {
        int dmd; // 0. bit zapina MZ1500, 1. bit priorita vykreslovani
        int mode1500_color[8]; // obsahuje kod barvy 0 - 7
    } st_GDG_MZ1500;

    extern st_GDG_MZ1500 g_gdg_mz1500;

#define GDG_MZ1500_TEST_MODE1500 ( ( g_gdg_mz1500.dmd & GDG_MZ1500_DMODE_BIT ) == GDG_MZ1500_FLAG_DMODE_1500 )
#define GDG_MZ1500_TEST_MODE700 ( !GDG_MZ1500_TEST_MODE1500 )

#define GDG_MZ1500_TEST_PMODE_BFP ( ( g_gdg_mz1500.dmd & GDG_MZ1500_PMODE_BIT ) == GDG_MZ1500_FLAG_PMODE_BFP )
#define GDG_MZ1500_TEST_PMODE_BPF ( !GDG_MZ1500_TEST_PMODEBFP )

    extern void gdg_mz1500_init ( void );
    extern void gdg_mz1500_reset ( void );

    extern Z80EX_BYTE gdg_mz1500_read_dmd_status ( void );
    extern void gdg_mz1500_write_byte ( unsigned addr, Z80EX_BYTE value );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
    extern void gdg_mz1500_write_byte_raw ( unsigned addr, Z80EX_BYTE value );
#endif
    
#endif


#ifdef __cplusplus
}
#endif

#endif /* GDG_MZ1500_H */

