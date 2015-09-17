/* 
 * File:   ui_cmt.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. srpna 2015, 19:40
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
#include <stdio.h>

#include "ui_cmt.h"
#include "ui_main.h"

#include "cmt/cmt.h"
#include "cmt/cmt_hack.h"



G_MODULE_EXPORT gboolean on_cmt_window_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {
    if ( event->keyval == GDK_KEY_Escape ) {
        ui_cmt_window_show_hide ( );
        return TRUE;
    };
    return FALSE;
}


G_MODULE_EXPORT void on_menuitem_cmt_hack ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_cmt_hack" ) ) ) {
        cmthack_load_rom_patch ( 1 );
    } else {
        cmthack_load_rom_patch ( 0 );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_1200 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    g_cmt.speed = CMT_SPEED_1200;

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), g_cmt.speed );
    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_2400 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    g_cmt.speed = CMT_SPEED_2400;

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), g_cmt.speed );
    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_3600 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    g_cmt.speed = CMT_SPEED_3600;

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), g_cmt.speed );
    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT void on_menuitem_cmt_open_and_play ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( RET_CMT_OK == cmt_open ( ) ) {
        cmt_play ( );
    };
}


void ui_cmt_window_show_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( gtk_widget_get_visible ( window ) ) {
        gtk_widget_hide ( window );
    } else {
        gtk_widget_show ( window );
        ui_cmt_window_update ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_show_window ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    ui_cmt_window_show_hide ( );
}


void ui_cmt_hack_menu_update ( void ) {
    LOCK_UICALLBACKS ( );
    if ( g_cmthack.load_patch_installed ) {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_hack" ), TRUE );
    } else {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_hack" ), FALSE );
    };
    UNLOCK_UICALLBACKS ( );
}


void ui_cmt_speed_menu_update ( void ) {

    LOCK_UICALLBACKS ( );

    if ( ( g_cmt.state > CMT_PLAY_NONE ) && ( g_cmt.state < CMT_PLAY_DONE ) ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_cmt_speed" ), FALSE );
    } else {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_cmt_speed" ), TRUE );
    };

    switch ( g_cmt.speed ) {
        case CMT_SPEED_1200:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_1200" ), TRUE );
            break;
        case CMT_SPEED_2400:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_2400" ), TRUE );
            break;
        case CMT_SPEED_3600:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_3600" ), TRUE );
            break;
    };

    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT gboolean on_cmt_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer data ) {
    (void) widget;
    (void) event;
    (void) data;

    ui_cmt_window_show_hide ( );
    return TRUE;
}


G_MODULE_EXPORT void on_cmt_hide_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    ui_cmt_window_show_hide ( );
}


G_MODULE_EXPORT void on_cmt_stop_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_stop ( );
}


G_MODULE_EXPORT void on_cmt_record_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __FUNCTION__ );
}


G_MODULE_EXPORT void on_cmt_play_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_play ( );
}


G_MODULE_EXPORT void on_cmt_pause_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __FUNCTION__ );
}


G_MODULE_EXPORT void on_cmt_open_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_open ( );
}


G_MODULE_EXPORT void on_cmt_eject_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_eject ( );
}

G_MODULE_EXPORT void on_cmt_speed_comboboxtext_changed ( GtkComboBox *combobox, gpointer data ) {
    //(void) combobox;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

    g_cmt.speed = gtk_combo_box_get_active ( combobox );
    ui_cmt_speed_menu_update ( );
}


void ui_cmt_window_update ( void ) {

    if ( g_cmt.fh == NULL ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );

        gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), "Empty" );

        gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfsize_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfexec_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfstart_label" ), "--" );
        gtk_label_set_markup ( ui_get_label ( "cmt_play_button_label" ), "Play" );
    } else {

        char buff [ 100 ];

        sprintf ( buff, "%02x", g_cmt.mzf_filetype );
        gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), buff );

        gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), g_cmt.mzf_filename );

        sprintf ( buff, "0x%04x", g_cmt.mzf_size );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfsize_label" ), buff );

        sprintf ( buff, "0x%04x", g_cmt.mzf_exec );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfexec_label" ), buff );

        sprintf ( buff, "0x%04x", g_cmt.mzf_start );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfstart_label" ), buff );

        if ( CMT_PLAY_NONE == g_cmt.state ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), TRUE );
            gtk_label_set_markup ( ui_get_label ( "cmt_play_button_label" ), "Play" );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );
            gtk_label_set_markup ( ui_get_label ( "cmt_play_button_label" ), "<b>Play</b>" );
        };
    };

    ui_cmt_update_player ( );

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), g_cmt.speed );
    UNLOCK_UICALLBACKS ( );

    ui_cmt_speed_menu_update ( );
}


void ui_cmt_update_player ( void ) {

    char buff [ 100 ];
    gdouble fraction;

    sprintf ( buff, "<b><span font='50'>%02d:%02d</span></b>", ( g_cmt.play_time / 60 ), ( g_cmt.play_time % 60 ) );
    gtk_label_set_markup ( ui_get_label ( "cmt_time_label" ), buff );

    if ( ( g_cmt.file_bits == 0 ) || ( g_cmt.file_bits_elapsed <= 0 ) ) {
        fraction = 0;
    } else {
        fraction = (gdouble) g_cmt.file_bits_elapsed / (gdouble) g_cmt.file_bits;
    };
#if 0
    printf ( "file_bits_elapsed: %u, file_bits: %u, progress: %0.5f\n", g_cmt.file_bits_elapsed, g_cmt.file_bits, fraction );
#endif
    gtk_progress_bar_set_fraction ( ui_get_progress_bar ( "cmt_progressbar" ), fraction );
}


void ui_cmt_set_filename ( char *filename ) {
    gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), filename );
}
