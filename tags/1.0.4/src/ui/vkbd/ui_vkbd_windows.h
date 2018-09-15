/* 
 * File:   ui_vkbd_windows.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 7. prosince 2017, 10:02
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


#ifndef UI_VKBD_WINDOWS_H
#define UI_VKBD_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef WINDOWS

#include "ui_vkbd_scancode.h"
#include <gtk/gtk.h>

    extern en_VKBD_SCANCODE ui_vkbd_windows_KeycodeToScancode ( guint16 hardware_keycode );

#endif


#ifdef __cplusplus
}
#endif

#endif /* UI_VKBD_WINDOWS_H */

