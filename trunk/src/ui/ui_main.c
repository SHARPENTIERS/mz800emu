/* 
 * File:   ui_main.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 3. července 2015, 9:44
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


#define _GNU_SOURCE /* vasprintf */

#ifdef WIN32
#include<windows.h>
#endif

/*
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#define GETTEXT_PACKAGE "gtk30"
 */
#include <gtk/gtk.h>

/*
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
 */



#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>



#include "main.h"
#include "ui_main.h"
#include "iface_sdl/iface_sdl.h"
#include "ui_display.h"

#include "mz800.h"

#include "cfgmain.h"
#include "build_time.h"



#ifdef MZ800_DEBUGGER
#include "debugger/debugger.h"
#include "ui/debugger/ui_breakpoints.h"
#endif

st_UI g_ui;
static int g_ui_is_initialised = 0;

#define UI_RESOURCES_DIR    "ui_resources/"


void ui_update_last_folder_value ( en_FILETYPE file_type, char *value ) {
    unsigned new_length = strlen ( value ) + 1;
    g_ui.last_folder[file_type] = realloc ( g_ui.last_folder[file_type], new_length );
    if ( g_ui.last_folder[file_type] == NULL ) {
        fprintf ( stderr, "%s():%d - Could not allocate memory: %s\n", __FUNCTION__, __LINE__, strerror ( errno ) );
        main_app_quit ( EXIT_FAILURE );
    };
    strcpy ( g_ui.last_folder[file_type], (char*) value );
}


void ui_propagatecfg_folder_mzf ( void *e, void *data ) {
    ui_update_last_folder_value ( FILETYPE_MZF, cfgelement_get_text_value ( (CFGELM *) e ) );
}


void ui_propagatecfg_folder_dsk ( void *e, void *data ) {
    ui_update_last_folder_value ( FILETYPE_DSK, cfgelement_get_text_value ( (CFGELM *) e ) );
}


void ui_propagatecfg_folder_dat ( void *e, void *data ) {
    ui_update_last_folder_value ( FILETYPE_DAT, cfgelement_get_text_value ( (CFGELM *) e ) );
}


void ui_savecfg_folder_mzf ( void *e, void *data ) {
    cfgelement_set_text_value ( (CFGELM *) e, g_ui.last_folder[FILETYPE_MZF] );
}


void ui_savecfg_folder_dsk ( void *e, void *data ) {
    cfgelement_set_text_value ( (CFGELM *) e, g_ui.last_folder[FILETYPE_DSK] );
}


void ui_savecfg_folder_dat ( void *e, void *data ) {
    cfgelement_set_text_value ( (CFGELM *) e, g_ui.last_folder[FILETYPE_DAT] );
}


//void ui_init ( int argc, char *argv[] ) {


void ui_init ( void ) {

    setlocale ( LC_ALL, "" );
    /*
            bindtextdomain ( GETTEXT_PACKAGE, DATADIR "/locale" );
            bind_textdomain_codeset ( GETTEXT_PACKAGE, "UTF-8" );
            textdomain ( GETTEXT_PACKAGE );
     */

    //gtk_init ( &argc, &argv );
    gtk_init ( 0, NULL );


    printf ( "GTK UI Init: %d.%d.%d\n", gtk_get_major_version ( ), gtk_get_minor_version ( ), gtk_get_micro_version ( ) );


    /* prozatim nepotrebujeme */
    /* add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");*/


    int i;
    for ( i = 0; i < FILETYPES_COUNT; i++ ) {
        g_ui.last_folder[i] = malloc ( 1 );
        if ( g_ui.last_folder[i] == NULL ) {
            fprintf ( stderr, "%s():%d - Could not allocate memory: %s\n", __FUNCTION__, __LINE__, strerror ( errno ) );
            main_app_quit ( EXIT_FAILURE );
        };
        strcpy ( g_ui.last_folder[i], "" );
    };

    g_ui.last_filetype = 0;


    UNLOCK_UICALLBACKS ( );
    g_ui.builder = NULL;

    g_ui.builder = gtk_builder_new ( );
    GError *err = NULL;
    if ( 0 == gtk_builder_add_from_file ( g_ui.builder, UI_RESOURCES_DIR "mz800emu.glade", &err ) ) {
        printf ( "GtkBuilder error: %s\n", err->message );
    };
    if ( 0 == gtk_builder_add_from_file ( g_ui.builder, UI_RESOURCES_DIR "mz800emu_cmt.glade", &err ) ) {
        printf ( "GtkBuilder error: %s\n", err->message );
    };


#ifdef MZ800_DEBUGGER
    /* TODO: prozkoumat GtkCssProvider - nastavit rules hint pro disassembled a stack treeview */
    if ( 0 == gtk_builder_add_from_file ( g_ui.builder, UI_RESOURCES_DIR "mz800emu_debugger.glade", &err ) ) {
        printf ( "GtkBuilder error: %s\n", err->message );
    };
#else
    gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_debugger" ), FALSE );
#endif


    gtk_builder_connect_signals ( g_ui.builder, NULL );
    /* jina moznost jak registrovat callbacky:*/
    /* gtk_builder_add_callback_symbol ( builder, "on_circle_clicked",  G_CALLBACK ( on_circle_clicked ) ); */
    /* g_object_unref ( G_OBJECT ( builder ) ); */


    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new ( );
    display = gdk_display_get_default ( );
    screen = gdk_display_get_default_screen ( display );

    gtk_style_context_add_provider_for_screen ( screen,
            GTK_STYLE_PROVIDER ( provider ),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );



    gtk_style_context_add_provider_for_screen ( screen,
            GTK_STYLE_PROVIDER ( provider ),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );

#if 0
    gtk_css_provider_load_from_data ( GTK_CSS_PROVIDER ( provider ),
            " GtkWindow {\n"
            "   -GtkWidget-focus-line-width: 0;\n"
            /* The next 2 lines are not guaranteed to work, they can be can be overridden by the window manager*/
            "   -GtkWindow-resize-grip-height: 0;\n"
            "   -GtkWindow-resize-grip-width: 0;\n"
            /* The next 4 lines are just 4 different ways to make the background blue. Each one overrides the last one.
                Their just different color units: named color units, rgb,  rgba, hexidecimal, and shade*/
            "   background-color: blue;\n"
            "   background-color: rgb (0, 0, 255);\n"
            "   background-color: rgba (0,0,255,1);\n"
            "   background-color: #0000FF;\n"
            "   background-color: shade(blue, 1.0);\n"
            "}\n", -1, NULL );
#endif

    gtk_css_provider_load_from_path ( GTK_CSS_PROVIDER ( provider ), UI_RESOURCES_DIR "mz800emu.css", &err );

    if ( err ) {
        printf ( "provider load error: %s\n", err->message );
    };


    g_object_unref ( provider );



    /*
     *  Konfiguracni udaje pro UI
     */


    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "UI" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "filebrowser_last_folder_mzf", CFGENTYPE_TEXT, "" );
    cfgelement_set_propagate_cb ( elm, ui_propagatecfg_folder_mzf, NULL );
    cfgelement_set_save_cb ( elm, ui_savecfg_folder_mzf, NULL );

    elm = cfgmodule_register_new_element ( cmod, "filebrowser_last_folder_dsk", CFGENTYPE_TEXT, "" );
    cfgelement_set_propagate_cb ( elm, ui_propagatecfg_folder_dsk, NULL );
    cfgelement_set_save_cb ( elm, ui_savecfg_folder_dsk, NULL );

    elm = cfgmodule_register_new_element ( cmod, "filebrowser_last_folder_dat", CFGENTYPE_TEXT, "" );
    cfgelement_set_propagate_cb ( elm, ui_propagatecfg_folder_dat, NULL );
    cfgelement_set_save_cb ( elm, ui_savecfg_folder_dat, NULL );

    elm = cfgmodule_register_new_element ( cmod, "filebrowser_last_filetype", CFGENTYPE_KEYWORD, FILETYPE_MZF,
            FILETYPE_MZF, "MZF",
            FILETYPE_DSK, "DSK",
            FILETYPE_DAT, "DAT",
            -1 );
    cfgelement_set_handlers ( elm, (void*) &g_ui.last_filetype, (void*) &g_ui.last_filetype );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    g_ui_is_initialised = 1;

    /* display bylo nacteno jeste pred inicializaci ui, proto udelame update_menu nyni */
    ui_display_update_menu ( );
}


void ui_exit ( void ) {

    int i;
    for ( i = 0; i < FILETYPES_COUNT; i++ ) {
        if ( g_ui.last_folder[i] != NULL ) {
            free ( g_ui.last_folder[i] );
        };
    };

    g_ui_is_initialised = 0;
    /* TODO: je potreba nejak specialne ukoncit i gtk?*/
}


void ui_iteration ( void ) {
    while ( gtk_events_pending ( ) ) {
        gtk_main_iteration ( );
    };
}


#if 0


void ui_short_iteration ( void ) {
    if ( gtk_events_pending ( ) ) {
        gtk_main_iteration ( );
    };
}
#endif


/*
void ui_show_error_dialog ( char *error_message ) {
    GtkWidget *dialog = gtk_message_dialog_new ( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, error_message );
    gtk_dialog_run ( GTK_DIALOG ( dialog ) );
    gtk_widget_destroy ( dialog );
    ui_iteration ( );
}
 */


#ifdef UI_USE_ERRORLOG


void ui_write_errorlog ( char *lvl, char *msg ) {
    FILE *fp;
    if ( NULL == ( fp = fopen ( UI_ERRORLOG_FILE, "a" ) ) ) {
        fprintf ( stderr, "%s():%d - '%s' - fopen error: %s", __FUNCTION__, __LINE__, UI_ERRORLOG_FILE, strerror ( errno ) );
    } else {
        fprintf ( fp, "%s %s:\t%s\n", cfgmain_create_timestamp ( ), lvl, msg );
        fclose ( fp );
    };
}
#endif


/* modalni okno s chybou */
void ui_show_error ( char *format, ... ) {

    char *msg = NULL;
    va_list args;
    va_start ( args, format );

    /* TODO: W32NAT nezna vasprintf - asi by bylo reseni pouzit vsnprintf (pokud jej tam existuje) */
#if W32NAT
    msg = "Unknown Error Message: your build is completed on host without vasprintf()\nCheck console for details.";
    vprintf ( format, args );
#else
    vasprintf ( &msg, format, args );
#endif
    va_end ( args );

    GtkWidget *dialog = NULL;
    if ( g_ui_is_initialised == 1 ) {
        dialog = gtk_message_dialog_new ( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, msg );
    };

#ifndef W32NAT
    char *msg_in_locale;
    msg_in_locale = g_locale_from_utf8 ( msg, -1, NULL, NULL, NULL );
    fprintf ( stderr, "\nUI_ERROR: %s\n", msg_in_locale );
    g_free ( msg_in_locale );
#endif

#ifdef UI_USE_ERRORLOG
    ui_write_errorlog ( "ERROR", msg );
#endif

    free ( msg );

    if ( g_ui_is_initialised == 1 ) {
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
    };
}


/* nemodalni okno s warningem */
void ui_show_warning ( char *format, ... ) {

    char *msg = NULL;
    va_list args;
    va_start ( args, format );

    /* TODO: W32NAT nezna vasprintf  - asi by bylo reseni pouzit vsnprintf (pokud jej tam existuje) */
#if W32NAT
    msg = "Unknown Error Message: your build is completed on host without vasprintf()\nCheck console for details.";
    vprintf ( format, args );
#else
    vasprintf ( &msg, format, args );
#endif

    va_end ( args );

    GtkWidget *dialog = NULL;
    if ( g_ui_is_initialised == 1 ) {
        dialog = gtk_message_dialog_new ( NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, msg );
    };


#ifndef W32NAT
    char *msg_in_locale;
    msg_in_locale = g_locale_from_utf8 ( msg, -1, NULL, NULL, NULL );
    fprintf ( stderr, "\nUI_WARNING: %s\n", msg_in_locale );
    g_free ( msg_in_locale );
#endif

#ifdef UI_USE_ERRORLOG
    ui_write_errorlog ( "WARNING", msg );
#endif

    free ( msg );

    if ( g_ui_is_initialised == 1 ) {
        g_signal_connect_swapped ( dialog, "response", G_CALLBACK ( gtk_widget_destroy ), dialog );
        gtk_widget_show ( dialog );
    };
}


unsigned ui_open_fille ( char *filename, char *predefined_filename, unsigned max_filename_size, en_FILETYPE filetype, char *window_title, en_OPENMODE openmode ) {

    GtkWidget *filechooserdialog;
    GtkFileFilter *filter;

    GtkFileChooserAction fcaction;
    if ( openmode == OPENMODE_READ ) {
        fcaction = GTK_FILE_CHOOSER_ACTION_OPEN;
    } else {
        fcaction = GTK_FILE_CHOOSER_ACTION_SAVE;
    };
    filechooserdialog = gtk_file_chooser_dialog_new ( window_title, NULL,
            fcaction,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT,
            NULL );

    gtk_container_set_border_width ( GTK_CONTAINER ( filechooserdialog ), 5 );
    g_object_set ( filechooserdialog, "local-only", FALSE, NULL );
    gtk_window_set_modal ( GTK_WINDOW ( filechooserdialog ), TRUE );
    gtk_window_set_skip_taskbar_hint ( GTK_WINDOW ( filechooserdialog ), TRUE );
    gtk_window_set_skip_pager_hint ( GTK_WINDOW ( filechooserdialog ), TRUE );
    gtk_window_set_type_hint ( GTK_WINDOW ( filechooserdialog ), GDK_WINDOW_TYPE_HINT_DIALOG );
    gtk_window_set_urgency_hint ( GTK_WINDOW ( filechooserdialog ), TRUE );

    filter = gtk_file_filter_new ( );

    if ( filetype == FILETYPE_MZF ) {
        gtk_file_filter_add_pattern ( filter, "*.mzf" );
        gtk_file_filter_add_pattern ( filter, "*.MZF" );
        gtk_file_filter_set_name ( filter, "MZ-800 Tape file" );
    } else if ( filetype == FILETYPE_DSK ) {
        gtk_file_filter_add_pattern ( filter, "*.dsk" );
        gtk_file_filter_add_pattern ( filter, "*.DSK" );
        gtk_file_filter_set_name ( filter, "MZ-800 Disc file" );
    } else if ( filetype == FILETYPE_DAT ) {
        gtk_file_filter_add_pattern ( filter, "*.dat" );
        gtk_file_filter_add_pattern ( filter, "*.DAT" );
        gtk_file_filter_set_name ( filter, "Ramdisk DAT file" );
    };

    gtk_file_chooser_add_filter ( (GtkFileChooser*) filechooserdialog, filter );

    if ( predefined_filename[0] != 0x00 ) {
        gtk_file_chooser_set_filename ( GTK_FILE_CHOOSER ( filechooserdialog ), predefined_filename );
    } else if ( g_ui.last_folder[filetype][0] != 0x00 ) {
        gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( filechooserdialog ), g_ui.last_folder[filetype] );
    } else if ( g_ui.last_folder[g_ui.last_filetype][0] != 0x00 ) {
        gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( filechooserdialog ), g_ui.last_folder[g_ui.last_filetype] );
    } else {
        gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( filechooserdialog ), "./" );
    };

    gtk_widget_show ( filechooserdialog );

    unsigned uiret = UIRET_FAILED;

    while ( gtk_dialog_run ( GTK_DIALOG ( filechooserdialog ) ) == GTK_RESPONSE_ACCEPT ) {

        char* current_folder = gtk_file_chooser_get_current_folder ( GTK_FILE_CHOOSER ( filechooserdialog ) );
        ui_update_last_folder_value ( filetype, current_folder );
        g_ui.last_filetype = filetype;

        if ( strlen ( (char*) gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( filechooserdialog ) ) ) < max_filename_size ) {
            strncpy ( filename, (char*) gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( filechooserdialog ) ), max_filename_size );
            filename [ max_filename_size - 1 ] = 0x00;
            uiret = UIRET_OK;
            break;
        } else {
            ui_show_error ( "Sorry, filepath is too big!" );
        };
    };

    gtk_widget_destroy ( filechooserdialog );

    //    ui_iteration ( );

    return uiret;
}

#ifdef UI_TOPMENU_IS_WINDOW


void ui_hide_main_menu ( void ) {
    GtkWidget *main_menu = ui_get_widget ( "main_window" );
    gtk_widget_hide ( main_menu );
}
#endif


void ui_show_hide_main_menu ( void ) {

    GtkWidget *main_menu = NULL;

    /* V Linuxu se mi nepodarilo otevrit samotny popup bez okna :( */
#ifdef UI_TOPMENU_IS_WINDOW
    main_menu = ui_get_widget ( "main_window" );
#else
    main_menu = ui_get_widget ( "menu_emulator_setup" );
#endif


#ifdef UI_TOPMENU_IS_WINDOW
    if ( gtk_widget_get_visible ( main_menu ) ) {
        gtk_widget_hide ( main_menu );
    } else {
        gtk_widget_show ( main_menu );
    };
#else
    gtk_menu_popup ( GTK_MENU ( main_menu ), NULL, NULL, NULL, NULL, 0, 0 );
#endif
}


/*
 * 
 * Callbacky main menu
 * 
 */


G_MODULE_EXPORT void on_reset ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    mz800_reset ( );
}


G_MODULE_EXPORT void on_max_cpu_speed ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( FALSE == gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_max_cpu_speed" ) ) ) {
        mz800_set_cpu_speed ( MZ800_EMULATION_SPEED_NORMAL );
    } else {
        mz800_set_cpu_speed ( MZ800_EMULATION_SPEED_MAX );
    };
}


/* 
 * 0 - normal
 * 1 - max
 * 
 */
void ui_main_update_cpu_speed_menu ( unsigned state ) {
    LOCK_UICALLBACKS ( );
    if ( state ) {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_max_cpu_speed" ), TRUE );
    } else {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_max_cpu_speed" ), FALSE );
    };
    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT void on_pause_emulation ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( FALSE == gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_pause_emulation" ) ) ) {
        mz800_pause_emulation ( 0 );
    } else {
        mz800_pause_emulation ( 1 );
    };
}


/* 
 * 0 - normal state
 * 1 - paused
 * 
 */
void ui_main_update_emulation_state ( unsigned state ) {
    LOCK_UICALLBACKS ( );
    if ( state ) {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_pause_emulation" ), TRUE );
    } else {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_pause_emulation" ), FALSE );
    };
    UNLOCK_UICALLBACKS ( );
}



#ifdef MZ800_DEBUGGER


G_MODULE_EXPORT void on_open_debugger ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    debugger_show_hide_main_window ( );
}


G_MODULE_EXPORT void on_menuitem_open_breakpoints_activate ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    ui_breakpoints_show_hide_window ( );
}

#else


/* Tohle zde musi byt, aby se nevypisoval warning, ze nebyl nalezen callback */

G_MODULE_EXPORT void on_open_debugger ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
}


G_MODULE_EXPORT void on_menuitem_open_breakpoints_activate ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
}
#endif



#ifdef WIN32


/* BUGFIX: ve win32 nefunguje spravne gtk_show_uri() */
void on_aboutdialog_activate_link_event ( GtkMenuItem *menuitem, gpointer user_data ) {
    GtkWidget *window = ui_get_widget ( "emulator_aboutdialog" );
    ShellExecute ( NULL, "open", gtk_about_dialog_get_website ( GTK_ABOUT_DIALOG ( window ) ), NULL, NULL, SW_SHOWNORMAL );
}
#endif


G_MODULE_EXPORT void on_about_menuitem_activate ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    GtkWidget *window = ui_get_widget ( "emulator_aboutdialog" );
    static int initialised = 0;
    if ( initialised == 0 ) {

        unsigned version_length = 0;
        char *version_format = "version: %s\nbuild: %s"; /* verze, build time */

        version_length += strlen ( CFGMAIN_EMULATOR_VERSION_TEXT );
        version_length += strlen ( build_time_get ( ) );
        version_length += strlen ( version_format ) + 1;

        gchar *version_txt = malloc ( version_length );
        sprintf ( version_txt, version_format, CFGMAIN_EMULATOR_VERSION_TEXT, build_time_get ( ) );

        gtk_about_dialog_set_version ( GTK_ABOUT_DIALOG ( window ), version_txt );

        free ( version_txt );

        g_signal_connect ( GTK_DIALOG ( window ), "response", G_CALLBACK ( gtk_widget_hide ), window );
#ifdef WIN32
        g_signal_connect ( GTK_DIALOG ( window ), "activate-link", G_CALLBACK ( on_aboutdialog_activate_link_event ), NULL );
#endif
        initialised = 1;
    };
    gtk_widget_show ( window );
}


G_MODULE_EXPORT gboolean on_emulator_aboutdialog_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    GtkWidget *window = ui_get_widget ( "emulator_aboutdialog" );
    gtk_widget_hide ( window );
    return TRUE;
}