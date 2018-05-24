/* 
 * File:   cmt_stream.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. května 2018, 10:29
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
#include <assert.h>

#include "cmt_stream.h"

#include "libs/generic_driver/generic_driver.h"
#include "ui/ui_utils.h"


void cmt_stream_destroy ( st_CMT_STREAM *stream ) {
    if ( !stream ) return;
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            cmt_bitstream_destroy ( stream->str.bitstream );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            cmt_vstream_destroy ( stream->str.vstream );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
}


st_CMT_STREAM* cmt_stream_new ( en_CMT_STREAM_TYPE type ) {
    st_CMT_STREAM *stream = NULL;
    switch ( type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
        case CMT_STREAM_TYPE_VSTREAM:
            stream = (st_CMT_STREAM*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_STREAM ) );
            if ( !stream ) {
                fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, (int) sizeof ( st_CMT_STREAM ) );
                return NULL;
            };
            stream->stream_type = type;
            break;

        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, type );
    };
    return stream;
}


st_CMT_STREAM* cmt_stream_new_from_wav ( st_HANDLER *h, en_CMT_STREAM_POLARITY polarity, en_CMT_STREAM_TYPE type ) {

    st_CMT_STREAM *stream = cmt_stream_new ( type );
    if ( !stream ) {
        return NULL;
    };

    switch ( type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
        {
            st_CMT_BITSTREAM *bitstream = cmt_bitstream_new_from_wav ( h, polarity );
            if ( !bitstream ) {
                fprintf ( stderr, "%s():%d - Can't create stream\n", __func__, __LINE__ );
                cmt_stream_destroy ( stream );
                return NULL;
            };
            stream->str.bitstream = bitstream;
            break;
        }

        case CMT_STREAM_TYPE_VSTREAM:
        {
            st_CMT_VSTREAM *vstream = cmt_vstream_new_from_wav ( h, polarity );
            if ( !vstream ) {
                fprintf ( stderr, "%s():%d - Can't create stream\n", __func__, __LINE__ );
                cmt_stream_destroy ( stream );
                return NULL;
            };
            stream->str.vstream = vstream;
            break;
        }

        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, type );
            cmt_stream_destroy ( stream );
            return NULL;
    };

    return stream;
}


en_CMT_STREAM_TYPE cmt_stream_get_stream_type ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    return stream->stream_type;
}


const char* cmt_stream_get_stream_type_txt ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            return "bitstream";
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            return "vstream";
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
    return "unknown";
}


uint32_t cmt_stream_get_size ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            return cmt_bitstream_get_size ( stream->str.bitstream );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            return cmt_vstream_get_size ( stream->str.vstream );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
    return 0;
}


uint32_t cmt_stream_get_rate ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            return cmt_bitstream_get_rate ( stream->str.bitstream );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            return cmt_vstream_get_rate ( stream->str.vstream );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
    return 0;
}


double cmt_stream_get_length ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            return cmt_bitstream_get_length ( stream->str.bitstream );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            return cmt_vstream_get_length ( stream->str.vstream );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
    return 0;
}


uint64_t cmt_stream_get_count_scans ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            return cmt_bitstream_get_count_scans ( stream->str.bitstream );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            return cmt_vstream_get_count_scans ( stream->str.vstream );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
    return 0;
}


double cmt_stream_get_scantime ( st_CMT_STREAM *stream ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            return cmt_bitstream_get_scantime ( stream->str.bitstream );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            return cmt_vstream_get_scantime ( stream->str.vstream );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
    return 0;
}


void cmt_stream_set_polarity ( st_CMT_STREAM *stream, en_CMT_STREAM_POLARITY polarity ) {
    assert ( stream != NULL );
    switch ( stream->stream_type ) {
        case CMT_STREAM_TYPE_BITSTREAM:
            cmt_bitstream_change_polarity ( stream->str.bitstream, polarity );
            break;
        case CMT_STREAM_TYPE_VSTREAM:
            cmt_vstream_change_polarity ( stream->str.vstream, polarity );
            break;
        default:
            fprintf ( stderr, "%s():%d - Unknown stream type '%d'\n", __func__, __LINE__, stream->stream_type );
    };
}
