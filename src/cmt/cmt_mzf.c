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
#include <math.h>

#include "libs/cmt_stream/cmt_stream.h"
#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "libs/mztape/mztape.h"

#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#include "libs/wav/wav.h"

#include "cmt_extension.h"
#include "cmt_mzf.h"
#include "cmt.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

st_CMTEXT *g_cmtmzf_extension = NULL;


static void cmtmzf_extension_eject ( void ) {
    assert ( g_cmtmzf_extension != NULL );
    cmtext_eject ( g_cmtmzf_extension );
}


void cmtmzf_filespec_destroy ( st_CMT_MZF_FILESPEC *filespec ) {
    if ( !filespec ) return;
    if ( filespec->hdr ) ui_utils_mem_free ( filespec->hdr );
    if ( filespec->mztmzf ) ui_utils_mem_free ( filespec->mztmzf );
    ui_utils_mem_free ( filespec );
}


st_CMT_MZF_FILESPEC* cmtmzf_filespec_new ( st_MZF_HEADER *hdr, st_MZTAPE_MZF *mztmzf ) {
    assert ( hdr != NULL );
    assert ( mztmzf != NULL );
    st_CMT_MZF_FILESPEC *filespec = (st_CMT_MZF_FILESPEC*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_MZF_FILESPEC ) );
    if ( !filespec ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_CMT_MZF_FILESPEC ) );
        return NULL;
    };
    filespec->hdr = hdr;
    filespec->mztmzf = mztmzf;
    return filespec;
}


st_CMT_BITSTREAM* cmtmzf_generate_bitstream ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_SPEED mztape_speed ) {
    st_CMT_BITSTREAM *cmt_bitstream = mztape_create_cmt_bitstream_from_mztmzf ( mztmzf, MZTAPE_FORMATSET_MZ800_SANE, mztape_speed, CMTMZF_DEFAULT_STREAM_RATE );
    if ( !cmt_bitstream ) {
        fprintf ( stderr, "%s() - %d: Can't create bitstream\n", __func__, __LINE__ );
        return NULL;
    };

    //printf ( "CMTMZF rate: %d\n", cmt_bitstream->rate );
    printf ( "speed: %d Bd\n", (int) round ( MZTAPE_DEFAULT_BDSPEED * g_speed_divisor[mztape_speed] ) );
    printf ( "length: %1.2f s\n", ( cmt_bitstream->scan_time * cmt_bitstream->scans ) );

    //printf ( "DEBUG bitstream size: %0.2f kB\n", ( cmt_bitstream->blocks * ( CMT_BITSTREAM_BLOCK_SIZE / 8 ) ) / (float) 1024 );
    //printf ( "DEBUG bitstream scans: %u\n", cmt_bitstream->scans );

    return cmt_bitstream;
}


st_CMT_VSTREAM* cmtmzf_generate_vstream ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_SPEED mztape_speed ) {
    st_CMT_VSTREAM *cmt_vstream = mztape_create_17MHz_cmt_vstream_from_mztmzf ( mztmzf, MZTAPE_FORMATSET_MZ800_SANE, mztape_speed );
    if ( !cmt_vstream ) {
        fprintf ( stderr, "%s() - %d: Can't create vstream\n", __func__, __LINE__ );
        return NULL;
    };

    printf ( "speed: %d Bd\n", (int) round ( MZTAPE_DEFAULT_BDSPEED * g_speed_divisor[mztape_speed] ) );
    printf ( "length: %1.2f s\n", cmt_vstream->stream_length );
    //printf ( "DEBUG vstream size: %0.2f kB\n", ( cmt_vstream->size / (float) 1024 ) );

    return cmt_vstream;
}

//#define CMTMZF_DEFAULT_STREAM_BITSTREAM
#define CMTMZF_DEFAULT_STREAM_VSTREAM


int cmtmzf_open ( st_CMTEXT *cmtext, st_HANDLER *h, en_MZTAPE_SPEED mztape_speed ) {

    assert ( cmtext != NULL );

    st_MZF_HEADER *hdr = (st_MZF_HEADER*) ui_utils_mem_alloc0 ( sizeof ( st_MZF_HEADER ) );
    if ( !hdr ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_MZF_HEADER ) );
        return EXIT_FAILURE;
    };

    if ( EXIT_SUCCESS == mzf_read_header ( h, hdr ) ) {
        char ascii_filename[MZF_FNAME_FULL_LENGTH];
        mzf_tools_get_fname ( hdr, ascii_filename );
        printf ( "fname: %s\n", ascii_filename );
        printf ( "ftype: 0x%02x\n", hdr->ftype );
        printf ( "fstrt: 0x%04x\n", hdr->fstrt );
        printf ( "fsize: 0x%04x\n", hdr->fsize );
        printf ( "fexec: 0x%04x\n", hdr->fexec );
    } else {
        fprintf ( stderr, "%s() - %d: Can't read MZF header\n", __func__, __LINE__ );
        ui_utils_mem_free ( hdr );
        return EXIT_FAILURE;
    };

    st_MZTAPE_MZF *mztmzf = mztape_create_mztmzf ( h );
    if ( !mztmzf ) {
        fprintf ( stderr, "%s() - %d: Can't create mztmzf\n", __func__, __LINE__ );
        ui_utils_mem_free ( hdr );
        return EXIT_FAILURE;
    };

#ifdef CMTMZF_DEFAULT_STREAM_BITSTREAM
    st_CMT_BITSTREAM *cmt_bitstream = cmtmzf_generate_bitstream ( mztmzf, mztape_speed );
    if ( !cmt_bitstream ) {
        fprintf ( stderr, "%s() - %d: Can't generate bitstream\n", __func__, __LINE__ );
        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
        return EXIT_FAILURE;
    };

    st_CMT_FILE *cmtfile = cmtext_cmtfile_new ( CMT_STREAM_TYPE_BITSTREAM );
    if ( !cmtfile ) {
        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
        cmt_bitstream_destroy ( cmt_bitstream );
        fprintf ( stderr, "%s() - %d: Can't create cmtfile\n", __func__, __LINE__ );
        return EXIT_FAILURE;
    }
    cmtfile->stream.str.bitstream = cmt_bitstream;
#endif

#ifdef CMTMZF_DEFAULT_STREAM_VSTREAM
    st_CMT_VSTREAM *cmt_vstream = cmtmzf_generate_vstream ( mztmzf, mztape_speed );
    if ( !cmt_vstream ) {
        fprintf ( stderr, "%s() - %d: Can't generate vstream\n", __func__, __LINE__ );
        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
        return EXIT_FAILURE;
    };

    st_CMT_FILE *cmtfile = cmtext_cmtfile_new ( CMT_STREAM_TYPE_VSTREAM );
    if ( !cmtfile ) {
        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
        cmt_vstream_destroy ( cmt_vstream );
        fprintf ( stderr, "%s() - %d: Can't create cmtfile\n", __func__, __LINE__ );
        return EXIT_FAILURE;
    }
    cmtfile->stream.str.vstream = cmt_vstream;
#endif

    cmtfile->type = CMT_FILETYPE_MZF;

    cmtfile->spec = cmtmzf_filespec_new ( hdr, mztmzf );
    if ( !cmtfile->spec ) {
        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
#ifdef CMTMZF_DEFAULT_STREAM_BITSTREAM
        cmt_bitstream_destroy ( cmt_bitstream );
#endif
#ifdef CMTMZF_DEFAULT_STREAM_VSTREAM
        cmt_vstream_destroy ( cmt_vstream );
#endif
        fprintf ( stderr, "%s() - %d: Can't create MZF filespec\n", __func__, __LINE__ );
        return EXIT_FAILURE;
    };

    g_cmtmzf_extension->cmtfile = cmtfile;

    return EXIT_SUCCESS;
}

#if 0
static st_DRIVER *g_driver_realoc = &g_ui_memory_driver_realloc;


static void cmtmzf_save_wav ( char *filename, st_CMT_BITSTREAM *cmt_bitstream ) {

    st_HANDLER *h = generic_driver_open_memory ( NULL, g_driver_realoc, 1 );
    generic_driver_set_handler_readonly_status ( h, 0 );
    h->spec.memspec.swelling_enabled = 1;

    if ( EXIT_SUCCESS != cmt_bitstream_create_wav ( h, cmt_bitstream ) ) {
        fprintf ( stderr, "Error: can't create wav from cmt stream!\n" );
        generic_driver_close ( h );
        return;
    };

    printf ( "Save WAV: %s\n", filename );

    if ( EXIT_SUCCESS != generic_driver_save_memory ( h, filename ) ) {
        fprintf ( stderr, "%s():%d - Could not write file '%s'\n", __func__, __LINE__, filename );
        generic_driver_close ( h );
        return;
    };

    generic_driver_close ( h );
}

#endif


static int cmtmzf_extension_open ( char * filename ) {

    assert ( g_cmtmzf_extension != NULL );

    cmtmzf_extension_eject ( );

    printf ( "CMTMZF Open: %s\n", filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "CMTMZF: can't open MZF file '%s'\n", filename );
        return EXIT_FAILURE;
    };

    if ( EXIT_FAILURE == cmtmzf_open ( g_cmtmzf_extension, h, g_cmt.speed ) ) {
        generic_driver_close ( h );
        return EXIT_FAILURE;
    };

    generic_driver_close ( h );

    cmtext_container_set_name ( g_cmtmzf_extension->container, ui_utils_basename ( filename ) );

    //cmtmzf_save_wav ( "pokusny.wav", g_cmtmzf_extension->cmtfile->stream.str.bitstream );

    return EXIT_SUCCESS;
}


void cmtmzf_exit ( void ) {
    cmtext_destroy ( g_cmtmzf_extension );
}


void cmtmzf_init ( void ) {
    g_cmtmzf_extension = cmtext_new ( cmtmzf_extension_open, cmtmzf_extension_eject, CMT_CONTAINER_TYPE_SINGLE );
}



