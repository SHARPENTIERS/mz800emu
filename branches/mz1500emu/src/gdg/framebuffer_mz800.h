/* 
 * File:   framebuffer_mz800.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 5. července 2015, 8:43
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

#ifndef FRAMEBUFFER_MZ800_H
#define FRAMEBUFFER_MZ800_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ800
    extern void framebuffer_mz800_mode800_current_screen_row_fill ( unsigned last_pixel );
    extern void framebuffer_mz800_mode800_all_screen_rows_fill ( void );
    extern void framebuffer_mz800_mode800_screen_changed ( void );

    extern void framebuffer_mz800_border_current_row_fill ( void );
    extern void framebuffer_mz800_border_all_rows_fill ( void );
    extern void framebuffer_mz800_border_changed ( void );

#include "framebuffer_mz700.h"

#endif

#ifdef __cplusplus
}
#endif

#endif /* FRAMEBUFFER_MZ800_H */

