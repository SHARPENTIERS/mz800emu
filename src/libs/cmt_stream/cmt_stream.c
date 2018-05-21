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

#include "cmt_stream.h"


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
