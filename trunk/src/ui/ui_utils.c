/* 
 * File:   ui_utils.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 20. září 2015, 22:05
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


#include <stdio.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>


#include "ui_utils.h"

GError *g_ui_utils_error;


FILE* ui_utils_fopen ( const char *filename_in_utf8, const char *mode ) {
    char *filename_in_locale;
    FILE *fp;

    filename_in_locale = g_locale_from_utf8 ( filename_in_utf8, -1, NULL, NULL, NULL );
    fp = fopen ( filename_in_locale, mode );
    g_free ( filename_in_locale );
    return fp;
}


int ui_utils_access ( const char *filename_in_utf8, int type ) {
    char *filename_in_locale;
    filename_in_locale = g_locale_from_utf8 ( filename_in_utf8, -1, NULL, NULL, NULL );
    int retval = access ( filename_in_locale, type );
    g_free ( filename_in_locale );
    return retval;
}


UI_UTILS_DIR_HANDLE* ui_utils_dir_open ( char *path ) {
    return g_dir_open ( path, 0, &g_ui_utils_error );
}


void ui_utils_dir_close ( UI_UTILS_DIR_HANDLE *dh ) {
    g_dir_close ( dh );
}


UI_UTILS_DIR_ITEM* ui_utils_dir_read ( UI_UTILS_DIR_HANDLE *dh ) {
    return g_dir_read_name ( dh );
}


const gchar* ui_utils_ditem_get_name ( UI_UTILS_DIR_ITEM *ditem ) {
    return ditem;
}


const gchar* ui_utils_ditem_get_filepath ( const char *path, UI_UTILS_DIR_ITEM *ditem ) {
    return g_build_filename ( path, ditem, (gchar*) NULL );
}


void ui_utils_free ( void *ptr ) {
    g_free ( ptr );
}


unsigned ui_utils_ditem_is_file ( char *path, UI_UTILS_DIR_ITEM *ditem ) {
    const gchar* filepath = ui_utils_ditem_get_filepath ( path, ditem );
    unsigned retval = g_file_test ( filepath, G_FILE_TEST_IS_REGULAR );
    ui_utils_free ( (void*) filepath );
    return retval;
}


const gchar* ui_utils_get_error_message ( void ) {
    return g_ui_utils_error->message;
}


int ui_utils_rename_file ( char *path, char *oldname, char *newname ) {
    
    gchar* filepath1 = g_build_filename ( path, oldname, (gchar*) NULL );
    gchar* filepath2 = g_build_filename ( path, newname, (gchar*) NULL );
    
    int retval = g_rename ( filepath1, filepath2 );
    
    ui_utils_free ( (void*) filepath1 );
    ui_utils_free ( (void*) filepath2 );

    return retval;
}
