/* 
 * File:   cmt_mzftape.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. května 2018, 7:25
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

#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#include "cmt.h"
#include "cmtext.h"
#include "cmt_mzf.h"
#include "cmt_mzftape.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

char *g_cmt_mzftape_fileext[] = {
                                 "mzt",
                                 NULL
};

st_CMTEXT_INFO g_cmt_mzftape_info = {
                                     "MZT",
                                     g_cmt_mzftape_fileext,
                                     "MZT cmt extension"
};

extern st_CMTEXT *g_cmt_mzftape;


static void cmtmzftape_container_close ( st_CMTEXT_CONTAINER *container ) {
    cmtext_container_destroy ( container );
}


static void cmtmzftape_eject ( void ) {
    cmtmzf_block_close ( g_cmt_mzftape->block );
    g_cmt_mzftape->block = (st_CMTEXT_BLOCK*) NULL;
    cmtmzftape_container_close ( g_cmt_mzftape->container );
    g_cmt_mzftape->container = (st_CMTEXT_CONTAINER*) NULL;
}


static st_CMTEXT_TAPE_INDEX* cmtmzftape_container_index_new ( st_HANDLER *h, uint32_t offset, en_MZTAPE_SPEED mztape_speed, int *count_blocks ) {

    *count_blocks = 0;

    st_MZF_HEADER *hdr = (st_MZF_HEADER*) ui_utils_mem_alloc0 ( sizeof ( st_MZF_HEADER ) );
    if ( !hdr ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, (int) sizeof ( st_MZF_HEADER ) );
        return NULL;
    };

    st_CMTEXT_TAPE_INDEX *index = NULL;
    int i = 0;

    while ( offset < h->spec.memspec.size ) {

        if ( EXIT_FAILURE == mzf_read_header_on_offset ( h, offset, hdr ) ) {
            fprintf ( stderr, "%s():%d - Can't read MZF header\n", __func__, __LINE__ );
            ui_utils_mem_free ( hdr );
            cmtext_container_tapeindex_destroy ( index, i );
            return NULL;
        };

        char ascii_filename[MZF_FNAME_FULL_LENGTH];
        mzf_tools_get_fname ( hdr, ascii_filename );

        index = cmtext_container_tape_index_aloc ( index, i );
        if ( !index ) {
            ui_utils_mem_free ( hdr );
            return NULL;
        };

        st_CMTEXT_TAPE_INDEX *idx = &index[i];

        idx->block_id = i++;
        idx->offset = offset;
        idx->blspeed = CMTEXT_BLOCK_SPEED_DEFAULT;
        idx->bltype = CMTEXT_BLOCK_TYPE_MZF;
        idx->pause_after = CMTMZFTAPE_DEFAULT_PAUSE_AFTER;

        st_CMTEXT_TAPE_ITEM_MZF *mzfitem = &idx->item.mzf;

        mzfitem->mztape_speed = mztape_speed;
        mzfitem->ftype = hdr->ftype;
        mzfitem->fsize = hdr->fsize;
        mzfitem->fstrt = hdr->fstrt;
        mzfitem->fexec = hdr->fexec;

        int name_length = strlen ( ascii_filename ) + 1;
        mzfitem->fname = (char*) ui_utils_mem_alloc0 ( name_length );
        if ( !mzfitem->fname ) {
            fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, name_length );
            ui_utils_mem_free ( hdr );
            cmtext_container_tapeindex_destroy ( index, i );
            return NULL;
        };

        strncpy ( mzfitem->fname, ascii_filename, name_length );

        offset += sizeof ( st_MZF_HEADER ) + hdr->fsize;
    };

    if ( i != 0 ) {
        st_CMTEXT_TAPE_INDEX *idx = &index[i - 1];
        idx->pause_after = 0;
    };

    ui_utils_mem_free ( hdr );

    *count_blocks = i;
    return index;
}


st_CMTEXT_BLOCK* cmtmzftape_block_open ( int block_id ) {

    assert ( g_cmt_mzftape->container != NULL );
    assert ( g_cmt_mzftape->container->tape != NULL );
    assert ( g_cmt_mzftape->container->tape->h != NULL );
    assert ( g_cmt_mzftape->container->tape->index != NULL );

    if ( ( block_id < 0 ) || ( block_id >= g_cmt_mzftape->container->count_blocks ) ) {
        ui_show_error ( "%s: Can't open block '%d' (blocks = %d)\n", cmtext_get_description ( g_cmt_mzftape ), block_id, g_cmt_mzftape->container->count_blocks );
        return NULL;
    };

    st_CMTEXT_TAPE_INDEX *idx = &g_cmt_mzftape->container->tape->index[block_id];
    uint32_t offset = idx->offset;
    en_CMTEXT_BLOCK_SPEED blspeed = idx->blspeed;
    st_CMTEXT_TAPE_ITEM_MZF *mzfitem = &idx->item.mzf;
    en_MZTAPE_SPEED mztape_speed = ( blspeed == CMTEXT_BLOCK_SPEED_SET ) ? mzfitem->mztape_speed : g_cmt.speed; // pokud neni SET, tak je DEFAULT
    uint16_t pause_after = idx->pause_after;

    st_CMTEXT_BLOCK *block = cmtmzf_block_open ( g_cmt_mzftape->container->tape->h, offset, block_id, pause_after, blspeed, mztape_speed );
    if ( !block ) {
        return NULL;
    };

    return block;
}


static int cmtmzftape_container_next_block ( void ) {
    assert ( g_cmt_mzftape->container != NULL );
    assert ( g_cmt_mzftape->block != NULL );
    int next_block_id = g_cmt_mzftape->block->block_id + 1;
    if ( next_block_id < g_cmt_mzftape->container->count_blocks ) {
        st_CMTEXT_BLOCK *block = cmtmzftape_block_open ( next_block_id );
        if ( !block ) return EXIT_FAILURE;
        cmtmzf_block_close ( g_cmt_mzftape->block );
        g_cmt_mzftape->block = block;
        return EXIT_SUCCESS;
    };
    return EXIT_FAILURE;
}


static int cmtmzftape_container_previous_block ( void ) {
    assert ( g_cmt_mzftape->container != NULL );
    assert ( g_cmt_mzftape->block != NULL );
    if ( g_cmt_mzftape->block->block_id == 0 ) return EXIT_FAILURE;
    int prev_block_id = g_cmt_mzftape->block->block_id - 1;
    st_CMTEXT_BLOCK *block = cmtmzftape_block_open ( prev_block_id );
    if ( !block ) return EXIT_FAILURE;
    cmtmzf_block_close ( g_cmt_mzftape->block );
    g_cmt_mzftape->block = block;
    return EXIT_SUCCESS;
}


static int cmtmzftape_container_open_block ( int block_id ) {
    return EXIT_FAILURE;
}


static int cmtmzftape_container_open ( char *filename ) {

    cmtmzftape_eject ( );

    printf ( "%s\nOpen: %s\n", cmtext_get_description ( g_cmt_mzftape ), filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "%s: Can't open file '%s'\n", cmtext_get_description ( g_cmt_mzftape ), filename );
        return EXIT_FAILURE;
    };

    int count_blocks = 0;

    st_CMTEXT_TAPE_INDEX *index = cmtmzftape_container_index_new ( h, 0, g_cmt.speed, &count_blocks );

    if ( !index ) {
        generic_driver_close ( h );
        return EXIT_FAILURE;
    };

    if ( !count_blocks ) {
        ui_show_error ( "%s: Empty container '%s'\n", cmtext_get_description ( g_cmt_mzftape ), filename );
        generic_driver_close ( h );
        cmtext_container_tapeindex_destroy ( index, count_blocks );
        return EXIT_FAILURE;
    };

    st_CMTEXT_CONTAINER_TAPE *tape = cmtext_container_tape_new ( h, index );
    if ( !tape ) {
        generic_driver_close ( h );
        cmtext_container_tapeindex_destroy ( index, count_blocks );
        return EXIT_FAILURE;
    };

    st_CMTEXT_CONTAINER *container = cmtext_container_new ( CMTEXT_CONTAINER_TYPE_SIMPLE_TAPE, ui_utils_basename ( filename ), count_blocks, tape, cmtmzftape_container_next_block, cmtmzftape_container_previous_block, cmtmzftape_container_open_block );
    if ( !container ) {
        cmtext_container_tape_destroy ( tape, count_blocks );
        return EXIT_FAILURE;
    };

    g_cmt_mzftape->container = container;

    st_CMTEXT_BLOCK *block = cmtmzftape_block_open ( 0 );
    if ( !block ) {
        ui_show_error ( "%s: Can't create cmt block\n", cmtext_get_description ( g_cmt_mzftape ) );
        cmtmzftape_container_close ( container );
        return EXIT_FAILURE;
    };

    g_cmt_mzftape->block = block;

    return EXIT_SUCCESS;
}


static void cmtmzftape_init ( void ) {
    return;
}


static void cmtmzftape_exit ( void ) {
    cmtmzftape_eject ( );
}


st_CMTEXT g_cmt_mzftape_extension = {
                                         &g_cmt_mzftape_info,
                                         (st_CMTEXT_CONTAINER*) NULL,
                                         (st_CMTEXT_BLOCK*) NULL,
                                         cmtmzftape_init,
                                         cmtmzftape_exit,
                                         cmtmzftape_container_open,
                                         cmtmzftape_eject,
};

st_CMTEXT *g_cmt_mzftape = &g_cmt_mzftape_extension;
