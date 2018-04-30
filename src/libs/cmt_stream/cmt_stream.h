/* 
 * File:   cmt_stream.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 29. dubna 2018, 13:32
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


#include <stdint.h>
#include <assert.h>

#include "libs/generic_driver/generic_driver.h"


    /*
     * Kondenzovany WAV. 1 bit odpovida jednomu scanu.
     */
    typedef struct st_CMT_STREAM {
        uint32_t rate; // vzorkovani 1/nnn
        double scan_time; // ( 1 / rate )
        uint32_t blocks; // pocet alokovanych datovych bloku
        uint32_t scans; // pocet obsazenych bitu
        uint32_t *data; // datove bloky
    } st_CMT_STREAM;

#define CMT_STREAM_BLOCK_SIZE ( sizeof ( uint32_t ) * 8 ) // pocet bitu na datovy blok (32)

    extern st_CMT_STREAM* cmt_stream_new ( uint32_t rate, uint32_t blocks );
    extern st_CMT_STREAM* cmt_stream_new_from_wav ( st_HANDLER *wav_handler );
    extern void cmt_stream_destroy ( st_CMT_STREAM *cmt_stream );
    extern uint32_t cmt_stream_compute_required_blocks_from_scans ( uint32_t scans );
    extern int cmt_stream_create_wav ( st_HANDLER *wav_handler, st_CMT_STREAM *cmt_stream );


    /**
     * 
     */
    static inline uint32_t cmt_stream_get_position_by_time ( st_CMT_STREAM *cmt_stream, double scan_time ) {
        uint32_t position = scan_time / cmt_stream->scan_time;
        return position;
    }


    /**
     * 
     */
    static inline int cmt_stream_get_value_on_position ( st_CMT_STREAM *cmt_stream, uint32_t position ) {
        assert ( position < cmt_stream->scans );
        assert ( cmt_stream->data != NULL );
        uint32_t block = position / CMT_STREAM_BLOCK_SIZE;
        assert ( block < cmt_stream->blocks );
        return ( cmt_stream->data[block] >> ( position % CMT_STREAM_BLOCK_SIZE ) ) & 1;
    }


    /**
     * 
     */
    static inline int cmt_stream_get_value_on_time ( st_CMT_STREAM *cmt_stream, double scan_time ) {
        uint32_t position = cmt_stream_get_position_by_time ( cmt_stream, scan_time );
        return cmt_stream_get_value_on_position ( cmt_stream, position );
    }


    /**
     * 
     */
    static inline void cmt_stream_set_value_on_position ( st_CMT_STREAM *cmt_stream, uint32_t position, int value ) {
        uint32_t block = position / CMT_STREAM_BLOCK_SIZE;
        assert ( block < cmt_stream->blocks );
        assert ( cmt_stream->data != NULL );

        if ( value ) {
            cmt_stream->data[block] |= (uint32_t) 1 << ( position % CMT_STREAM_BLOCK_SIZE );
        } else {
            cmt_stream->data[block] &= ~( (uint32_t) 1 << ( position % CMT_STREAM_BLOCK_SIZE ) );
        };


        if ( !( position < cmt_stream->scans ) ) {
            cmt_stream->scans = position;
        };
    }


    static inline void cmt_stream_set_value_on_time ( st_CMT_STREAM *cmt_stream, double scan_time, int value ) {
        uint32_t position = cmt_stream_get_position_by_time ( cmt_stream, scan_time );
        cmt_stream_set_value_on_position ( cmt_stream, position, value );
    }


#ifdef __cplusplus
}
#endif

#endif /* CMT_STREAM_H */

