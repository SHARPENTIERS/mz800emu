/* 
 * File:   cmtext.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. května 2018, 13:19
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
#include <string.h>
#include <strings.h>

#include "cmtext.h"
#include "cmtext_container.h"
#include "cmtext_block.h"

/* Pripojeni cmt extenzi */

#include "cmt_wav.h"
#include "cmt_mzf.h"
#include "cmt_mzftape.h"
#include "cmt_tap.h"

st_CMTEXT *g_cmtext[] = {
                         &g_cmt_wav_extension,
                         &g_cmt_mzf_extension,
                         &g_cmt_mzftape_extension,
                         &g_cmt_tap_extension,
                         NULL
};

/* Obecne cmtext rutiny */

int g_count_extensions = 0;


void cmtext_init ( void ) {
    int i = 0;
    while ( g_cmtext[i] != NULL ) {
        if ( g_cmtext[i]->cb_init != NULL ) g_cmtext[i]->cb_init ( );
        i++;
    };
    g_count_extensions = i;
}


void cmtext_exit ( void ) {
    int i;
    for ( i = 0; i < g_count_extensions; i++ ) {
        if ( g_cmtext[i]->cb_exit != NULL ) g_cmtext[i]->cb_exit ( );
    };
}


st_CMTEXT* cmtext_get_extension ( const char *filename ) {

    int filename_length = strlen ( filename );

    if ( filename_length < 5 ) {
        fprintf ( stderr, "%s():%d - Can't resolve file extension - '%s'\n", __func__, __LINE__, filename );
        return NULL;
    };

    const char *file_ext = &filename[( filename_length - 3 )];

    int i;
    for ( i = 0; i < g_count_extensions; i++ ) {
        if ( g_cmtext[i]->info == NULL ) continue;
        char **valid_fileexts = g_cmtext[i]->info->fileext;
        int j = 0;
        while ( valid_fileexts[j] != NULL ) {
            if ( 0 == strncasecmp ( file_ext, valid_fileexts[j++], 3 ) ) return g_cmtext[i];
        };
    };

    return NULL;
}


const char* cmtext_get_description ( st_CMTEXT *ext ) {
    if ( ( ext ) && ( ext->info ) && ( ext->info->description ) ) {
        return ext->info->description;
    };
    return "Unknown CMT extension (!)";
}


const char* cmtext_get_name ( st_CMTEXT *ext ) {
    if ( ( ext ) && ( ext->info ) && ( ext->info->name ) ) {
        return ext->info->name;
    };
    return "CMT_UNKNOWN";
}
