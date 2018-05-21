/* 
 * File:   cmt_mzf.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. května 2018, 16:55
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
#include <assert.h>
#include <math.h>

#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "libs/mztape/mztape.h"
#include "libs/cmt_stream/cmt_stream.h"
#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#include "cmt.h"
#include "cmtext.h"
#include "cmt_mzf.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

char *g_cmt_mzf_fileext[] = {
                             "mzf",
                             "m12",
                             NULL
};

st_CMTEXT_INFO g_cmt_mzf_info = {
                                 "MZF",
                                 g_cmt_mzf_fileext,
                                 "MZF cmt extension"
};

extern st_CMTEXT_NEW *g_cmt_mzf;


st_MZF_HEADER* cmtmzf_block_get_mzfheader ( st_CMTEXT_BLOCK *block ) {
    assert ( block != NULL );
    assert ( block->spec != NULL );
    st_CMTMZF_BLOCKSPEC *blspec = (st_CMTMZF_BLOCKSPEC*) block->spec;
    return blspec->hdr;
}


void cmtmzf_blockspec_destroy ( st_CMTMZF_BLOCKSPEC *blspec ) {
    if ( !blspec ) return;
    if ( blspec->hdr ) ui_utils_mem_free ( blspec->hdr );
    mztape_mztmzf_destroy ( blspec->mztmzf );
}


st_CMTMZF_BLOCKSPEC* cmtmzf_blockspec_new ( st_HANDLER *h, uint32_t offset, en_MZTAPE_SPEED mztape_speed ) {

    st_MZF_HEADER *hdr = (st_MZF_HEADER*) ui_utils_mem_alloc0 ( sizeof ( st_MZF_HEADER ) );
    if ( !hdr ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, (int) sizeof ( st_MZF_HEADER ) );
        return NULL;
    };

    if ( EXIT_SUCCESS == mzf_read_header_on_offset ( h, offset, hdr ) ) {
        char ascii_filename[MZF_FNAME_FULL_LENGTH];
        mzf_tools_get_fname ( hdr, ascii_filename );
        printf ( "%s fname: %s\n", cmtext_get_name ( g_cmt_mzf ), ascii_filename );
        printf ( "%s ftype: 0x%02x\n", cmtext_get_name ( g_cmt_mzf ), hdr->ftype );
        printf ( "%s fstrt: 0x%04x\n", cmtext_get_name ( g_cmt_mzf ), hdr->fstrt );
        printf ( "%s fsize: 0x%04x\n", cmtext_get_name ( g_cmt_mzf ), hdr->fsize );
        printf ( "%s fexec: 0x%04x\n", cmtext_get_name ( g_cmt_mzf ), hdr->fexec );
    } else {
        fprintf ( stderr, "%s():%d - Can't read MZF header\n", __func__, __LINE__ );
        ui_utils_mem_free ( hdr );
        return NULL;
    };

    st_MZTAPE_MZF *mztmzf = mztape_create_mztapemzf ( h, offset );
    if ( !mztmzf ) {
        fprintf ( stderr, "%s() - %d: Can't create mztmzf\n", __func__, __LINE__ );
        ui_utils_mem_free ( hdr );
        return NULL;
    };

    st_CMTMZF_BLOCKSPEC *blspec = (st_CMTMZF_BLOCKSPEC*) ui_utils_mem_alloc0 ( sizeof ( st_CMTMZF_BLOCKSPEC ) );
    if ( !blspec ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, (int) sizeof ( st_CMTMZF_BLOCKSPEC ) );
        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
        return NULL;
    };

    blspec->hdr = hdr;
    blspec->mztmzf = mztmzf;
    blspec->mztape_speed = mztape_speed;

    return blspec;
}


void cmtmzf_block_close ( st_CMTEXT_BLOCK *block ) {
    if ( !block ) return;
    cmtmzf_blockspec_destroy ( (st_CMTMZF_BLOCKSPEC*) block->spec );
    block->spec = (void*) NULL;
    cmtext_block_destroy ( block );
}


static void cmtmzf_container_close ( st_CMTEXT_CONTAINER *container ) {
    cmtext_container_destroy ( container );
}


static void cmtmzf_eject ( void ) {
    cmtmzf_block_close ( g_cmt_mzf->block );
    g_cmt_mzf->block = (st_CMTEXT_BLOCK*) NULL;
    cmtmzf_container_close ( g_cmt_mzf->container );
    g_cmt_mzf->container = (st_CMTEXT_CONTAINER*) NULL;
}


st_CMT_BITSTREAM* cmtmzf_generate_bitstream ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_SPEED mztape_speed ) {

    st_CMT_BITSTREAM *bitstream = mztape_create_cmt_bitstream_from_mztmzf ( mztmzf, MZTAPE_FORMATSET_MZ800_SANE, mztape_speed, CMTMZF_DEFAULT_BITSTREAM_RATE );
    if ( !bitstream ) {
        fprintf ( stderr, "%s():%d - Can't create bitstream\n", __func__, __LINE__ );
        return NULL;
    };

    printf ( "%s rate: %d\n", cmtext_get_name ( g_cmt_mzf ), bitstream->rate );
    printf ( "%s speed: %d Bd\n", cmtext_get_name ( g_cmt_mzf ), (int) round ( MZTAPE_DEFAULT_BDSPEED * g_speed_divisor[mztape_speed] ) );
    printf ( "%s length: %1.2f s\n", cmtext_get_name ( g_cmt_mzf ), bitstream->stream_length );
    printf ( "%s bitstream size: %0.2f kB\n", cmtext_get_name ( g_cmt_mzf ), (float) ( bitstream->scans / CMT_BITSTREAM_BLOCK_SIZE ) / (float) 1024 );
    //printf ( "DEBUG bitstream scans: %u\n", cmt_bitstream->scans );

    return bitstream;
}


st_CMT_VSTREAM* cmtmzf_generate_vstream ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_SPEED mztape_speed ) {

    st_CMT_VSTREAM *vstream = mztape_create_17MHz_cmt_vstream_from_mztmzf ( mztmzf, MZTAPE_FORMATSET_MZ800_SANE, mztape_speed );
    if ( !vstream ) {
        fprintf ( stderr, "%s() - %d: Can't create vstream\n", __func__, __LINE__ );
        return NULL;
    };

    printf ( "%s rate: %d Hz\n", cmtext_get_name ( g_cmt_mzf ), vstream->rate );
    printf ( "%s speed: %d Bd\n", cmtext_get_name ( g_cmt_mzf ), (int) round ( MZTAPE_DEFAULT_BDSPEED * g_speed_divisor[mztape_speed] ) );
    printf ( "%s length: %1.2f s\n", cmtext_get_name ( g_cmt_mzf ), vstream->stream_length );
    printf ( "%s vstream size: %0.2f kB\n", cmtext_get_name ( g_cmt_mzf ), (float) ( vstream->size / (float) 1024 ) );

    return vstream;
}


static int cmtmzf_set_speed ( void *cmtext, en_MZTAPE_SPEED mztape_speed ) {
    assert ( cmtext != NULL );
    st_CMTEXT_NEW *cext = (st_CMTEXT_NEW*) cmtext;
    st_CMTEXT_BLOCK *block = cext->block;
    assert ( block != NULL );
    if ( block->block_speed != CMTEXT_BLOCK_SPEED_DEFAULT ) return EXIT_FAILURE;
    st_CMTMZF_BLOCKSPEC *blspec = (st_CMTMZF_BLOCKSPEC*) block->spec;
    if ( blspec->mztape_speed == mztape_speed ) return EXIT_SUCCESS;

    assert ( block->stream != NULL );

    st_CMT_BITSTREAM *bitstream = NULL;
    st_CMT_VSTREAM *vstream = NULL;

    switch ( block->stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            bitstream = cmtmzf_generate_bitstream ( blspec->mztmzf, mztape_speed );
            if ( !bitstream ) return EXIT_FAILURE;
            cmt_bitstream_destroy ( block->stream->str.bitstream );
            block->stream->str.bitstream = bitstream;
            break;

        case CMT_STREAM_TYPE_VSTREAM:
            vstream = cmtmzf_generate_vstream ( blspec->mztmzf, mztape_speed );
            if ( !vstream ) return EXIT_FAILURE;
            cmt_vstream_destroy ( block->stream->str.vstream );
            block->stream->str.vstream = vstream;
            break;

        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, block->stream->stream_type );
            return EXIT_FAILURE;
    };

    blspec->mztape_speed = mztape_speed;
    return EXIT_SUCCESS;
}


static uint16_t cmtmzf_get_bdspeed ( void *cmtext ) {
    assert ( cmtext != NULL );
    st_CMTEXT_NEW *cext = (st_CMTEXT_NEW*) cmtext;
    st_CMTMZF_BLOCKSPEC *blspec = (st_CMTMZF_BLOCKSPEC*) cext->block->spec;
    assert ( blspec != NULL );
    return (uint16_t) round ( MZTAPE_DEFAULT_BDSPEED * g_speed_divisor[blspec->mztape_speed] );
}


//#define CMTMZF_DEFAULT_STREAM_BITSTREAM
#define CMTMZF_DEFAULT_STREAM_VSTREAM


st_CMTEXT_BLOCK* cmtmzf_block_open ( st_HANDLER *h, uint32_t offset, int block_id, int pause_after, en_CMTEXT_BLOCK_SPEED block_speed, en_MZTAPE_SPEED mztape_speed ) {

    printf ( "%s block id: %d\n", cmtext_get_name ( g_cmt_mzf ), block_id );

    st_CMTMZF_BLOCKSPEC *blspec = cmtmzf_blockspec_new ( h, offset, mztape_speed );
    if ( !blspec ) {
        return NULL;
    }

    st_CMT_STREAM *stream = (st_CMT_STREAM*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_STREAM ) );
    if ( !stream ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, (int) sizeof ( st_CMT_STREAM ) );
        cmtmzf_blockspec_destroy ( blspec );
        return NULL;
    };

#ifdef CMTMZF_DEFAULT_STREAM_BITSTREAM
    st_CMT_BITSTREAM *bitstream = cmtmzf_generate_bitstream ( blspec->mztmzf, mztape_speed );
    if ( !bitstream ) {
        fprintf ( stderr, "%s():%d - Can't generate bitstream\n", __func__, __LINE__ );
        cmtmzf_blockspec_destroy ( blspec );
        return NULL;
    };

    stream->stream_type = CMT_STREAM_TYPE_BITSTREAM;
    stream->str.bitstream = bitstream;
#endif

#ifdef CMTMZF_DEFAULT_STREAM_VSTREAM
    st_CMT_VSTREAM *vstream = cmtmzf_generate_vstream ( blspec->mztmzf, mztape_speed );
    if ( !vstream ) {
        fprintf ( stderr, "%s():%d - Can't generate vstream\n", __func__, __LINE__ );
        cmtmzf_blockspec_destroy ( blspec );
        cmt_stream_destroy ( stream );
        return NULL;
    };

    stream->str.vstream = vstream;
    stream->stream_type = CMT_STREAM_TYPE_VSTREAM;
#endif

    st_CMTEXT_BLOCK *block = cmtext_block_new ( block_id, CMTEXT_BLOCK_TYPE_MZF, stream, block_speed, pause_after, blspec );
    if ( !block ) {
        cmtmzf_blockspec_destroy ( blspec );
        cmt_stream_destroy ( stream );
    };

    block->cb_play = cmtext_block_play;
    block->cb_get_playname = cmtext_block_get_playname;
    block->cb_set_polarity = (cmtext_block_cb_set_polarity) NULL;
    block->cb_set_speed = cmtmzf_set_speed;
    block->cb_get_bdspeed = cmtmzf_get_bdspeed;

    return block;
}


static int cmtmzf_container_open ( char *filename ) {

    cmtmzf_eject ( );

    st_CMTEXT_CONTAINER *container = cmtext_container_new ( CMTEXT_CONTAINER_TYPE_SINGLE, ui_utils_basename ( filename ), 1, NULL, NULL, NULL, NULL );
    if ( !container ) {
        return EXIT_FAILURE;
    };

    printf ( "%s\nOpen: %s\n", cmtext_get_description ( g_cmt_mzf ), filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "%s: Can't open file '%s'\n", cmtext_get_description ( g_cmt_mzf ), filename );
        cmtmzf_container_close ( container );
        return EXIT_FAILURE;
    };

    st_CMTEXT_BLOCK *block = cmtmzf_block_open ( h, 0, 0, 0, CMTEXT_BLOCK_SPEED_DEFAULT, g_cmt.speed );
    if ( !block ) {
        ui_show_error ( "%s: Can't create cmt block\n", cmtext_get_description ( g_cmt_mzf ) );
        cmtmzf_container_close ( container );
        generic_driver_close ( h );
        return EXIT_FAILURE;
    };

    generic_driver_close ( h );

    g_cmt_mzf->container = container;
    g_cmt_mzf->block = block;

    return EXIT_SUCCESS;
}


static void cmtmzf_init ( void ) {
    return;
}


static void cmtmzf_exit ( void ) {
    cmtmzf_eject ( );
}


st_CMTEXT_NEW g_cmt_mzf_extension = {
                                     &g_cmt_mzf_info,
                                     (st_CMTEXT_CONTAINER*) NULL,
                                     (st_CMTEXT_BLOCK*) NULL,
                                     cmtmzf_init,
                                     cmtmzf_exit,
                                     cmtmzf_container_open,
                                     cmtmzf_eject,
};

st_CMTEXT_NEW *g_cmt_mzf = &g_cmt_mzf_extension;
