/* 
 * File:   ui_memload.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 12. června 2018, 12:37
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
#include "mz800emu_cfg.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED

#include "z80ex/include/z80ex.h"
#include "ui_membrowser.h"
#include "../ui_main.h"
#include "ui/ui_hexeditable.h"
#include "debugger/debugger.h"
#include "ramdisk/ramdisk.h"
#include "gdg/gdg.h"
#include "cmt/cmthack.h"
#include "memory/memory.h"
#include "gdg/framebuffer.h"
#include "iface_sdl//iface_sdl.h"
#include "ui_debugger.h"
#include "ui_memload.h"



static FILE *g_memload_file;
static long g_memload_fsize;
static gboolean g_ui_memload_offset_lock = FALSE;
static gboolean g_ui_memload_memaloc_lock = FALSE;


static void ui_memload_ok_button_sensitivity ( void ) {
    GtkWidget *button = ui_get_widget ( "dbg_memload_ok_button" );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ) ) );
    gboolean button_is_enabled = ( size ) ? TRUE : FALSE;
    gtk_widget_set_sensitive ( button, button_is_enabled );
}


static void ui_memload_window_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memload_window" );
    gtk_widget_hide ( window );
}


G_MODULE_EXPORT gboolean on_dbg_memload_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_memload_window_hide ( );
    fclose ( g_memload_file );
    return TRUE;
}


G_MODULE_EXPORT void on_dbg_memload_close_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;
    ui_memload_window_hide ( );
    fclose ( g_memload_file );
}


static void ui_memload_check_size ( void ) {
    uint32_t offset = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ) ) );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ) ) );
    if ( size > ( g_memload_fsize - offset ) ) {
        char buff[5];
        snprintf ( buff, sizeof ( buff ), "%04X", (Z80EX_WORD) ( g_memload_fsize - offset ) );
        gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ), buff );
    };
}


static uint32_t ui_memload_get_offset ( void ) {
    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memload_offset_hex_entry" ) ) ) {
        g_ui_memload_offset_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ), "0" );
        g_ui_memload_offset_lock = FALSE;
    };

    return debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ) ) );
}


G_MODULE_EXPORT void on_dbg_memload_offset_hex_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memload_offset_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    uint32_t offset = ui_memload_get_offset ( );

    char buff[11];

    if ( offset >= g_memload_fsize ) {
        offset = g_memload_fsize - 1;
        snprintf ( buff, sizeof ( buff ), "%X", offset );
        g_ui_memload_offset_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ), buff );
        g_ui_memload_offset_lock = FALSE;
    }

    snprintf ( buff, sizeof ( buff ), "%u", offset );
    g_ui_memload_offset_lock = TRUE;
    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_dec_entry" ), buff );
    g_ui_memload_offset_lock = FALSE;

    ui_memload_check_size ( );
}


G_MODULE_EXPORT void on_dbg_memload_offset_dec_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memload_offset_lock ) return;

    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memload_offset_dec_entry" ) ) ) {
        g_ui_memload_offset_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_dec_entry" ), "0" );
        g_ui_memload_offset_lock = FALSE;
    };

    uint32_t offset = atoi ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_offset_dec_entry" ) ) );

    char buff[11];

    if ( offset >= g_memload_fsize ) {
        offset = g_memload_fsize - 1;
        snprintf ( buff, sizeof ( buff ), "%u", offset );
        g_ui_memload_offset_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_dec_entry" ), buff );
        g_ui_memload_offset_lock = FALSE;
    }
    snprintf ( buff, sizeof ( buff ), "%X", offset );
    g_ui_memload_offset_lock = TRUE;
    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ), buff );
    g_ui_memload_offset_lock = FALSE;

    ui_memload_check_size ( );
}


static void ui_memload_set_hex_size ( Z80EX_WORD size ) {
    g_ui_memload_memaloc_lock = TRUE;
    char buff[5];
    snprintf ( buff, sizeof ( buff ), "%04X", size );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ), buff );
    g_ui_memload_memaloc_lock = FALSE;
    ui_memload_ok_button_sensitivity ( );
}


static void ui_memload_set_dec_size ( Z80EX_WORD size ) {
    g_ui_memload_memaloc_lock = TRUE;
    char buff[6];
    snprintf ( buff, sizeof ( buff ), "%d", size );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_dec_entry" ), buff );
    g_ui_memload_memaloc_lock = FALSE;
    ui_memload_ok_button_sensitivity ( );
}


static void ui_memload_set_size ( Z80EX_WORD size ) {
    ui_memload_set_hex_size ( size );
    ui_memload_set_dec_size ( size );
}


static void ui_memload_set_to ( Z80EX_WORD to ) {
    g_ui_memload_memaloc_lock = TRUE;
    char buff[5];
    snprintf ( buff, sizeof ( buff ), "%04X", to );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_to_entry" ), buff );
    g_ui_memload_memaloc_lock = FALSE;
}


G_MODULE_EXPORT void on_dbg_memload_from_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memload_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memload_to_entry" ) ) ) {
        ui_memload_ok_button_sensitivity ( );
        return;
    };

    Z80EX_WORD addr_from = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_from_entry" ) ) );
    Z80EX_WORD addr_to = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_to_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( addr_from > 0x1fff ) {
            addr_from = 0x1fff;
            g_ui_memload_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memload_from_entry" ), "1FFF" );
            g_ui_memload_memaloc_lock = FALSE;
        };
    };

    ui_memload_set_size ( ( addr_to - addr_from ) & mask );
    ui_memload_check_size ( );
}


static Z80EX_WORD ui_memload_get_from ( void ) {
    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memload_from_entry" ) ) ) {
        g_ui_memload_memaloc_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memload_from_entry" ), "0000" );
        g_ui_memload_memaloc_lock = FALSE;
    };

    return debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_from_entry" ) ) );
}


G_MODULE_EXPORT void on_dbg_memload_to_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memload_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    Z80EX_WORD addr_from = ui_memload_get_from ( );
    Z80EX_WORD addr_to = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_to_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( addr_to > 0x1fff ) {
            addr_to = 0x1fff;
            g_ui_memload_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memload_to_entry" ), "1FFF" );
            g_ui_memload_memaloc_lock = FALSE;
        };
    };

    ui_memload_set_size ( ( addr_to - addr_from ) & mask );
    ui_memload_check_size ( );
}


G_MODULE_EXPORT void on_dbg_memload_size_hex_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memload_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    Z80EX_WORD addr_from = ui_memload_get_from ( );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( size > 0x1fff ) {
            size = 0x1fff;
            g_ui_memload_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ), "1FFF" );
            g_ui_memload_memaloc_lock = FALSE;
        };
    };

    ui_memload_set_to ( ( addr_from + size ) & mask );
    ui_memload_set_dec_size ( size );
    ui_memload_check_size ( );
}


G_MODULE_EXPORT void on_dbg_memload_size_dec_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memload_memaloc_lock ) return;

    Z80EX_WORD addr_from = ui_memload_get_from ( );
    int atoi_size = atoi ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_size_dec_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( atoi_size > 0x1fff ) {
            atoi_size = 0x1fff;
            g_ui_memload_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_dec_entry" ), "8191" );
            g_ui_memload_memaloc_lock = FALSE;
        };
    } else {
        if ( atoi_size > 0xffff ) {
            atoi_size = 0xffff;
            g_ui_memload_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_dec_entry" ), "65535" );
            g_ui_memload_memaloc_lock = FALSE;
        };
    }

    ui_memload_set_to ( ( addr_from + (Z80EX_WORD) atoi_size ) & mask );
    ui_memload_set_hex_size ( (Z80EX_WORD) atoi_size );
    ui_memload_check_size ( );
}


void ui_memload_window_show ( void ) {

    fseek ( g_memload_file, 0, SEEK_END );
    g_memload_fsize = ftell ( g_memload_file );
    if ( g_memload_fsize > 0xffffffff ) {
        ui_show_error ( "This file is too big for this operation.\n" );
        return;
    };

    char buff[11];

    snprintf ( buff, sizeof ( buff ), "0x%04x", (unsigned) g_memload_fsize );
    gtk_label_set_text ( ui_get_label ( "dbg_memload_size_hex_label" ), buff );

    snprintf ( buff, sizeof ( buff ), "%d", (unsigned) g_memload_fsize );
    gtk_label_set_text ( ui_get_label ( "dbg_memload_size_dec_label" ), buff );

    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ), "0" );

    Z80EX_WORD memmax = g_membrowser.mem_size - 1;

    snprintf ( buff, sizeof ( buff ), "%04X", ( g_memload_fsize < memmax ) ? (unsigned) g_memload_fsize : memmax );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ), buff );

    gtk_entry_set_text ( ui_get_entry ( "dbg_memload_offset_hex_entry" ), "0" );

    ui_memload_ok_button_sensitivity ( );

    GtkWidget *window = ui_get_widget ( "dbg_memload_window" );
    gtk_widget_grab_focus ( ui_get_widget ( "dbg_memload_from_entry" ) );
    gtk_widget_show ( window );
}


void ui_memload_select_file ( void ) {

    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new ( "Load file into memory",
                                           ui_get_window ( "dbg_membrowser_window" ),
                                           action,
                                           "_Cancel",
                                           GTK_RESPONSE_CANCEL,
                                           "_Save",
                                           GTK_RESPONSE_ACCEPT,
                                           NULL );
    chooser = GTK_FILE_CHOOSER ( dialog );

    char *filename = NULL;

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );
    if ( res == GTK_RESPONSE_ACCEPT ) filename = gtk_file_chooser_get_filename ( chooser );

    gtk_widget_destroy ( dialog );

    if ( filename ) {
        g_memload_file = g_fopen ( filename, "rb" );
        if ( g_memload_file ) {
            ui_memload_window_show ( );
        } else {
            ui_show_error ( "Can't open file '%s'\n", filename );
        };
        g_free ( filename );
    };
}


G_MODULE_EXPORT void on_dbg_memload_ok_button_clicked ( GtkButton *button, gpointer user_data ) {
    ui_memload_window_hide ( );

    uint32_t offset = ui_memload_get_offset ( );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_size_hex_entry" ) ) );
    Z80EX_WORD addr = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memload_from_entry" ) ) );

    uint8_t data[0x10000];
    fseek ( g_memload_file, offset, SEEK_SET );
    uint32_t read_size = fread ( data, 1, size, g_memload_file );
    fclose ( g_memload_file );
    if ( read_size != size ) {
        ui_show_error ( "Can't read data from file\n" );
    };

    if ( !TEST_EMULATION_PAUSED ) {
        mz800_pause_emulation ( 1 );
    };

    Z80EX_WORD mem_max = g_membrowser.mem_size - 1;

    if ( g_membrowser.MEM != NULL ) {

        Z80EX_WORD src_addr = 0;

        if ( ( ( g_membrowser.memsrc == MEMBROWSER_SOURCE_PEZIK_E8 ) || ( g_membrowser.memsrc == MEMBROWSER_SOURCE_PEZIK_68 ) ) && ( g_membrowser.pezik_addressing == MEMBROWSER_PEZIK_ADDRESSING_LE ) ) {
            /* pezik little endian */
            while ( size ) {
                unsigned le_addr = addr + src_addr;
                g_membrowser.MEM[ ( ( le_addr & 0xff ) << 8 ) | ( le_addr >> 8 )] = data[src_addr++];
                size--;
            };
        } else {
            while ( size ) {
                uint32_t i = addr + size;
                uint32_t load_size = size;

                if ( i > mem_max ) {
                    load_size = mem_max - addr;
                };

                memcpy ( &g_membrowser.MEM[addr], &data[src_addr], load_size );

                size -= load_size;
                src_addr += load_size;
                addr += load_size + 1;
                addr &= mem_max;
            };
        };

    } else {
        //if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_MAPED ) {
        cmthack_load_data_into_actual_mapped_memory ( data, addr, size );
    };

    printf ( "\nLoad done.\n" );

    // Pokud se psalo do VRAM pres "MAPED", tak by se mel provest update obrazu sam, 
    // ale po primem zapisu do g_memoryVRAM_* je potreba udelat update framebufferu a prekreslit okno.
    if ( mem_max == 0x1fff ) {
        if ( DMD_TEST_MZ700 ) {
            framebuffer_update_MZ700_all_rows ( );
        } else {
            framebuffer_MZ800_all_screen_rows_fill ( );
        };
        g_iface_sdl.redraw_full_screen_request = 1;
        iface_sdl_update_window ( );
    };

    ui_membrowser_refresh ( );

    if ( gtk_widget_is_visible ( ui_get_widget ( "debugger_main_window" ) ) ) {
        ui_debugger_update_disassembled ( ui_debugger_dissassembled_get_first_addr ( ), -1 );
        ui_debugger_update_stack ( );
    };
}

#endif
