/* 
 * File:   ui_dbg_color_selection_dialog.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. března 2019, 9:56
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


#ifndef UI_DBG_COLOR_SELECTION_DIALOG_H
#define UI_DBG_COLOR_SELECTION_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

    extern int ui_dbg_color_selection_dialog ( const char *title, GdkRGBA *rgba, int count, GtkWindow *parent );

#ifdef __cplusplus
}
#endif

#endif /* UI_DBG_COLOR_SELECTION_DIALOG_H */

