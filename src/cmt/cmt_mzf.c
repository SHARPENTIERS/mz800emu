/* 
 * File:   cmt_mzf.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 24. dubna 2018, 12:19
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
#include "ui/ui_utils.h"

#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"

#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "libs/mztape/mztape.h"

#include "cmt.h"
#include "cmt_extension.h"
#include "cmt_mzf.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

st_CMT_MZF g_cmt_mzf;
st_CMT_EXTENSION g_cmt_mzf_extension;


static en_CMT_EXTENSION_NAME cmt_mzf_get_name ( void ) {
    return CMT_EXTENSION_NAME_MZF;
}


static st_CMT_STREAM* cmt_mzf_get_stream ( void ) {
    return g_cmt_mzf.stream;
}


static char* cmt_mzf_get_playfile_name ( void ) {
    return g_cmt_mzf.playfile_name;
}


static void cmt_mzf_eject ( void ) {
    if ( g_cmt_mzf.stream ) cmt_stream_destroy ( g_cmt_mzf.stream );
    g_cmt_mzf.stream = NULL;
    if ( g_cmt_mzf.mztmzf ) ui_utils_mem_free ( g_cmt_mzf.mztmzf );
    g_cmt_mzf.mztmzf = NULL;
    if ( g_cmt_mzf.playfile_name ) ui_utils_mem_free ( g_cmt_mzf.playfile_name );
    g_cmt_mzf.playfile_name = NULL;
}


static const char* cmt_mzf_get_speed_name ( en_CMT_SPEED speed ) {
    switch ( speed ) {
        case CMT_SPEED_1200:
            return "1200 bps";
        case CMT_SPEED_2400:
            return "2400 bps";
        case CMT_SPEED_3600:
            return "3600 bps";
    };
    return "Unknown Speed!";
}


static en_MZTAPE_SPEED cmt_mzf_get_mztape_speed ( en_CMT_SPEED speed ) {
    switch ( speed ) {
        case CMT_SPEED_1200:
            return MZTAPE_SPEED_1_1;
        case CMT_SPEED_2400:
            return MZTAPE_SPEED_2_1;
        case CMT_SPEED_3600:
            return MZTAPE_SPEED_3_1;
    };
    return MZTAPE_SPEED_1_1;
}


static int cmt_mzf_change_speed ( en_CMT_SPEED speed ) {

    st_CMT_STREAM *stream = mztape_create_cmt_stream_from_mztmzf ( g_cmt_mzf.mztmzf, MZTAPE_FORMATSET_MZ800_SANE, cmt_mzf_get_mztape_speed ( speed ), CMT_MZF_DEFAULT_MZTAPE_RATE );

    if ( !stream ) {
        ui_show_error ( "CMT MZF: can't create cmt stream\n" );
        return EXIT_FAILURE;
    };

    cmt_stream_destroy ( g_cmt_mzf.stream );
    g_cmt_mzf.stream = stream;

    printf ( "MZF (%s) length: %1.2f s\n", cmt_mzf_get_speed_name ( speed ), ( g_cmt_mzf.stream->scan_time * g_cmt_mzf.stream->scans ) );

    return EXIT_SUCCESS;
}


static int cmt_mzf_open ( char *filename ) {

    cmt_mzf_eject ( );

    printf ( "Open MZF: %s\n", filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "CMT MZF: can't open MZF file '%s' - %s, gdriver_err: %s\n", filename, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        return EXIT_FAILURE;
    };


    if ( EXIT_SUCCESS == mzf_read_header ( h, &g_cmt_mzf.hdr ) ) {
        char ascii_filename[MZF_FNAME_FULL_LENGTH];
        mzf_tools_get_fname ( &g_cmt_mzf.hdr, ascii_filename );
        printf ( "fname: %s\n", ascii_filename );
        printf ( "ftype: 0x%02x\n", g_cmt_mzf.hdr.ftype );
        printf ( "fstrt: 0x%04x\n", g_cmt_mzf.hdr.fstrt );
        printf ( "fsize: 0x%04x\n", g_cmt_mzf.hdr.fsize );
        printf ( "fexec: 0x%04x\n", g_cmt_mzf.hdr.fexec );
    } else {
        ui_show_error ( "CMT MZF can't read MZF header '%s': %s, gdriver_err: %s\n", filename, strerror ( errno ), mzf_error_message ( h, h->driver ) );
        generic_driver_close ( h );
        return EXIT_FAILURE;
    };

    g_cmt_mzf.mztmzf = mztape_create_mztmzf ( h );

    if ( !g_cmt_mzf.mztmzf ) {
        ui_show_error ( "CMT MZF: can't create mztape mzf - %s, gdriver_err: %s\n", strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        generic_driver_close ( h );
        cmt_mzf_eject ( );
        return EXIT_FAILURE;
    };

    generic_driver_close ( h );

    g_cmt_mzf.stream = mztape_create_cmt_stream_from_mztmzf ( g_cmt_mzf.mztmzf, MZTAPE_FORMATSET_MZ800_SANE, cmt_mzf_get_mztape_speed ( g_cmt.speed ), CMT_MZF_DEFAULT_MZTAPE_RATE );

    if ( !g_cmt_mzf.stream ) {
        ui_show_error ( "CMT MZF: can't create cmt stream\n" );
        cmt_mzf_eject ( );
        return EXIT_FAILURE;
    };

    g_cmt_mzf.playfile_name = ui_utils_mem_alloc ( strlen ( basename ( filename ) ) + 1 );

    if ( !g_cmt_mzf.playfile_name ) {
        ui_show_error ( "CMT WAV: Can't alocate memory: %s\n", strerror ( errno ) );
        cmt_mzf_eject ( );
        return EXIT_FAILURE;
    };

    strcpy ( g_cmt_mzf.playfile_name, basename ( filename ) );

    printf ( "MZF (%s) length: %1.2f s\n", cmt_mzf_get_speed_name ( g_cmt.speed ), ( g_cmt_mzf.stream->scan_time * g_cmt_mzf.stream->scans ) );

    return EXIT_SUCCESS;
}


static st_MZF_HEADER* cmt_mzf_get_mzfheader ( void ) {
    return &g_cmt_mzf.hdr;
}


void cmt_mzf_init ( void ) {
    g_cmt_mzf.stream = NULL;
    g_cmt_mzf.mztmzf = NULL;
    g_cmt_mzf.playfile_name = NULL;

    memset ( &g_cmt_mzf_extension, 0x00, sizeof (g_cmt_mzf_extension ) );
    g_cmt_mzf_extension.get_name = cmt_mzf_get_name;
    g_cmt_mzf_extension.get_stream = cmt_mzf_get_stream;
    g_cmt_mzf_extension.open = cmt_mzf_open;
    g_cmt_mzf_extension.eject = cmt_mzf_eject;
    g_cmt_mzf_extension.get_mzfheader = cmt_mzf_get_mzfheader;
    g_cmt_mzf_extension.change_speed = cmt_mzf_change_speed;
    g_cmt_mzf_extension.get_playfile_name = cmt_mzf_get_playfile_name;
}


void cmt_mzf_exit ( void ) {
    cmt_mzf_eject ( );
}



#if 0
static st_DRIVER *g_driver_realoc = &g_ui_memory_driver_realloc;


static void cmt_mzf_save_wav ( char *filename, st_CMT_STREAM *cmt_stream ) {

    st_HANDLER *h = generic_driver_open_memory ( NULL, g_driver_realoc, 1 );
    generic_driver_set_handler_readonly_status ( h, 0 );
    h->spec.memspec.swelling_enabled = 1;

    if ( EXIT_SUCCESS != cmt_stream_create_wav ( h, cmt_stream ) ) {
        fprintf ( stderr, "Error: can't create wav from cmt stream!\n" );
        generic_driver_close ( h );
        return;
    };

    printf ( "Save WAV: %s\n", filename );

    if ( EXIT_SUCCESS != generic_driver_save_memory ( h, filename ) ) {
        fprintf ( stderr, "%s():%d - Could not open file '%s': %s\n", __func__, __LINE__, filename, strerror ( errno ) );
        generic_driver_close ( h );
        return;
    };

    generic_driver_close ( h );
}
#endif
