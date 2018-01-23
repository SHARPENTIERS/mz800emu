/* 
 * File:   ui_debugger_callbacks.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 24. srpna 2015, 11:04
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

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED

#include <string.h>

#include "ui/ui_main.h"
#include "debugger/debugger.h"
#include "mz800.h"
#include "ui_debugger.h"
#include "ui_debugger_iasm.h"
#include "memory/memory.h"
#include "ui_breakpoints.h"
#include "ui_memdump.h"
#include "ui/ui_hexeditable.h"


G_MODULE_EXPORT void on_debugger_main_window_size_allocate ( GtkWidget *widget, GdkRectangle *allocation, gpointer user_data ) {
    if ( g_debugger.animated_updates != DEBUGGER_ANIMATED_UPDATES_DISABLED ) return;
    ui_debugger_show_spinner_window ( );
}


G_MODULE_EXPORT gboolean on_debugger_main_window_configure_event ( GtkWidget *widget, GdkEventConfigure *event ) {
    if ( g_debugger.animated_updates == DEBUGGER_ANIMATED_UPDATES_DISABLED ) ui_debugger_show_spinner_window ( );
    return FALSE;
}


G_MODULE_EXPORT gboolean on_debugger_main_window_delete ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    g_uidebugger.accelerators_locked = 0;
    debugger_show_hide_main_window ( );
    return TRUE;
}


G_MODULE_EXPORT gboolean on_debugger_main_window_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {
    if ( g_uidebugger.accelerators_locked != 0 ) return FALSE;

    if ( event->keyval == GDK_KEY_Escape ) {
        debugger_show_hide_main_window ( );
        return TRUE;
    };

    return FALSE;
}


G_MODULE_EXPORT void on_dbg_hide_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
    if ( g_uidebugger.accelerators_locked != 0 ) return;
    debugger_show_hide_main_window ( );
}


G_MODULE_EXPORT void on_dbg_reset_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
    mz800_reset ( );
}


G_MODULE_EXPORT void on_dbg_speed_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
    mz800_switch_emulation_speed ( ( ~g_mz800.use_max_emulation_speed ) & 0x01 );
}


G_MODULE_EXPORT void on_dbg_pause_switch_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
    mz800_pause_emulation ( ( ~g_mz800.emulation_paused ) & 0x01 );
}


G_MODULE_EXPORT void on_dbg_play_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
        return;
    };

    debugger_step_call ( 0 );
    mz800_pause_emulation ( 0 );
}


G_MODULE_EXPORT void on_dbg_pause_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
    mz800_pause_emulation ( 1 );
}


G_MODULE_EXPORT void on_dbg_step_menuitem_activate ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
    debugger_step_call ( 1 );
}


G_MODULE_EXPORT void on_dbg_animated_enabled_radiomenuitem_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( g_uidebugger.accelerators_locked != 0 ) return;
    g_debugger.animated_updates = DEBUGGER_ANIMATED_UPDATES_ENABLED;
    ui_debugger_hide_spinner_window ( );
    debugger_step_call ( 0 );
}


G_MODULE_EXPORT void on_dbg_animated_disabled_radiomenuitem_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( g_uidebugger.accelerators_locked != 0 ) return;
    g_debugger.animated_updates = DEBUGGER_ANIMATED_UPDATES_DISABLED;
    ui_debugger_show_spinner_window ( );
}


G_MODULE_EXPORT void on_dbg_hexeditable_changed ( GtkEditable *ed, gpointer user_data ) {
    ui_hexeditable_changed ( ed, user_data );
}


G_MODULE_EXPORT void on_dbg_reg_edited ( GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data ) {

    g_uidebugger.accelerators_locked = 0;

    if ( !TEST_EMULATION_PAUSED ) return;

    //GtkTreePath *path = gtk_tree_path_new_from_string ( path_string );
    //gint row = gtk_tree_path_get_indices ( path )[0];

    GtkTreeView *treeview = GTK_TREE_VIEW ( data );
    GtkTreeModel *model = gtk_tree_view_get_model ( treeview );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_from_string ( model, &iter, path_string );

    GValue reg_id = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_REG_ID, &reg_id );

    debugger_change_z80_register ( g_value_get_uint ( &reg_id ), (Z80EX_WORD) debuger_text_to_z80_word ( new_text ) );
}


G_MODULE_EXPORT void on_dbg_stack_edited ( GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data ) {

    g_uidebugger.accelerators_locked = 0;

    if ( !TEST_EMULATION_PAUSED ) return;


    //GtkTreePath *path = gtk_tree_path_new_from_string ( path_string );
    //gint row = gtk_tree_path_get_indices ( path )[0];

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_stack_liststore" ) );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_from_string ( model, &iter, path_string );

    GValue gv = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_STACK_ADDR, &gv );

    //    printf ( "addr: 0x%04x, value: 0x%s\n", g_value_get_uint ( &addr ), new_text );

    Z80EX_WORD new_value = (Z80EX_WORD) debuger_text_to_z80_word ( new_text );
    Z80EX_WORD addr = (Z80EX_WORD) g_value_get_uint ( &gv );

    debugger_memory_write_byte ( addr, (Z80EX_BYTE) ( new_value & 0xff ) );
    debugger_memory_write_byte ( ( addr + 1 ), (Z80EX_BYTE) ( ( new_value >> 8 ) & 0xff ) );


    /* TODO: updatnout jen zmeneny radek a zachovat selection */
    ui_debugger_update_stack ( );

    /* TODO: update disassembled volat jen pokud je to nutne a pokusit se zachovat selection */
    ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
}


G_MODULE_EXPORT void on_dbg_cell_editing_canceled ( GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path_string, gpointer data ) {
    g_uidebugger.accelerators_locked = 0;
}


G_MODULE_EXPORT void on_dbg_cell_editing_started ( GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path_string, gpointer data ) {


    if ( !TEST_EMULATION_PAUSED ) {
        /* Tohle nefunguje, nicmene pokud po editing_started prijde zmena obsahu, tak se editing rezim stejne sam ukonci, takze OK */
        //gtk_cell_renderer_stop_editing ( GTK_CELL_RENDERER ( cell ), TRUE );
        ui_debugger_pause_emulation ( );
        //return;
    };

    GtkEntry *entry;
    g_uidebugger.accelerators_locked = 1;

    entry = GTK_ENTRY ( editable );
    gchar *text = gtk_editable_get_chars ( GTK_EDITABLE ( editable ), 0, -1 );
    gtk_entry_set_max_length ( entry, strlen ( text ) );
    g_signal_connect ( editable, "changed", G_CALLBACK ( on_dbg_hexeditable_changed ), NULL );
}


G_MODULE_EXPORT gboolean on_dbg_mmap ( GtkWidget *widget, GdkEventButton *event, gpointer user_data ) {
    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
    } else {
        ui_debugger_show_hide_mmap_menu ( );
    };
    return FALSE;
}


G_MODULE_EXPORT void on_dbg_mmap0_mount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_mount ( MEMORY_MAP_FLAG_ROM_0000 );
}


G_MODULE_EXPORT void on_dbg_mmap0_umount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_umount ( MEMORY_MAP_FLAG_ROM_0000 );
}


G_MODULE_EXPORT void on_dbg_mmap1_mount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_mount ( MEMORY_MAP_FLAG_ROM_1000 );
}


G_MODULE_EXPORT void on_dbg_mmap1_umount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_umount ( MEMORY_MAP_FLAG_ROM_1000 );
}


G_MODULE_EXPORT void on_dbg_mmap_vram_mount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_mount ( MEMORY_MAP_FLAG_CGRAM_VRAM );
}


G_MODULE_EXPORT void on_dbg_mmap_vram_umount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_umount ( MEMORY_MAP_FLAG_CGRAM_VRAM );
}


G_MODULE_EXPORT void on_dbg_mmape_mount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_mount ( MEMORY_MAP_FLAG_ROM_E000 );
}


G_MODULE_EXPORT void on_dbg_mmape_umount ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_umount ( MEMORY_MAP_FLAG_ROM_E000 );
}


G_MODULE_EXPORT void on_dbg_mmap_mount_all ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_mount ( MEMORY_MAP_FLAG_ROM_0000 | MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_CGRAM_VRAM | MEMORY_MAP_FLAG_ROM_E000 );
}


G_MODULE_EXPORT void on_dbg_mmap_umount_all ( GtkMenuItem *menuitem, gpointer user_data ) {
    debugger_mmap_umount ( MEMORY_MAP_FLAG_ROM_0000 | MEMORY_MAP_FLAG_ROM_1000 | MEMORY_MAP_FLAG_CGRAM_VRAM | MEMORY_MAP_FLAG_ROM_E000 );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit0 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 0, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit1 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 1, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit2 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 2, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit3 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 3, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit4 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 4, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit5 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 5, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit6 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 6, value );
}


G_MODULE_EXPORT void on_dbg_flagreg_bit7 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_flagbit ( 7, value );
}


G_MODULE_EXPORT void on_dbg_im ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    debugger_change_z80_register ( regIM, gtk_combo_box_get_active ( combobox ) );
}


G_MODULE_EXPORT void on_dbg_regiff1 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_register ( regIFF1, value );
}


G_MODULE_EXPORT void on_dbg_regiff2 ( GtkToggleButton *togglebutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    unsigned value = gtk_toggle_button_get_active ( togglebutton ) ? 1 : 0;
    debugger_change_z80_register ( regIFF2, value );
}


G_MODULE_EXPORT void on_dbg_regdmd ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;
    debugger_change_dmd ( gtk_combo_box_get_active ( combobox ) );
}


G_MODULE_EXPORT void on_dbg_play_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
        return;
    };
    debugger_step_call ( 0 );
    mz800_pause_emulation ( 0 );
}


G_MODULE_EXPORT void on_dbg_stop_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    mz800_pause_emulation ( 1 );
}


G_MODULE_EXPORT void on_dbg_step_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    debugger_step_call ( 1 );
}


G_MODULE_EXPORT void on_dbg_breakpoints_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    ui_breakpoints_show_hide_window ( );
}


G_MODULE_EXPORT void on_dbg_memdump_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    ui_memdump_show_hide_window ( );
}


G_MODULE_EXPORT gboolean on_dbg_disassembled_treeview_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {

    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
        return FALSE;
    };

    //printf ( "string: 0x%02x\n", event->string[0] );
    /* ridici klavesy maji hodnotu 0x00, 0x0d a 0x20 se pouzivaji pro aktivaci radku */
    //if ( event->string[0] <= 0x20 ) return FALSE;

    char c = event->string[0];
    if ( ( ( c >= 'a' ) && ( c <= 'z' ) ) || ( ( c >= 'A' ) && ( c <= 'Z' ) ) ) {

        GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_disassembled_liststore" ) );
        GtkTreeSelection *selection = gtk_tree_view_get_selection ( ui_get_tree_view ( "dbg_disassembled_treeview" ) );
        GtkTreeIter iter;
        gtk_tree_selection_get_selected ( selection, &model, &iter );

        GValue addr_txt = G_VALUE_INIT;
        gtk_tree_model_get_value ( model, &iter, DBG_DIS_ADDR_TXT, &addr_txt );

        ui_debugger_iasm_show_main_window ( event->string, 0, (gchar*) g_value_get_string ( &addr_txt ) );
    };

    return FALSE;
}


G_MODULE_EXPORT gboolean on_dbg_disassembled_treeview_button_press_event ( GtkWidget *widget, GdkEventButton *event, gpointer user_data ) {

    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
    } else {
        if ( 3 == event->button ) {
            ui_debugger_show_hide_disassembled_menu ( );
        };
    };
    return FALSE;
}


G_MODULE_EXPORT void on_dbg_disassembled_treeview_row_activated ( GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data ) {

    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
        return;
    };

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_disassembled_liststore" ) );
    GtkTreeIter iter;
    gtk_tree_model_get_iter ( model, &iter, path );

    GValue addr_txt = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_DIS_ADDR_TXT, &addr_txt );

    GValue mnemonics = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_DIS_MNEMONIC, &mnemonics );

    ui_debugger_iasm_show_main_window ( (gchar*) g_value_get_string ( &mnemonics ), 1, (gchar*) g_value_get_string ( &addr_txt ) );
}


G_MODULE_EXPORT void on_dbg_set_as_breakpoint_menuitem_activate ( GtkMenuItem *menuitem, gpointer user_data ) {

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_disassembled_liststore" ) );
    GtkTreeSelection *selection = gtk_tree_view_get_selection ( ui_get_tree_view ( "dbg_disassembled_treeview" ) );
    GtkTreeIter iter;
    gtk_tree_selection_get_selected ( selection, &model, &iter );
    GValue gv = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_DIS_ADDR, &gv );
    Z80EX_WORD addr = (Z80EX_WORD) g_value_get_uint ( &gv );
    ui_breakpoints_show_window ( );
    int id = ui_breakpoints_simple_add_event ( addr );
    if ( id == -1 ) return;
    ui_breakpoints_select_id ( id );
}


G_MODULE_EXPORT void on_dbg_set_as_pc_menuitem_activate ( GtkMenuItem *menuitem, gpointer user_data ) {
    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_disassembled_liststore" ) );
    GtkTreeSelection *selection = gtk_tree_view_get_selection ( ui_get_tree_view ( "dbg_disassembled_treeview" ) );
    GtkTreeIter iter;
    gtk_tree_selection_get_selected ( selection, &model, &iter );
    GValue gv = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_DIS_ADDR, &gv );
    Z80EX_WORD addr = (Z80EX_WORD) g_value_get_uint ( &gv );
    debugger_change_z80_register ( regPC, addr );
}


G_MODULE_EXPORT void on_dbg_focus_to_menuitem_activate ( GtkMenuItem *menuitem, gpointer user_data ) {
    GtkWidget *window = ui_get_widget ( "dbg_focus_to_window" );
    gtk_widget_show ( window );
    GtkWidget *entry = ui_get_widget ( "dbg_focus_to_addr_entry" );
    gtk_entry_set_alignment ( GTK_ENTRY ( entry ), 1 );
    gchar addr_txt [ 5 ];
    sprintf ( addr_txt, "%04X", g_uidebugger.last_focus_addr );
    gtk_entry_set_text ( GTK_ENTRY ( entry ), addr_txt );
    gtk_editable_select_region ( GTK_EDITABLE ( entry ), 0, -1 );
    gtk_widget_grab_focus ( entry );
}


G_MODULE_EXPORT void on_dbg_edit_row_menuitem_activate ( GtkMenuItem *menuitem, gpointer user_data ) {
    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_disassembled_liststore" ) );
    GtkTreeSelection *selection = gtk_tree_view_get_selection ( ui_get_tree_view ( "dbg_disassembled_treeview" ) );
    GtkTreeIter iter;
    gtk_tree_selection_get_selected ( selection, &model, &iter );

    GValue addr_txt = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_DIS_ADDR_TXT, &addr_txt );

    GValue mnemonics = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, &iter, DBG_DIS_MNEMONIC, &mnemonics );

    ui_debugger_iasm_show_main_window ( (gchar*) g_value_get_string ( &mnemonics ), 1, (gchar*) g_value_get_string ( &addr_txt ) );
}


G_MODULE_EXPORT gboolean on_dbg_focus_to_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    GtkWidget *window = ui_get_widget ( "dbg_focus_to_window" );
    gtk_widget_hide ( window );
    return TRUE;
}


G_MODULE_EXPORT void on_dbg_focus_to_cancel_button_clicked ( GtkButton *button, gpointer user_data ) {
    GtkWidget *window = ui_get_widget ( "dbg_focus_to_window" );
    gtk_widget_hide ( window );
}


G_MODULE_EXPORT void on_dbg_focus_to_ok_button_clicked ( GtkButton *button, gpointer user_data ) {
    GtkWidget *entry = ui_get_widget ( "dbg_focus_to_addr_entry" );
    const gchar *addr_txt = gtk_entry_get_text ( GTK_ENTRY ( entry ) );
    if ( strlen ( addr_txt ) ) {
        g_uidebugger.last_focus_addr = (Z80EX_WORD) debuger_text_to_z80_word ( addr_txt );
        ui_debugger_update_disassembled ( g_uidebugger.last_focus_addr, -1 );
        GtkWidget *window = ui_get_widget ( "dbg_focus_to_window" );
        gtk_widget_hide ( window );
    };
}


G_MODULE_EXPORT gboolean on_dbg_spinner_window_press_event ( GtkWidget *widget, GdkEventButton *event, gpointer user_data ) {
    ui_debugger_pause_emulation ( );
    return FALSE;
}


#endif
