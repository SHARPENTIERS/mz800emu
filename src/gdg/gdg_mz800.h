/* 
 * File:   gdg_mz800.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 18:38
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

#ifndef GDG_MZ800_H
#define GDG_MZ800_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ800

#include <stdint.h>

#include "gdg.h"
#include "video.h"

    //#define GDGCLK_BASE                 17734475
#define GDGCLK_BASE                 ( VIDEO_SCREEN_TICKS * 50 )   /* 312 * 1136 * 50 = 17 721 600, snimek realneho sharpa netrva presne 20 ms, coz ovsem neumim dobre emulovat */
#define GDGCLK2CPU_DIVIDER          5

#define GDG_MZ800_DMD_FLAG_MODE700      ( 1 << 3 )
#define GDG_MZ800_DMD_FLAG_SCRW640      ( 1 << 2 )
#define GDG_MZ800_DMD_FLAG_HICOLOR      ( 1 << 1 )
#define GDG_MZ800_DMD_FLAG_VBANK        ( 1 << 0 )

#define GDG_MZ800_TEST_DMD_MODE700      ( g_gdg_mz800.regDMD & GDG_MZ800_DMD_FLAG_MODE700 )
#define GDG_MZ800_TEST_DMD_SCRW640      ( g_gdg_mz800.regDMD & GDG_MZ800_DMD_FLAG_SCRW640 )
#define GDG_MZ800_TEST_DMD_HICOLOR      ( g_gdg_mz800.regDMD & GDG_MZ800_DMD_FLAG_HICOLOR )
#define GDG_MZ800_TEST_DMD_VBANK        ( g_gdg_mz800.regDMD & GDG_MZ800_DMD_FLAG_VBANK )


    typedef struct st_GDG_MZ800 {
        unsigned regDMD; /* Display Mode register */
        unsigned regBOR; /* Border register */
        unsigned regPALGRP; /* Palette Group register */
        unsigned regPAL0; /* Palette0 register */
        unsigned regPAL1; /* Palette1 register */
        unsigned regPAL2; /* Palette2 register */
        unsigned regPAL3; /* Palette3 register */
    } st_GDG_MZ800;

    extern st_GDG_MZ800 g_gdg_mz800;

    extern struct st_GDGEVENT g_gdgevent_mz800[];

    extern void gdg_mz800_init ( void );
    extern void gdg_mz800_reset ( void );

    extern Z80EX_BYTE gdg_mz800_read_dmd_status ( void );
    extern void gdg_mz800_write_byte ( unsigned addr, Z80EX_BYTE value );


#endif /* MACHINE_EMU_MZ800 */


#ifdef __cplusplus
}
#endif

#endif /* GDG_MZ800_H */

