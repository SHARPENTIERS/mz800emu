/* 
 * File:   ui_ramdisk.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 10. srpna 2015, 20:44
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
#include "ui_ramdisk.h"

#include "ramdisk/ramdisk.h"


G_MODULE_EXPORT void on_ramdisk_not_connected ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_pezik_init ( RAMDISK_PEZIK_E8, RAMDISK_DISCONNECTED );
    ramdisk_pezik_init ( RAMDISK_PEZIK_68, RAMDISK_DISCONNECTED );
    ramdisk_std_disconnect ( );

    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_pezik_e8 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_ramdisk_pezik_e8" ) ) ) {
        ramdisk_pezik_init ( RAMDISK_PEZIK_E8, RAMDISK_CONNECTED );
    } else {
        ramdisk_pezik_init ( RAMDISK_PEZIK_E8, RAMDISK_DISCONNECTED );
    };

    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_pezik_68 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_ramdisk_pezik_68" ) ) ) {
        ramdisk_pezik_init ( RAMDISK_PEZIK_68, RAMDISK_CONNECTED );
    } else {
        ramdisk_pezik_init ( RAMDISK_PEZIK_68, RAMDISK_DISCONNECTED );
    };

    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_std ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_ramdisk_std" ) ) ) {
        ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, g_ramdisk.std.size, g_ramdisk.std.filepath );
    } else {
        ramdisk_std_disconnect ( );
    };

    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_size_64 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, RAMDISK_SIZE_64, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_size_256 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, RAMDISK_SIZE_256, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_size_512 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, RAMDISK_SIZE_512, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_size_1mb ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, RAMDISK_SIZE_1M, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_size_16mb ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, g_ramdisk.std.type, RAMDISK_SIZE_16M, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_type_std ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, RAMDISK_TYPE_STD, g_ramdisk.std.size, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_type_sram ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, RAMDISK_TYPE_SRAM, g_ramdisk.std.size, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_type_rom ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    ramdisk_std_init ( RAMDISK_CONNECTED, RAMDISK_TYPE_ROM, g_ramdisk.std.size, g_ramdisk.std.filepath );
    ui_ramdisk_update_menu ( );
}


G_MODULE_EXPORT void on_ramdisk_file ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    
    ramdisk_std_open_file ();
    ui_ramdisk_update_menu ( );
}


void ui_ramdisk_update_menu ( void ) {

    LOCK_UICALLBACKS ( );

    unsigned ramdisk_connected = 0;

    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_not_connected" ), TRUE );

    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_pezik_e8" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_pezik_68" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_std" ), FALSE );

    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_pezik_e8" ), TRUE );
    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_pezik_68" ), TRUE );
    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_std" ), TRUE );

    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_size" ), FALSE );
    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_type" ), FALSE );
    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_file" ), FALSE );


    if ( g_ramdisk.pezik[RAMDISK_PEZIK_E8].connected ) {
        ramdisk_connected = 1;
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_pezik_e8" ), TRUE );

        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_std" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_std" ), FALSE );
    };

    if ( g_ramdisk.pezik[RAMDISK_PEZIK_68].connected ) {
        ramdisk_connected = 1;
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_pezik_68" ), TRUE );
    };

    if ( g_ramdisk.std.connected ) {
        ramdisk_connected = 1;
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_std" ), TRUE );

        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_pezik_e8" ), FALSE );

        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_size" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_type" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_file" ), TRUE );

        switch ( g_ramdisk.std.size ) {
            case RAMDISK_SIZE_64:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_size_64" ), TRUE );
                break;
            case RAMDISK_SIZE_256:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_size_256" ), TRUE );
                break;
            case RAMDISK_SIZE_512:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_size_512" ), TRUE );
                break;
            case RAMDISK_SIZE_1M:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_size_1mb" ), TRUE );
                break;
            case RAMDISK_SIZE_16M:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_size_16mb" ), TRUE );
                break;
        };
        
        switch ( g_ramdisk.std.type ) {
            case RAMDISK_TYPE_STD:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_type_std" ), TRUE );
                gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_ramdisk_file" ), FALSE );
                break;
            case RAMDISK_TYPE_SRAM:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_type_sram" ), TRUE );
                break;
            case RAMDISK_TYPE_ROM:
                gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_type_rom" ), TRUE );
                break;
        };
    };

    if ( ramdisk_connected ) {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_ramdisk_not_connected" ), FALSE );
    }

    UNLOCK_UICALLBACKS ( );
}
