/* 
 * File:   ui_display.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 13. srpna 2015, 10:05
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

#include <gtk/gtk.h>

#include "ui_main.h"

#include "display.h"
#include "iface_sdl/iface_sdl.h"


G_MODULE_EXPORT void on_menuitem_display_colors_normal ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    display_set_colors ( DISPLAY_NORMAL );
}


G_MODULE_EXPORT void on_menuitem_display_colors_grayscale ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    display_set_colors ( DISPLAY_GRAYSCALE );
}


G_MODULE_EXPORT void on_menuitem_display_colors_green ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    display_set_colors ( DISPLAY_GREEN );
}


G_MODULE_EXPORT void on_menuitem_display_window_size_normal_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    iface_sdl_set_window_size ( 1 );
}


G_MODULE_EXPORT void on_menuitem_display_window_size_bigger_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    iface_sdl_set_window_size ( 1.5 );
}


void ui_display_update_menu ( void ) {
    
    LOCK_UICALLBACKS ( );
    
    switch ( display_get_window_color_schema() ) {
        case DISPLAY_NORMAL:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_display_colors_normal" ), TRUE );
            break;
        case DISPLAY_GRAYSCALE:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_display_colors_grayscale" ), TRUE );
            break;
        case DISPLAY_GREEN:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_display_colors_green" ), TRUE );
            break;
    };

    UNLOCK_UICALLBACKS ( );
}



