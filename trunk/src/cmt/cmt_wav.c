/* 
 * File:   cmt_wav.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. května 2018, 8:21
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

#include "libs/wav/wav.h"
#include "libs/cmt_stream/cmt_stream.h"
#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#include "cmt.h"
#include "cmtext.h"
#include "cmt_wav.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

char *g_cmt_wav_fileext[] = {
                             "wav",
                             NULL
};

st_CMTEXT_INFO g_cmt_wav_info = {
                                 "WAV",
                                 g_cmt_wav_fileext,
                                 "WAV cmt extension"
};

extern st_CMTEXT *g_cmt_wav;


void cmtwav_block_close ( st_CMTEXT_BLOCK *block ) {
    cmtext_block_destroy ( block );
}


static void cmtwav_container_close ( st_CMTEXT_CONTAINER *container ) {
    cmtext_container_destroy ( container );
}


static void cmtwav_eject ( void ) {
    cmtwav_block_close ( g_cmt_wav->block );
    g_cmt_wav->block = (st_CMTEXT_BLOCK*) NULL;
    cmtwav_container_close ( g_cmt_wav->container );
    g_cmt_wav->container = (st_CMTEXT_CONTAINER*) NULL;
}


st_CMTEXT_BLOCK* cmtwav_block_open ( st_HANDLER *h, uint32_t offset, int block_id, int pause_after ) {

    st_CMT_STREAM *stream = (st_CMT_STREAM*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_STREAM ) );
    if ( !stream ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, (int) sizeof ( st_CMT_STREAM ) );
        return NULL;
    };

    stream->stream_type = CMT_STREAM_TYPE_BITSTREAM;

    st_CMT_BITSTREAM *bitstream = cmt_bitstream_new_from_wav ( h, g_cmt.polarity );
    if ( !bitstream ) {
        fprintf ( stderr, "%s():%d - Can't create stream\n", __func__, __LINE__ );
        cmt_stream_destroy ( stream );
        return NULL;
    };

    stream->str.bitstream = bitstream;

    st_CMTEXT_BLOCK *block = cmtext_block_new ( block_id, CMTEXT_BLOCK_TYPE_WAV, stream, CMTEXT_BLOCK_SPEED_NONE, pause_after, NULL );
    if ( !block ) {
        cmt_stream_destroy ( stream );
    };

    printf ( "%s block id: %d\n", cmtext_get_name ( g_cmt_wav ), block_id );
    printf ( "%s bitstream size: %0.2f kB\n", cmtext_get_name ( g_cmt_wav ), (float) ( bitstream->scans / CMT_BITSTREAM_BLOCK_SIZE ) / (float) 1024 );
    printf ( "%s polarity: %s\n", cmtext_get_name ( g_cmt_wav ), ( g_cmt.polarity == CMT_STREAM_POLARITY_NORMAL ) ? "normal" : "inverted" );
    printf ( "%s rate: %d Hz\n", cmtext_get_name ( g_cmt_wav ), bitstream->rate );
    printf ( "%s length: %1.2f s\n", cmtext_get_name ( g_cmt_wav ), ( bitstream->scan_time * bitstream->scans ) );

    block->cb_play = cmtext_block_play;
    block->cb_get_playname = cmtext_block_get_playname;
    block->cb_set_polarity = cmtext_block_set_polarity;
    block->cb_set_speed = (cmtext_block_cb_set_speed) NULL;
    block->cb_get_bdspeed = (cmtext_block_cb_get_bdspeed) NULL;

    return block;
}


static int cmtwav_container_open ( char *filename ) {

    cmtwav_eject ( );

    st_CMTEXT_CONTAINER *container = cmtext_container_new ( CMTEXT_CONTAINER_TYPE_SINGLE, ui_utils_basename ( filename ), 1, NULL, NULL, NULL, NULL );
    if ( !container ) {
        return EXIT_FAILURE;
    };

    printf ( "%s\nOpen: %s\n", cmtext_get_description ( g_cmt_wav ), filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "%s: Can't open file '%s'\n", cmtext_get_description ( g_cmt_wav ), filename );
        cmtwav_container_close ( container );
        return EXIT_FAILURE;
    };

    st_CMTEXT_BLOCK *block = cmtwav_block_open ( h, 0, 0, 0 );
    if ( !block ) {
        ui_show_error ( "%s: Can't create cmt block\n", cmtext_get_description ( g_cmt_wav ) );
        cmtwav_container_close ( container );
        generic_driver_close ( h );
        return EXIT_FAILURE;
    };

    generic_driver_close ( h );

    g_cmt_wav->container = container;
    g_cmt_wav->block = block;

    return EXIT_SUCCESS;
}


static void cmtwav_init ( void ) {
    return;
}


static void cmtwav_exit ( void ) {
    cmtwav_eject ( );
}


st_CMTEXT g_cmt_wav_extension = {
                                     &g_cmt_wav_info,
                                     (st_CMTEXT_CONTAINER*) NULL,
                                     (st_CMTEXT_BLOCK*) NULL,
                                     cmtwav_init,
                                     cmtwav_exit,
                                     cmtwav_container_open,
                                     cmtwav_eject,
};

st_CMTEXT *g_cmt_wav = &g_cmt_wav_extension;
