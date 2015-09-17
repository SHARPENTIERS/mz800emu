/* 
 * File:   display.h
 * Author: chaky
 *
 * Created on 14. června 2015, 9:44
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

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

    /* celkovy pocet emulovanych barev */
#define DISPLAY_MZCOLORS   16
    
    typedef enum en_DISPLAY_COLOR_SCHEMA {
        DISPLAY_NORMAL = 0,
        DISPLAY_GRAYSCALE,
        DISPLAY_GREEN,
        DISPLAY_COLORS_COUNT
    } en_DISPLAY_COLOR_SCHEMA;
    
    extern void display_init ( void );
    extern uint32_t* display_get_default_color_schema ( void );
    extern void display_set_colors ( en_DISPLAY_COLOR_SCHEMA color_schema );
    extern unsigned display_get_window_color_schema ( void );

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

