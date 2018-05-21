/* 
 * File:   cmt_stream.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 7. května 2018, 10:23
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


#ifndef CMT_STREAM_H
#define CMT_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmt_stream_all.h"

    typedef enum en_CMT_STREAM_TYPE {
        CMT_STREAM_TYPE_BITSTREAM = 0,
        CMT_STREAM_TYPE_VSTREAM,
    } en_CMT_STREAM_TYPE;


#include "cmt_bitstream.h"
#include "cmt_vstream.h"


    typedef union un_CMT_STREAM {
        st_CMT_BITSTREAM *bitstream;
        st_CMT_VSTREAM *vstream;
    } un_CMT_STREAM;


    typedef struct st_CMT_STREAM {
        en_CMT_STREAM_TYPE stream_type;
        un_CMT_STREAM str;
    } st_CMT_STREAM;

    extern void cmt_stream_destroy ( st_CMT_STREAM *stream );

#ifdef __cplusplus
}
#endif

#endif /* CMT_STREAM_H */

