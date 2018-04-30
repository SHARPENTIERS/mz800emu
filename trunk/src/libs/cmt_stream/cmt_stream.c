/* 
 * File:   cmt_stream.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 29. dubna 2018, 13:37
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
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "ui/ui_utils.h"

#include "libs/generic_driver/generic_driver.h"
#include "libs/wav/wav.h"
#include "libs/endianity/endianity.h"
#include "libs/mztape/mztape.h"

#include "cmt_stream.h"


void cmt_stream_destroy ( st_CMT_STREAM *cmt_stream ) {
    if ( !cmt_stream ) return;
    if ( cmt_stream->data ) {
        ui_utils_mem_free ( cmt_stream->data );
    }
    ui_utils_mem_free ( cmt_stream );
}


st_CMT_STREAM* cmt_stream_new ( uint32_t rate, uint32_t blocks ) {

    st_CMT_STREAM *cmt_stream = ui_utils_mem_alloc0 ( sizeof ( st_CMT_STREAM ) );

    if ( !cmt_stream ) {
        fprintf ( stderr, "%s() - %d, Error: can't alocate memory - strerr: %s\n", __func__, __LINE__, strerror ( errno ) );
        return NULL;
    };

    cmt_stream->rate = rate;
    cmt_stream->scan_time = ( 1 / (double) rate );

    if ( blocks ) {
        cmt_stream->data = ui_utils_mem_alloc0 ( ( CMT_STREAM_BLOCK_SIZE / 8 ) * blocks );
        if ( !cmt_stream->data ) {
            fprintf ( stderr, "%s() - %d, Error: can't alocate memory - strerr: %s\n", __func__, __LINE__, strerror ( errno ) );
            return NULL;
        };
        cmt_stream->blocks = blocks;
    };

    return cmt_stream;
}


uint32_t cmt_stream_compute_required_blocks_from_scans ( uint32_t scans ) {
    uint32_t required_blocks = scans / CMT_STREAM_BLOCK_SIZE;
    if ( scans % CMT_STREAM_BLOCK_SIZE ) required_blocks++;
    return required_blocks;
}


st_CMT_STREAM* cmt_stream_new_from_wav ( st_HANDLER *wav_handler ) {

    st_HANDLER *h = wav_handler;

    st_WAV_SIMPLE_HEADER *sh = wav_simple_header_new_from_handler ( h );

    if ( !sh ) {
        return NULL;
    };

    uint32_t scans = sh->blocks;
    uint32_t blocks = cmt_stream_compute_required_blocks_from_scans ( scans );

    st_CMT_STREAM *cmt_stream = cmt_stream_new ( sh->sample_rate, blocks );
    if ( !cmt_stream ) {
        return NULL;
    }

    cmt_stream->scans = scans;

    assert ( sh->bits_per_sample <= 64 );

    int8_t buffer[( WAV_MAX_BITS_PER_SAMPLE / 8 )];
    void *buf = &buffer;
    int sample_bytes = ( sh->bits_per_sample / 8 );

    uint32_t i;
    for ( i = 0; i < scans; i++ ) {

        uint32_t offset = ( i * sh->block_size ) + sh->real_data_offset;

        if ( EXIT_SUCCESS != generic_driver_read ( h, offset, buffer, sample_bytes ) ) {
            fprintf ( stderr, "%s():%d - Could not read sample: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
            cmt_stream_destroy ( cmt_stream );
            return NULL;
        };

        int scan_bit = 0;

        switch ( sh->bits_per_sample ) {

            case 8:
            {
                int8_t sample = buffer[0];
                scan_bit = ( sample > 0 ) ? 1 : 0;
                break;
            }

            case 16:
            {
                int16_t sample = endianity_bswap16_LE ( * (int16_t*) buf );
                scan_bit = ( sample > 0 ) ? 1 : 0;
                break;
            }

            case 32:
            {
                int32_t sample = endianity_bswap32_LE ( * (int32_t*) buf );
                scan_bit = ( sample > 0 ) ? 1 : 0;
                break;
            }

            case 64:
            {
                int64_t sample = endianity_bswap64_LE ( * (int64_t*) buf );
                scan_bit = ( sample > 0 ) ? 1 : 0;
                break;
            }
        };

        cmt_stream_set_value_on_position ( cmt_stream, i, scan_bit );
    };

    return cmt_stream;
}


int cmt_stream_create_wav ( st_HANDLER *wav_handler, st_CMT_STREAM *cmt_stream ) {

    st_HANDLER *h = wav_handler;

    uint32_t wav_size = sizeof ( st_WAV_RIFF_HEADER ) + sizeof ( st_WAV_CHUNK_HEADER ) + sizeof ( st_WAV_FMT16 ) + sizeof ( st_WAV_CHUNK_HEADER ) + cmt_stream->scans;

    st_WAV_RIFF_HEADER wavhdr;
    memcpy ( wavhdr.riff_tag, WAV_TAG_RIFF, sizeof ( wavhdr.riff_tag ) );
    memcpy ( wavhdr.wave_tag, WAV_TAG_WAVE, sizeof ( wavhdr.riff_tag ) );
    wavhdr.overall_size = endianity_bswap32_LE ( wav_size - sizeof ( st_WAV_RIFF_HEADER ) );

    uint32_t pos = 0;

    if ( EXIT_SUCCESS != generic_driver_write ( h, pos, &wavhdr, sizeof ( st_WAV_RIFF_HEADER ) ) ) {
        fprintf ( stderr, "%s():%d - Could not write: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        return EXIT_FAILURE;
    };

    pos += sizeof ( st_WAV_RIFF_HEADER );

    st_WAV_CHUNK_HEADER fmtchunk;
    memcpy ( fmtchunk.chunk_tag, WAV_TAG_FMT, sizeof ( fmtchunk.chunk_tag ) );
    fmtchunk.chunk_size = endianity_bswap32_LE ( sizeof ( st_WAV_FMT16 ) );

    if ( EXIT_SUCCESS != generic_driver_write ( h, pos, &fmtchunk, sizeof ( st_WAV_CHUNK_HEADER ) ) ) {
        fprintf ( stderr, "%s():%d - Could not write: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        return EXIT_FAILURE;
    };

    pos += sizeof ( st_WAV_CHUNK_HEADER );

    st_WAV_FMT16 fmt;
    fmt.format_code = endianity_bswap16_LE ( WAVE_FORMAT_PCM );
    fmt.channels = endianity_bswap16_LE ( 1 );
    fmt.sample_rate = endianity_bswap32_LE ( cmt_stream->rate );
    fmt.bytes_per_sec = endianity_bswap32_LE ( ( cmt_stream->rate * 8 * 1 ) / 8 );
    fmt.block_size = endianity_bswap16_LE ( ( 8 * 1 ) / 8 );
    fmt.bits_per_sample = endianity_bswap16_LE ( 8 );

    if ( EXIT_SUCCESS != generic_driver_write ( h, pos, &fmt, sizeof ( st_WAV_FMT16 ) ) ) {
        fprintf ( stderr, "%s():%d - Could not write: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        return EXIT_FAILURE;
    };

    pos += sizeof ( st_WAV_FMT16 );

    st_WAV_CHUNK_HEADER datachunk;
    memcpy ( datachunk.chunk_tag, WAV_TAG_DATA, sizeof ( datachunk.chunk_tag ) );
    datachunk.chunk_size = endianity_bswap32_LE ( cmt_stream->scans );

    if ( EXIT_SUCCESS != generic_driver_write ( h, pos, &datachunk, sizeof ( st_WAV_CHUNK_HEADER ) ) ) {
        fprintf ( stderr, "%s():%d - Could not write: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        return EXIT_FAILURE;
    };

    pos += sizeof ( st_WAV_CHUNK_HEADER );

    uint32_t i;
    for ( i = 0; i < cmt_stream->scans; i++ ) {

        int sample = cmt_stream_get_value_on_position ( cmt_stream, i );

        int8_t buffer = ( sample ) ? MZTAPE_WAV_LEVEL_HIGH : MZTAPE_WAV_LEVEL_LOW;

        if ( EXIT_SUCCESS != generic_driver_write ( h, pos++, &buffer, 1 ) ) {
            fprintf ( stderr, "%s():%d - Could not write: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
            return EXIT_FAILURE;
        };

    };

    return EXIT_SUCCESS;
}
