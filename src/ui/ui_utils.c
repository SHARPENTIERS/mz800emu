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

