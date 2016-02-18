/* 
 * File:   ui_qdisk.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. února 2016, 21:31
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
#include <gtk-3.0/gtk/gtkwidget.h>
#include <gtk-2.0/gtk/gtkwidget.h>

#include "ui_main.h"
#include "ui_qdisk.h"

#include "qdisk/qdisk.h"


void ui_qdisk_set_path ( char *path ) {
    if ( path [ 0 ] != 0x00 ) {
        if ( g_qdisk.type == QDISK_TYPE_IMAGE ) {
            gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_drive" ), "_QD Drive (image)" );
        } else {
            gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_drive" ), "_QD Drive (directory)" );
        };
        gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_path" ), path );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_qdisk_umount" ), TRUE );
    } else {
        gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_drive" ), "_QD Drive (Empty)" );
        gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_path" ), "Empty" );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_qdisk_umount" ), FALSE );
    };
}


void ui_qdisk_menu_update ( void ) {
    
    LOCK_UICALLBACKS ( );
    if ( g_qdisk.connected == QDISK_DISCONNECTED ) {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_not_connected" ), TRUE );
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_std" ), FALSE );
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_virtual" ), FALSE );

        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_qdisk_drive" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_qdisk_write_protected" ), FALSE );
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_write_protected" ), FALSE );

        ui_qdisk_set_path ( "" );

    } else {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_not_connected" ), FALSE );
        if ( g_qdisk.type == QDISK_TYPE_IMAGE ) {
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_std" ), TRUE );
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_virtual" ), FALSE );
        } else {
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_std" ), FALSE );
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_virtual" ), TRUE );
        }

        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_qdisk_drive" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_qdisk_write_protected" ), TRUE );

        if ( g_qdisk.status & QDSTS_IMG_READONLY ) {
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_write_protected" ), TRUE );
        } else {
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_qdisk_write_protected" ), FALSE );
        };

        if ( g_qdisk.type == QDISK_TYPE_IMAGE ) {
            gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_mount" ), "_Mount Image..." );
        } else {
            gtk_menu_item_set_label ( ui_get_menu_item ( "menuitem_qdisk_mount" ), "_Mount Directory..." );
        };

    };

    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT void on_qdisk_changed ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    qdisk_close ( );

    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_qdisk_not_connected" ) ) ) {
        g_qdisk.connected = QDISK_DISCONNECTED;
    } else {

        g_qdisk.connected = QDISK_CONNECTED;

        if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_qdisk_std" ) ) ) {
            g_qdisk.type = QDISK_TYPE_IMAGE;
        } else {
            g_qdisk.type = QDISK_TYPE_VIRTUAL;
        };

        qdisk_open ( );

    };
    ui_qdisk_menu_update ( );
}


G_MODULE_EXPORT void on_menuitem_qdisk_mount ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    qdisk_mount ( );
}


G_MODULE_EXPORT void on_menuitem_qdisk_umount ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    qdisk_umount ( );
}


G_MODULE_EXPORT void on_menuitem_qdisk_write_protected ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    qdisk_set_write_protected ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_qdisk_write_protected" ) ) );
}
