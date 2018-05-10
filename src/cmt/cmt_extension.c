/* 
 * File:   cmt_extension.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 4. května 2018, 13:11
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
#include <stdint.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "ui/ui_utils.h"

//#include "libs/mzf/mzf.h"

#include "libs/cmt_stream/cmt_stream.h"
#include "libs/mztape/mztape.h"
#include "cmt_extension.h"
#include "cmt_mzf.h"


/*
 * 
 * cmtfile
 * 
 */


void cmtext_cmtfile_destroy ( st_CMT_FILE *cmtfile ) {
    if ( !cmtfile ) return;
    if ( ( cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM ) && ( cmtfile->stream.str.bitstream ) ) cmt_bitstream_destroy ( cmtfile->stream.str.bitstream );
    if ( ( cmtfile->stream.stream_type == CMT_STREAM_TYPE_VSTREAM ) && ( cmtfile->stream.str.vstream ) ) cmt_vstream_destroy ( cmtfile->stream.str.vstream );
    if ( cmtfile->type == CMT_FILETYPE_MZF ) cmtmzf_filespec_destroy ( (st_CMT_MZF_FILESPEC*) cmtfile->spec );
    ui_utils_mem_free ( cmtfile );
}


st_CMT_FILE* cmtext_cmtfile_new ( en_CMT_STREAM_TYPE stream_type ) {

    st_CMT_FILE *cmtfile = (st_CMT_FILE*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_FILE ) );
    if ( !cmtfile ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_CMT_FILE ) );
        return NULL;
    };

    cmtfile->stream.stream_type = stream_type;

    return cmtfile;
}


uint64_t cmtext_cmtfile_get_scans ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    if ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM ) {
        st_CMT_BITSTREAM *bitstream = cmtext->cmtfile->stream.str.bitstream;
        return bitstream->scans;
    };
    assert ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_VSTREAM );
    st_CMT_VSTREAM *vstream = cmtext->cmtfile->stream.str.vstream;
    return vstream->scans;
}


double cmtext_cmtfile_get_scantime ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    if ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM ) {
        st_CMT_BITSTREAM *bitstream = cmtext->cmtfile->stream.str.bitstream;
        return bitstream->scan_time;
    };
    assert ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_VSTREAM );
    st_CMT_VSTREAM *vstream = cmtext->cmtfile->stream.str.vstream;
    return vstream->scan_time;
}


uint32_t cmtext_cmtfile_get_rate ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    assert ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM );
    st_CMT_BITSTREAM *bitstream = cmtext->cmtfile->stream.str.bitstream;
    return bitstream->rate;
}


double cmtext_cmtfile_get_length ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    assert ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM );
    st_CMT_BITSTREAM *bitstream = cmtext->cmtfile->stream.str.bitstream;
    return ( bitstream->scan_time * bitstream->scans );
}


en_CMT_FILETYPE cmtext_cmtfile_get_filetype ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    return cmtext->cmtfile->type;
}


char* cmtext_cmtfile_get_playname ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->container != NULL );
    assert ( cmtext->container->type == CMT_CONTAINER_TYPE_SINGLE );
    return cmtext->container->name;
}


en_CMT_STREAM_TYPE cmtext_cmtfile_get_stream_type ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    return cmtext->cmtfile->stream.stream_type;
}


st_CMT_BITSTREAM* cmtext_cmtfile_get_bitstream ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    assert ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM );
    st_CMT_BITSTREAM *bitstream = cmtext->cmtfile->stream.str.bitstream;
    return bitstream;
}


st_CMT_VSTREAM* cmtext_cmtfile_get_vstream ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    assert ( cmtext->cmtfile->stream.stream_type == CMT_STREAM_TYPE_VSTREAM );
    st_CMT_VSTREAM *vstream = cmtext->cmtfile->stream.str.vstream;
    return vstream;
}


st_MZF_HEADER* cmtext_cmtfile_get_mzfheader ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    assert ( cmtext->cmtfile->type == CMT_FILETYPE_MZF );
    assert ( cmtext->cmtfile->spec != NULL );
    st_CMT_MZF_FILESPEC *spec = (st_CMT_MZF_FILESPEC*) cmtext->cmtfile->spec;
    assert ( spec->hdr != NULL );
    return spec->hdr;
}


int cmtext_cmtfile_change_speed ( st_CMTEXT *cmtext, en_MZTAPE_SPEED mztape_speed ) {
    assert ( cmtext != NULL );
    st_CMT_FILE *cmtfile = cmtext->cmtfile;
    assert ( cmtfile != NULL );
    assert ( cmtfile->type == CMT_FILETYPE_MZF );
    assert ( cmtfile->spec != NULL );
    st_CMT_MZF_FILESPEC *spec = (st_CMT_MZF_FILESPEC*) cmtext->cmtfile->spec;
    assert ( spec->mztmzf != NULL );

    if ( cmtfile->stream.stream_type == CMT_STREAM_TYPE_BITSTREAM ) {
        st_CMT_BITSTREAM *bitstream = cmtmzf_generate_bitstream ( spec->mztmzf, mztape_speed );
        if ( !bitstream ) return EXIT_FAILURE;
        if ( cmtfile->stream.str.bitstream ) cmt_bitstream_destroy ( cmtfile->stream.str.bitstream );
        cmtfile->stream.str.bitstream = bitstream;
        return EXIT_SUCCESS;
    };

    assert ( cmtfile->stream.stream_type == CMT_STREAM_TYPE_VSTREAM );
    st_CMT_VSTREAM *vstream = cmtmzf_generate_vstream ( spec->mztmzf, mztape_speed );
    if ( !vstream ) return EXIT_FAILURE;
    if ( cmtfile->stream.str.vstream ) cmt_vstream_destroy ( cmtfile->stream.str.vstream );
    cmtfile->stream.str.vstream = vstream;
    return EXIT_SUCCESS;

}


/*
 * 
 * cmtcontainer
 * 
 */
void cmtext_container_destroy ( st_CMT_CONTAINER * container ) {
    if ( !container ) return;
    if ( container->name ) ui_utils_mem_free ( container->name );
    ui_utils_mem_free ( container );
}


st_CMT_CONTAINER * cmtext_container_new ( en_CMT_CONTAINER_TYPE container_type ) {

    st_CMT_CONTAINER *container = (st_CMT_CONTAINER*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_CONTAINER ) );
    if ( !container ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_CMT_CONTAINER ) );
        return NULL;
    };

    container->type = container_type;

    if ( container_type == CMT_CONTAINER_TYPE_SINGLE ) {
        container->count_files = 1;
    };

    container->name = (char*) ui_utils_mem_alloc0 ( sizeof ( char ) );
    if ( !container ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( char ) );
        return NULL;
    };

    return container;
}


void cmtext_container_set_name ( st_CMT_CONTAINER *container, char *name ) {
    assert ( container != NULL );
    int size = strlen ( name ) + 1;
    container->name = ui_utils_mem_realloc ( container->name, size );
    if ( !container->name ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, size );
        return;
    };
    snprintf ( container->name, size, "%s", name );
}


/*
 * 
 * cmtext
 * 
 */
void cmtext_destroy ( st_CMTEXT * cmtext ) {
    if ( !cmtext ) return;
    cmtext->eject ( );
    cmtext_container_destroy ( cmtext->container );
    cmtext_cmtfile_destroy ( cmtext->cmtfile );
    ui_utils_mem_free ( cmtext );
}


st_CMTEXT * cmtext_new ( cmtext_cb_open cb_open, cmtext_cb_eject cb_eject, en_CMT_CONTAINER_TYPE container_type ) {

    assert ( cb_open != NULL );
    assert ( cb_eject != NULL );

    st_CMTEXT *cmtext = (st_CMTEXT*) ui_utils_mem_alloc0 ( sizeof ( st_CMTEXT ) );
    if ( !cmtext ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_CMTEXT ) );
        return NULL;
    };

    cmtext->open = cb_open;
    cmtext->eject = cb_eject;

    cmtext->container = cmtext_container_new ( container_type );

    return cmtext;
}


/*
 * 
 */

void cmtext_eject ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->container != NULL );
    cmtext_container_set_name ( cmtext->container, "" );
    cmtext_cmtfile_destroy ( cmtext->cmtfile );
    cmtext->cmtfile = NULL;
}


void cmtext_play ( st_CMTEXT *cmtext ) {
    assert ( cmtext != NULL );
    assert ( cmtext->cmtfile != NULL );
    if ( cmtext->cmtfile->stream.stream_type != CMT_STREAM_TYPE_VSTREAM ) return;
    st_CMT_VSTREAM *cmt_vstream = cmtext->cmtfile->stream.str.vstream;
    assert ( cmt_vstream != NULL );
    cmt_vstream_read_reset ( cmt_vstream );
}
