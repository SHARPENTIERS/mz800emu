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
#include <math.h>

#include "ui_main.h"
#include "ui_cmt.h"

#include "cmt/cmt.h"
#include "cmt/cmt_extension.h"
#include "cmt/cmt_hack.h"

#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"

#include "libs/mztape/mztape.h"


typedef enum en_UICMT_SHOW {
    UICMT_SHOW_PLAY_TIME = 0,
    UICMT_SHOW_REMAINING_TIME,
} en_UICMT_SHOW;


typedef struct st_UICMT {
    st_UIWINPOS pos;
    en_UICMT_SHOW show_time;
} st_UICMT;

static st_UICMT g_uicmt;


void ui_cmt_init ( void ) {
    ui_main_setpos ( &g_uicmt.pos, -1, -1 );
    g_uicmt.show_time = UICMT_SHOW_REMAINING_TIME;
}


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


G_MODULE_EXPORT void on_menuitem_cmt_speed_1_1_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( MZTAPE_SPEED_1_1 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_2_1_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( MZTAPE_SPEED_2_1 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_7_3_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( MZTAPE_SPEED_7_3 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_8_3_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( MZTAPE_SPEED_8_3 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_3_1_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( MZTAPE_SPEED_3_1 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_open_and_play ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( EXIT_SUCCESS == cmt_open ( ) ) {
        cmt_play ( );
    };
}


void ui_cmt_window_show_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( gtk_widget_get_visible ( window ) ) {
        ui_main_win_get_pos ( GTK_WINDOW ( window ), &g_uicmt.pos );
        gtk_widget_hide ( window );
    } else {
        gtk_widget_show ( window );
        ui_main_win_move_to_pos ( GTK_WINDOW ( window ), &g_uicmt.pos );
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

    if ( !TEST_CMT_STOP ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_cmt_speed" ), FALSE );
    } else {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_cmt_speed" ), TRUE );
    };

    switch ( g_cmt.speed ) {
        case MZTAPE_SPEED_1_1:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_1_1" ), TRUE );
            break;
        case MZTAPE_SPEED_2_1:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_2_1" ), TRUE );
            break;
        case MZTAPE_SPEED_7_3:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_7_3" ), TRUE );
            break;
        case MZTAPE_SPEED_8_3:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_8_3" ), TRUE );
            break;
        case MZTAPE_SPEED_3_1:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_3_1" ), TRUE );
            break;
    };

    UNLOCK_UICALLBACKS ( );
}


void ui_cmt_polarity_menu_update ( void ) {
    if ( !TEST_CMT_STOP ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_dip_switch_cmt_inverted_polarity" ), FALSE );
    } else {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_dip_switch_cmt_inverted_polarity" ), TRUE );
    };
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


G_MODULE_EXPORT void on_cmt_record_togglebutton_toggled ( GtkToggleButton *togglebutton, gpointer data ) {
    (void) togglebutton;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_play_togglebutton_toggled ( GtkToggleButton *togglebutton, gpointer data ) {
    (void) togglebutton;
    (void) data;

    if ( ( gtk_toggle_button_get_active ( togglebutton ) ) && ( TEST_CMT_STOP ) ) {
        cmt_play ( );
    };
}


G_MODULE_EXPORT void on_cmt_pause_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
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


G_MODULE_EXPORT void on_cmt_previous_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_next_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_rewind_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_forward_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_filelist_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_speed_comboboxtext_changed ( GtkComboBox *combobox, gpointer data ) {
    //(void) combobox;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

    LOCK_UICALLBACKS ( );
    cmt_change_speed ( gtk_combo_box_get_active ( combobox ) );
    UNLOCK_UICALLBACKS ( );
}


void ui_cmt_window_update ( void ) {

    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( !gtk_widget_get_visible ( window ) ) {
        ui_cmt_speed_menu_update ( );
        ui_cmt_polarity_menu_update ( );
        return;
    };

    char buff [ 100 ];

    static en_CMT_FILETYPE cmt_filetype = CMT_FILETYPE_MZF;
    static int is_tape = 0;
    static int file_speed = 0;

    int play_file = 0;
    int total_files = 0;

    if ( TEST_CMT_FILLED ) {
        cmt_filetype = cmtext_cmtfile_get_filetype ( g_cmt.ext );
        is_tape = 0;
        file_speed = 0;
    };

    // Tape info
    if ( !is_tape ) {
        gtk_widget_hide ( ui_get_widget ( "cmt_tape_info_box" ) );
    } else {
        gtk_widget_show ( ui_get_widget ( "cmt_tape_info_box" ) );
        if ( total_files == 0 ) {
            gtk_label_set_text ( ui_get_label ( "cmt_play_file_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_total_files_label" ), "--" );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), FALSE );
        } else {
            snprintf ( buff, sizeof (buff ), "%d", play_file );
            gtk_label_set_text ( ui_get_label ( "cmt_play_file_label" ), buff );

            snprintf ( buff, sizeof (buff ), "%d", total_files );
            gtk_label_set_text ( ui_get_label ( "cmt_total_files_label" ), buff );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), TRUE );
        };
    };


    // MZF info
    if ( cmt_filetype != CMT_FILETYPE_MZF ) {
        gtk_widget_hide ( ui_get_widget ( "cmt_mzf_info_table" ) );
        gtk_widget_hide ( ui_get_widget ( "cmt_speed_label" ) );
        gtk_widget_hide ( ui_get_widget ( "cmt_speed_comboboxtext" ) );
    } else {
        gtk_widget_show ( ui_get_widget ( "cmt_mzf_info_table" ) );
        gtk_widget_show ( ui_get_widget ( "cmt_speed_label" ) );
        gtk_widget_show ( ui_get_widget ( "cmt_speed_comboboxtext" ) );

        if ( !TEST_CMT_FILLED ) {
            gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_mzfsize_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_mzfexec_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_mzfstart_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_file_speed_value_label" ), "--" );
        } else {

            st_MZF_HEADER *hdr = cmtext_cmtfile_get_mzfheader ( g_cmt.ext );
            g_assert ( hdr != NULL );
            snprintf ( buff, sizeof (buff ), "%02x", hdr->ftype );
            gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), buff );

            mzf_tools_get_fname ( hdr, (char*) &buff );
            gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), buff );

            snprintf ( buff, sizeof (buff ), "0x%04x", hdr->fsize );
            gtk_label_set_text ( ui_get_label ( "cmt_mzfsize_label" ), buff );

            snprintf ( buff, sizeof (buff ), "0x%04x", hdr->fexec );
            gtk_label_set_text ( ui_get_label ( "cmt_mzfexec_label" ), buff );

            snprintf ( buff, sizeof (buff ), "0x%04x", hdr->fstrt );
            gtk_label_set_text ( ui_get_label ( "cmt_mzfstart_label" ), buff );
        };

        if ( !file_speed ) {
            gtk_widget_hide ( ui_get_widget ( "cmt_file_speed_label" ) );
            gtk_widget_hide ( ui_get_widget ( "cmt_file_speed_value_label" ) );
        } else {
            gtk_widget_show ( ui_get_widget ( "cmt_file_speed_label" ) );
            gtk_widget_show ( ui_get_widget ( "cmt_file_speed_value_label" ) );

            snprintf ( buff, sizeof (buff ), "%d Bd", file_speed );
            gtk_label_set_text ( ui_get_label ( "cmt_file_speed_value_label" ), buff );
        };
    };

    // WAV info
    if ( cmt_filetype != CMT_FILETYPE_WAV ) {
        gtk_widget_hide ( ui_get_widget ( "cmt_stream_info_table" ) );
    } else {
        gtk_widget_show ( ui_get_widget ( "cmt_stream_info_table" ) );

        if ( !TEST_CMT_FILLED ) {
            gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "--" );
            gtk_label_set_text ( ui_get_label ( "cmt_stream_rate_label" ), "--" );
        } else {
            snprintf ( buff, sizeof (buff ), "%d Hz", cmtext_cmtfile_get_rate ( g_cmt.ext ) );
            gtk_label_set_text ( ui_get_label ( "cmt_stream_rate_label" ), buff );
            gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "WAV" );
        };
    };


    // progress bar
    if ( !TEST_CMT_FILLED ) {
        gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), "*** Empty ***" );
    } else {
        gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), cmtext_cmtfile_get_playname ( g_cmt.ext ) );
    };


    // Ovladaci prvky
    if ( !TEST_CMT_FILLED ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), FALSE );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_togglebutton" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_togglebutton" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_previous_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_next_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_rewind_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_forward_button" ), FALSE );

    } else {
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_previous_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_next_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_rewind_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_forward_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );

        if ( TEST_CMT_STOP ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_togglebutton" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_togglebutton" ), FALSE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), TRUE );

            gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "cmt_play_togglebutton" ) ), FALSE );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_togglebutton" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_togglebutton" ), FALSE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), TRUE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );

            if ( TEST_CMT_PLAY ) {
                LOCK_UICALLBACKS ( );
                gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "cmt_play_togglebutton" ) ), TRUE );
                UNLOCK_UICALLBACKS ( );
            };
        };
    };


    ui_cmt_update_player ( );

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), g_cmt.speed );
    UNLOCK_UICALLBACKS ( );

    ui_cmt_speed_menu_update ( );
    ui_cmt_polarity_menu_update ( );
}


void ui_cmt_update_player ( void ) {

    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( !gtk_widget_get_visible ( window ) ) return;

    gdouble total_time = 0;
    gdouble play_time = 0;
    gdouble fraction = 0;
    guint32 print_time;

    if ( TEST_CMT_FILLED ) {
        double scan_time = cmtext_cmtfile_get_scantime ( g_cmt.ext );
        uint64_t scans = cmtext_cmtfile_get_scans ( g_cmt.ext );
        total_time = ( scan_time * scans );
        if ( !TEST_CMT_STOP ) {
            play_time = cmt_get_playtime ( );
            fraction = ( play_time / total_time );
        };
    };

    char *minus = "";

    if ( g_uicmt.show_time == UICMT_SHOW_REMAINING_TIME ) {
        print_time = ( total_time - play_time );
        if ( ( total_time - play_time ) > print_time ) {
            print_time++;
        };
        minus = "-";
    } else {
        print_time = play_time;
    };

    char buff [ 100 ];

    sprintf ( buff, "<b><span font='50'>%s%02d:%02d</span></b>", minus, ( print_time / 60 ), ( print_time % 60 ) );
    gtk_label_set_markup ( ui_get_label ( "cmt_time_label" ), buff );

    gtk_progress_bar_set_fraction ( ui_get_progress_bar ( "cmt_progressbar" ), fraction );

}


void ui_cmt_set_filename ( char *filename ) {
    gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), filename );
}


void ui_cmt_show_time_changed ( void ) {
    if ( g_uicmt.show_time == UICMT_SHOW_REMAINING_TIME ) {
        g_uicmt.show_time = UICMT_SHOW_PLAY_TIME;
        gtk_label_set_text ( ui_get_label ( "cmt_time_info_label" ), "Play time:" );
    } else {
        g_uicmt.show_time = UICMT_SHOW_REMAINING_TIME;
        gtk_label_set_text ( ui_get_label ( "cmt_time_info_label" ), "Remaining time:" );
    };

    ui_cmt_update_player ( );
}


G_MODULE_EXPORT gboolean on_cmt_time_info_eventbox_button_press_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_cmt_show_time_changed ( );
    return FALSE;
}


G_MODULE_EXPORT gboolean on_cmt_time_eventbox_button_press_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_cmt_show_time_changed ( );
    return FALSE;
}
