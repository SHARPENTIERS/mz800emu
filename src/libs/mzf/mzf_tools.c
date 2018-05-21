/* 
 * File:   mzf_tools.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 28. prosince 2017, 16:11
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

#include <string.h>
#include "mzf.h"
#include "src/sharpmz_ascii.h"


void mzf_tools_set_fname ( st_MZF_HEADER *mzfhdr, char *ascii_filename ) {

    int ascii_fname_length = strlen ( ascii_filename );
    int num_chars = ( ascii_fname_length >= MZF_FILE_NAME_LENGTH ) ? MZF_FILE_NAME_LENGTH : ascii_fname_length;

    int i;
    for ( i = 0; i < num_chars; i++ ) {
        mzfhdr->fname.name[i] = sharpmz_cnv_to ( ascii_filename[i] );
    };

    while ( i < MZF_FNAME_FULL_LENGTH ) {
        mzfhdr->fname.name[i++] = MZF_FNAME_TERMINATOR;
    };
}


uint8_t mzf_tools_get_fname_length ( st_MZF_HEADER *mzfhdr ) {
    int i = 0;
    while ( i < MZF_FNAME_FULL_LENGTH ) {
        if ( mzfhdr->fname.name[i++] == MZF_FNAME_TERMINATOR ) {
            return i;
        };
    };
    return MZF_FILE_NAME_LENGTH;
}


void mzf_tools_get_fname ( st_MZF_HEADER *mzfhdr, char *ascii_filename ) {
    uint8_t *fname = mzfhdr->fname.name;
    int i;
    for ( i = 0; i < MZF_FNAME_FULL_LENGTH; i++ ) {
        if ( *fname == MZF_FNAME_TERMINATOR ) {
            break;
        };
        if ( sharpmz_cnv_from ( *fname ) >= 0x20 ) {
            *ascii_filename++ = sharpmz_cnv_from ( *fname );
        };
        fname++;
    };
    *ascii_filename = 0x00;
}
