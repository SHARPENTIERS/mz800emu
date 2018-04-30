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
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include "ui/ui_main.h"

#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "libs/wav/wav.h"
#include "libs/cmt_stream/cmt_stream.h"

#include "cmt.h"
#include "cmt_extension.h"
#include "cmt_wav.h"
#include "ui/ui_utils.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

st_CMT_WAV g_cmt_wav;
st_CMT_EXTENSION g_cmt_wav_extension;


static en_CMT_EXTENSION_NAME cmt_wav_get_name ( void ) {
    return CMT_EXTENSION_NAME_WAV;
}


static st_CMT_STREAM* cmt_wav_get_stream ( void ) {
    return g_cmt_wav.stream;
}


static char* cmt_wav_get_playfile_name ( void ) {
    return g_cmt_wav.playfile_name;
}


static void cmt_wav_eject ( void ) {
    if ( g_cmt_wav.stream ) cmt_stream_destroy ( g_cmt_wav.stream );
    g_cmt_wav.stream = NULL;
    if ( g_cmt_wav.playfile_name ) ui_utils_mem_free ( g_cmt_wav.playfile_name );
    g_cmt_wav.playfile_name = NULL;
}


static int cmt_wav_open ( char *filename ) {

    cmt_wav_eject ( );

    printf ( "Open WAV: %s\n", filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "CMT WAV: can't open WAV file '%s' - %s, gdriver_err: %s\n", filename, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        return EXIT_FAILURE;
    };

    g_cmt_wav.stream = cmt_stream_new_from_wav ( h );
    st_CMT_STREAM *cmt_stream = g_cmt_wav.stream;

    if ( !cmt_stream ) {
        ui_show_error ( "CMT WAV: Can't create cmt_stream from file '%s', gdriver_err: %s\n", filename, generic_driver_error_message ( h, h->driver ) );
        generic_driver_close ( h );
        return EXIT_FAILURE;
    }

    generic_driver_close ( h );

    g_cmt_wav.playfile_name = ui_utils_mem_alloc ( strlen ( basename ( filename ) ) + 1 );

    if ( !g_cmt_wav.playfile_name ) {
        ui_show_error ( "CMT WAV: Can't alocate memory: %s\n", strerror ( errno ) );
        cmt_wav_eject ( );
        return EXIT_FAILURE;
    };

    strcpy ( g_cmt_wav.playfile_name, basename ( filename ) );

    printf ( "WAV rate: %d\n", cmt_stream->rate );
    printf ( "WAV length: %1.2f s\n", ( cmt_stream->scan_time * cmt_stream->scans ) );

    return EXIT_SUCCESS;
}


void cmt_wav_init ( void ) {
    g_cmt_wav.stream = NULL;
    g_cmt_wav.playfile_name = NULL;

    memset ( &g_cmt_wav_extension, 0x00, sizeof (g_cmt_wav_extension ) );
    g_cmt_wav_extension.get_name = cmt_wav_get_name;
    g_cmt_wav_extension.get_stream = cmt_wav_get_stream;
    g_cmt_wav_extension.open = cmt_wav_open;
    g_cmt_wav_extension.eject = cmt_wav_eject;
    g_cmt_wav_extension.get_playfile_name = cmt_wav_get_playfile_name;
}


void cmt_wav_exit ( void ) {
    cmt_wav_eject ( );
}

