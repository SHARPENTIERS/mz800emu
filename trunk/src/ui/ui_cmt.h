/* 
 * File:   ui_cmt.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. srpna 2015, 19:41
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

#ifndef UI_CMT_H
#define	UI_CMT_H

#ifdef	__cplusplus
extern "C" {
#endif

    extern void ui_cmt_window_show_hide ( void );
    extern void ui_cmt_hack_menu_update ( void );
    extern void ui_cmt_window_update ( void );
    extern void ui_cmt_update_player ( void );
    extern void ui_cmt_set_filename ( char *filename );

#ifdef	__cplusplus
}
#endif

#endif	/* UI_CMT_H */

