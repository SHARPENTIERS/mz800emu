/* 
 * File:   cmt_wav.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 22. dubna 2018, 8:55
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

#include "libs/cmt_stream/cmt_stream.h"
#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#include "libs/wav/wav.h"

#include "cmt_extension.h"
#include "cmt_wav.h"
#include "cmt.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

st_CMTEXT *g_cmtwav_extension = NULL;


static void cmtwav_extension_eject ( void ) {
    assert ( g_cmtwav_extension != NULL );
    cmtext_eject ( g_cmtwav_extension );
}


int cmtwav_open ( st_CMTEXT *cmtext, st_HANDLER *h ) {

    assert ( cmtext != NULL );
    st_CMT_BITSTREAM *cmt_bitstream = cmt_bitstream_new_from_wav ( h, ( g_cmt.polarity == CMT_POLARITY_NORMAL ) ? WAV_POLARITY_NORMAL : WAV_POLARITY_INVERTED );

    if ( !cmt_bitstream ) {
        fprintf ( stderr, "%s() - %d: Can't create bitstream\n", __func__, __LINE__ );
        return EXIT_FAILURE;
    }

    st_CMT_FILE *cmtfile = cmtext_cmtfile_new ( CMT_STREAM_TYPE_BITSTREAM );
    if ( !cmtfile ) {
        cmt_bitstream_destroy ( cmt_bitstream );
        fprintf ( stderr, "%s() - %d: Can't create cmtfile\n", __func__, __LINE__ );
        return EXIT_FAILURE;
    }
    cmtfile->type = CMT_FILETYPE_WAV;

    printf ( "CMTWAV polarity: %s\n", ( g_cmt.polarity == CMT_POLARITY_NORMAL ) ? "normal" : "inverted" );
    printf ( "CMTWAV rate: %d\n", cmt_bitstream->rate );
    printf ( "CMTWAV length: %1.2f s\n", ( cmt_bitstream->scan_time * cmt_bitstream->scans ) );

    cmtfile->stream.str.bitstream = cmt_bitstream;
    g_cmtwav_extension->cmtfile = cmtfile;

    return EXIT_SUCCESS;
}


static int cmtwav_extension_open ( char * filename ) {

    assert ( g_cmtwav_extension != NULL );

    cmtwav_extension_eject ( );

    printf ( "CMTWAV Open: %s\n", filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "CMTWAV: Can't open WAV file '%s'\n", filename );
        return EXIT_FAILURE;
    };

    if ( EXIT_FAILURE == cmtwav_open ( g_cmtwav_extension, h ) ) {
        ui_show_error ( "CMTWAV: Can't create cmt stream\n" );
        generic_driver_close ( h );
        return EXIT_FAILURE;
    };

    generic_driver_close ( h );

    cmtext_container_set_name ( g_cmtwav_extension->container, ui_utils_basename ( filename ) );

    return EXIT_SUCCESS;
}


void cmtwav_exit ( void ) {
    cmtext_destroy ( g_cmtwav_extension );
}


void cmtwav_init ( void ) {
    g_cmtwav_extension = cmtext_new ( cmtwav_extension_open, cmtwav_extension_eject, CMT_CONTAINER_TYPE_SINGLE );
}
