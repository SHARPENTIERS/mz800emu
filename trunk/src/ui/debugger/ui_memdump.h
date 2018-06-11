/* 
 * File:   ui_memdump.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 26. října 2015, 9:46
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

#ifndef UI_MEMDUMP_H
#define	UI_MEMDUMP_H

#ifdef	__cplusplus
extern "C" {
#endif

    extern void ui_memdump_show_hide_window ( void );
    extern void ui_memdump_show_window ( void );
    extern void ui_memdump_memsave_window_show ( void );
    extern void ui_memdump_memload_select_file ( void );

#ifdef	__cplusplus
}
#endif

#endif	/* UI_MEMDUMP_H */

