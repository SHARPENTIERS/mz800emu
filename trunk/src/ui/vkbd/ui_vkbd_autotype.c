/* 
 * File:   ui_vkbd_autotype.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. září 2019, 18:45
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

#include <stdlib.h>
#include <string.h>
#include <gmodule.h>

#include "main.h"
#include "mz800.h"
#include "../ui_main.h"
#include "pio8255/pio8255.h"

static char *g_vkbd_autotype_text = NULL;
static gboolean g_lock_togglebutton = FALSE;
static gboolean g_lock_spinbutton = FALSE;


void ui_vkbd_autotype_show_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "window_vkbd_autotype" );
    if ( gtk_widget_get_visible ( window ) ) {
        gtk_widget_hide ( window );
    } else {
        g_lock_spinbutton = TRUE;
        gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( ui_get_widget ( "vkbd_key_down_ms_spinbutton" ) ), g_pio8255.vkbd_autotype_kd_ms );
        gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( ui_get_widget ( "vkbd_key_up_ms_spinbutton" ) ), g_pio8255.vkbd_autotype_ku_ms );
        g_lock_spinbutton = FALSE;
        gtk_widget_show ( window );
    };
}


void ui_vkbd_autotype_deactivate ( void ) {
    g_lock_togglebutton = TRUE;
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "vkbd_autotype_active_togglebutton" ) ), FALSE );
    g_lock_togglebutton = FALSE;
    if ( g_vkbd_autotype_text ) {
        g_free ( g_vkbd_autotype_text );
        g_vkbd_autotype_text = NULL;
    };
    pio8255_set_autotype ( NULL );
    gtk_text_view_set_editable ( GTK_TEXT_VIEW ( ui_get_widget ( "vkbd_autotype_textview" ) ), TRUE );
}


G_MODULE_EXPORT void on_vkbd_key_down_ms_spinbutton_value_changed ( GtkSpinButton *spin_button, gpointer user_data ) {
    if ( g_lock_spinbutton ) return;
    pio8255_autotype_set_key_down_ms ( gtk_spin_button_get_value ( spin_button ) );
}


G_MODULE_EXPORT void on_vkbd_key_up_ms_spinbutton_value_changed ( GtkSpinButton *spin_button, gpointer user_data ) {
    if ( g_lock_spinbutton ) return;
    pio8255_autotype_set_key_up_ms ( gtk_spin_button_get_value ( spin_button ) );
}


G_MODULE_EXPORT void on_vkbd_autotype_active_togglebutton_toggled ( GtkToggleButton *togglebutton, gpointer data ) {
    (void) data;

    if ( g_lock_togglebutton ) return;

    if ( !gtk_toggle_button_get_active ( togglebutton ) ) {
        ui_vkbd_autotype_deactivate ( );
        return;
    };

    GtkWidget *view = ui_get_widget ( "vkbd_autotype_textview" );
    GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_start_iter ( buffer, &start );
    gtk_text_buffer_get_end_iter ( buffer, &end );
    g_vkbd_autotype_text = gtk_text_buffer_get_text ( buffer, &start, &end, FALSE );
    if ( !strlen ( g_vkbd_autotype_text ) ) {
        gtk_toggle_button_set_active ( togglebutton, FALSE );
    } else {
        pio8255_set_autotype ( g_vkbd_autotype_text );
        gtk_text_view_set_editable ( GTK_TEXT_VIEW ( ui_get_widget ( "vkbd_autotype_textview" ) ), FALSE );
    };
}


G_MODULE_EXPORT gboolean on_window_vkbd_autotype_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    gtk_widget_hide ( widget );
    return TRUE;
}


G_MODULE_EXPORT void on_vkbd_autotype_close_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;
    ui_vkbd_autotype_show_hide ( );
}


G_MODULE_EXPORT void on_menuitem_keyboard_virtual_autotype_show_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    ui_vkbd_autotype_show_hide ( );
}

