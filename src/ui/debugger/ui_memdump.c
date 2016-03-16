/* 
 * File:   ui_memdump.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 26. října 2015, 9:45
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
#include <string.h>

#ifdef MZ800_DEBUGGER

#include "z80ex/include/z80ex.h"
#include "ui_memdump.h"
#include "../ui_main.h"
#include "debugger/debugger.h"
#include "memory/memory.h"
#include "ramdisk/ramdisk.h"
#include "sharpmz_ascii.h"

typedef enum en_MEMSRC {
    MEMSRC_MAPED = 0,
    MEMSRC_RAM,
    MEMSRC_VRAM,
    MEMSRC_PEZIK_E8,
    MEMSRC_PEZIK_68,
    MEMSRC_MZ1R18,
} en_MEMSRC;

typedef enum en_MEMCHARSET {
    MEMCHAR_ASCII = 0,
    MEMCHAR_SHASCII,
} en_MEMCHARSET;

typedef struct st_UIMEMDUMP {
    st_UIWINPOS main_pos;

    en_MEMCHARSET charset;
    en_MEMSRC memsrc;
    unsigned vram_plane;
    unsigned pezik_e8_bank;
    unsigned pezik_68_bank;
    unsigned pezik_endianity;
    unsigned mr1z18_bank;
} st_UIMEMDUMP;

st_UIMEMDUMP g_uimemdump;

typedef enum en_DBG_MEMDUMP {
    DBG_MEMDUMP_ADDR = 0,
    DBG_MEMDUMP_ADDR_TXT,
    DBG_MEMDUMP_VALUES,
    DBG_MEMDUMP_ASCII,
    DBG_MEMDUMP_COUNT_COLUMNS
} en_DBG_MEMDUMP;


void ui_memdump_update ( void ) {

    LOCK_UICALLBACKS ( );

    gtk_combo_box_set_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ), g_uimemdump.memsrc );

    GtkWidget *label = ui_get_widget ( "dbg_memdump_bank_label" );
    GtkWidget *spinbutton = ui_get_widget ( "dbg_memdump_bank_spinbutton" );
    GtkWidget *endianness_combo = ui_get_widget ( "dbg_memdump_pezik_endianness_comboboxtext" );

    gtk_combo_box_set_active ( (GtkComboBox*) endianness_combo, g_uimemdump.pezik_endianity );
    gtk_widget_hide ( endianness_combo );
    
    if ( g_uimemdump.memsrc >= MEMSRC_VRAM ) {

        gtk_widget_show ( label );
        gtk_widget_show ( spinbutton );

        if ( g_uimemdump.memsrc == MEMSRC_VRAM ) {
            gtk_label_set_text ( GTK_LABEL ( label ), "Plane:" );
            if ( g_uimemdump.vram_plane > 3 ) g_uimemdump.vram_plane = 0;
            gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, 3 );
            gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.vram_plane );
        } else {
            gtk_label_set_text ( GTK_LABEL ( label ), "Bank:" );
            if ( g_uimemdump.memsrc == MEMSRC_PEZIK_68 ) {
                gtk_widget_show ( endianness_combo );
                if ( !g_ramdisk.pezik [ RAMDISK_PEZIK_68 ].connected ) {
                    g_uimemdump.memsrc = MEMSRC_MAPED;
                    ui_memdump_update ( );
                    return;
                };
                if ( g_uimemdump.pezik_68_bank > 7 ) g_uimemdump.pezik_68_bank = 0;
                gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, 7 );
                gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.pezik_68_bank );
            } else if ( g_uimemdump.memsrc == MEMSRC_PEZIK_E8 ) {
                gtk_widget_show ( endianness_combo );
                if ( !g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].connected ) {
                    g_uimemdump.memsrc = MEMSRC_MAPED;
                    ui_memdump_update ( );
                    return;
                };
                if ( g_uimemdump.pezik_e8_bank > 7 ) g_uimemdump.pezik_e8_bank = 0;
                gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, 7 );
                gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.pezik_e8_bank );
            } else if ( g_uimemdump.memsrc == MEMSRC_MZ1R18 ) {
                if ( !g_ramdisk.std.connected ) {
                    g_uimemdump.memsrc = MEMSRC_MAPED;
                    ui_memdump_update ( );
                    return;
                };
                g_uimemdump.mr1z18_bank &= g_ramdisk.std.size;
                if ( g_ramdisk.std.size == RAMDISK_SIZE_64 ) {
                    gtk_widget_hide ( label );
                    gtk_widget_hide ( spinbutton );
                } else {
                    gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, g_ramdisk.std.size );
                    gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.mr1z18_bank );
                };
            };
        };
    } else {
        gtk_widget_hide ( label );
        gtk_widget_hide ( spinbutton );
    };
    gtk_combo_box_set_active ( ui_get_combo_box ( "dbg_memdump_charset_comboboxtext" ), g_uimemdump.charset );

    UNLOCK_UICALLBACKS ( );
}


void ui_memdump_load ( void ) {

    ui_memdump_update ( );

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_memdump_liststore" ) );

    unsigned addr = 0x0000;
    unsigned addr_end = 0x10000;

    uint8_t *MEM = NULL;

    switch ( g_uimemdump.memsrc ) {

        case MEMSRC_MAPED:
            break;

        case MEMSRC_RAM:
            MEM = g_memory.RAM;
            break;

        case MEMSRC_VRAM:
            addr_end = 0x2000;
            switch ( g_uimemdump.vram_plane ) {
                case 0:
                    MEM = g_memoryVRAM_I;
                    break;
                case 1:
                    MEM = g_memoryVRAM_II;
                    break;
                case 2:
                    MEM = g_memoryVRAM_III;
                    break;
                case 3:
                    MEM = g_memoryVRAM_IV;
                    break;
            };
            break;

        case MEMSRC_PEZIK_E8:

            MEM = &g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].memory [ g_uimemdump.pezik_e8_bank << 16 ];
            break;

        case MEMSRC_PEZIK_68:
            MEM = &g_ramdisk.pezik [ RAMDISK_PEZIK_68 ].memory [ g_uimemdump.pezik_68_bank << 16 ];
            break;

        case MEMSRC_MZ1R18:
            MEM = &g_ramdisk.std.memory [ g_uimemdump.mr1z18_bank << 16 ];
            break;
    };

    static int last_dump_src = -1;

    GtkTreeIter iter;

    if ( last_dump_src != g_uimemdump.memsrc ) {
        if ( ( last_dump_src == -1 ) || ( g_uimemdump.memsrc == MEMSRC_VRAM ) || ( last_dump_src == MEMSRC_VRAM ) ) {
            gtk_list_store_clear ( GTK_LIST_STORE ( model ) );
            last_dump_src = -1;
        };
    };

    do {
        char addr_txt [ 6 ];
        sprintf ( addr_txt, "%04X:", addr );
        unsigned i;
        char values_row [ ( 16 * 3 ) + 2 ];
        char ascii_row [ 16 + 1 ];
        *values_row = 0x00;
        ascii_row [ 16 ] = 0x00;
        for ( i = 0; i < 16; i++ ) {

            Z80EX_BYTE value;

            if ( g_uimemdump.memsrc == MEMSRC_MAPED ) {
                value = debugger_memory_read_byte ( addr + i );
            } else {
                if ( ( ( g_uimemdump.memsrc == MEMSRC_PEZIK_E8 ) || ( g_uimemdump.memsrc == MEMSRC_PEZIK_68 ) ) && ( g_uimemdump.pezik_endianity == 1 ) ) {
                    /* pezik low endian */
                    unsigned le_addr = addr + i;
                    value = MEM [ ( ( le_addr & 0xff ) << 8 ) | ( le_addr >> 8 ) ];
                } else {
                    /* standardni endianita adresovani + pezik big endian */
                    value = MEM [ addr + i ];
                };
            };

            char value_txt [ 4 ];
            if ( i == 0 ) {
                sprintf ( value_txt, "%02X", value );
            } else {
                sprintf ( value_txt, " %02X", value );
            };
            strcat ( values_row, value_txt );
            if ( i == 7 ) {
                strcat ( values_row, "  " );
            };
            if ( ( value >= 0x20 ) && ( value < 0x7f ) ) {
                if ( g_uimemdump.charset == MEMCHAR_ASCII ) {
                    ascii_row [ i ] = value;
                } else {
                    ascii_row [ i ] = sharpmz_cnv_from ( value );
                };
            } else {
                ascii_row [ i ] = '.';
            };
        };

        //printf ( "%s: %s\n", addr_txt, values_row );

        if ( last_dump_src == -1 ) {
            gtk_list_store_append ( GTK_LIST_STORE ( model ), &iter );
        } else {
            if ( addr == 0 ) {
                gtk_tree_model_get_iter_first ( model, &iter );
            } else {
                gtk_tree_model_iter_next ( model, &iter );
            };
        };

        gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
                DBG_MEMDUMP_ADDR, addr,
                DBG_MEMDUMP_ADDR_TXT, addr_txt,
                DBG_MEMDUMP_VALUES, values_row,
                DBG_MEMDUMP_ASCII, ascii_row,
                -1 );
        addr += i;
    } while ( addr != addr_end );

    last_dump_src = g_uimemdump.memsrc;
}


void ui_memdump_show_window ( void ) {

    GtkWidget *window = ui_get_widget ( "dbg_memdump_window" );
    if ( gtk_widget_get_visible ( window ) ) return;

    static int initialised = 0;
    if ( initialised == 0 ) {
        initialised = 1;
        ui_main_setpos ( &g_uimemdump.main_pos, -1, -1 );
        g_uimemdump.charset = MEMCHAR_ASCII;
        g_uimemdump.memsrc = MEMSRC_MAPED;
        g_uimemdump.mr1z18_bank = 0;
        g_uimemdump.pezik_68_bank = 0;
        g_uimemdump.pezik_e8_bank = 0;
        g_uimemdump.pezik_endianity = 0;
        g_uimemdump.mr1z18_bank = 0;
        g_uimemdump.vram_plane = 0;
    };
    ui_main_win_move_to_pos ( GTK_WINDOW ( window ), &g_uimemdump.main_pos );
    gtk_widget_show ( window );
    ui_memdump_load ( );
}


void ui_memdump_hide_window ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memdump_window" );
    ui_main_win_get_pos ( GTK_WINDOW ( window ), &g_uimemdump.main_pos );
    gtk_widget_hide ( window );
}


void ui_memdump_show_hide_window ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memdump_window" );
    if ( gtk_widget_get_visible ( window ) ) {
        ui_memdump_hide_window ( );
    } else {
        ui_memdump_show_window ( );
    };
}


/*
 * 
 * Callbacks
 * 
 */

G_MODULE_EXPORT gboolean on_dbg_memdump_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_memdump_hide_window ( );
    return TRUE;
}


G_MODULE_EXPORT gboolean on_dbg_memdump_window_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {
    if ( event->keyval == GDK_KEY_Escape ) {
        ui_memdump_hide_window ( );
        return TRUE;
    };
    return FALSE;
}


G_MODULE_EXPORT void on_dbg_memdump_refresh_button_clicked ( GtkButton *button, gpointer user_data ) {
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_src_comboboxtext_changed ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    en_MEMSRC memsrc = gtk_combo_box_get_active ( combobox );

    unsigned not_connected = 0;

    if ( memsrc == MEMSRC_PEZIK_68 ) {
        not_connected = ~g_ramdisk.pezik [ RAMDISK_PEZIK_68 ].connected & 1;
    } else if ( memsrc == MEMSRC_PEZIK_E8 ) {
        not_connected = ~g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].connected & 1;
    } else if ( memsrc == MEMSRC_MZ1R18 ) {
        not_connected = ~g_ramdisk.std.connected & 1;
    };

    if ( not_connected ) {
        ui_show_error ( "This memory disc device is not connected!" );
        ui_memdump_update ( );
        return;
    };

    g_uimemdump.memsrc = memsrc;
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_charset_comboboxtext_changed ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    g_uimemdump.charset = gtk_combo_box_get_active ( combobox );
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_bank_spinbutton_value_changed ( GtkSpinButton *spinbutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    unsigned value = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( spinbutton ) );

    if ( g_uimemdump.memsrc == MEMSRC_VRAM ) {
        g_uimemdump.vram_plane = value;
    } else if ( g_uimemdump.memsrc == MEMSRC_PEZIK_68 ) {
        g_uimemdump.pezik_68_bank = value;
    } else if ( g_uimemdump.memsrc == MEMSRC_PEZIK_E8 ) {
        g_uimemdump.pezik_e8_bank = value;
    } else if ( g_uimemdump.memsrc == MEMSRC_MZ1R18 ) {
        g_uimemdump.mr1z18_bank = value;
    };

    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_pezik_endianness_comboboxtext_changed ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    g_uimemdump.pezik_endianity = gtk_combo_box_get_active ( combobox );
    ui_memdump_load ( );
}

#endif
