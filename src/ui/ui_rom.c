/* 
 * File:   ui_rom.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 22. února 2016, 20:07
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
#include "ui_rom.h"

#include "memory/rom.h"
#include "mz800.h"


void ui_rom_menu_update ( void ) {

    LOCK_UICALLBACKS ( );


    gboolean devel_visible = ( g_mz800.development_mode == DEVELMODE_YES ) ? TRUE : FALSE;

    gtk_widget_set_visible ( ui_get_widget ( "menuitem_rom_jss103" ), devel_visible );
    gtk_widget_set_visible ( ui_get_widget ( "menuitem_rom_jss105c" ), devel_visible );
    gtk_widget_set_visible ( ui_get_widget ( "separatormenuitem_rom_devel1" ), devel_visible );


    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_standard" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss103" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss105c" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss106a" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss108c" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_willy_en" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_willy_ge" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_willy_jap" ), FALSE );
    gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_user_defined" ), FALSE );

    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_rom_user_defined" ), FALSE );
    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_rom_user_settings" ), FALSE );


    switch ( g_rom.type ) {

        case ROMTYPE_STANDARD:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_standard" ), TRUE );
            break;

        case ROMTYPE_JSS103:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss103" ), TRUE );
            break;

        case ROMTYPE_JSS105C:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss105c" ), TRUE );
            break;

        case ROMTYPE_JSS106A:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss106a" ), TRUE );
            break;

        case ROMTYPE_JSS108C:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_jss108c" ), TRUE );
            break;

        case ROMTYPE_WILLY_EN:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_willy_en" ), TRUE );
            break;

        case ROMTYPE_WILLY_GE:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_willy_ge" ), TRUE );
            break;

        case ROMTYPE_WILLY_JAP:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_willy_jap" ), TRUE );
            break;

        default:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_rom_standard" ), TRUE );
            break;
    };

    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT void on_rom_changed ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_standard" ) ) ) {
        rom_reinstall ( ROMTYPE_STANDARD );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_jss103" ) ) ) {
        rom_reinstall ( ROMTYPE_JSS103 );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_jss105c" ) ) ) {
        rom_reinstall ( ROMTYPE_JSS105C );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_jss106a" ) ) ) {
        rom_reinstall ( ROMTYPE_JSS106A );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_jss108c" ) ) ) {
        rom_reinstall ( ROMTYPE_JSS108C );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_willy_en" ) ) ) {
        rom_reinstall ( ROMTYPE_WILLY_EN );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_willy_ge" ) ) ) {
        rom_reinstall ( ROMTYPE_WILLY_GE );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_willy_jap" ) ) ) {
        rom_reinstall ( ROMTYPE_WILLY_JAP );
    } else if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_rom_user_defined" ) ) ) {
        rom_reinstall ( ROMTYPE_USER_DEFINED );
    };

}


G_MODULE_EXPORT void on_menuitem_rom_user_settings_activate ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    printf ( "%s() - NOT IMPLEMENTED!\n", __func__ );
}


void ui_rom_settings_hide_window ( void ) {
    GtkWidget *window = ui_get_widget ( "dialog_rom_user_settings" );
    //ui_main_win_get_pos ( GTK_WINDOW ( window ), &g_uirom.main_pos );
    gtk_widget_hide ( window );
}


G_MODULE_EXPORT gboolean on_dialog_rom_user_settings_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_rom_settings_hide_window ( );
    return TRUE;
}


G_MODULE_EXPORT void on_rom_settings_changed ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    printf ( "%s() - NOT IMPLEMENTED!\n", __func__ );
}


G_MODULE_EXPORT void on_button_rom_user_settings_close_clicked ( GtkButton *button, gpointer user_data ) {
    ui_rom_settings_hide_window ( );
}


G_MODULE_EXPORT void on_button_rom_user_settings_ok_clicked ( GtkButton *button, gpointer user_data ) {
    printf ( "%s() - NOT IMPLEMENTED!\n", __func__ );
    ui_rom_settings_hide_window ( );
}
