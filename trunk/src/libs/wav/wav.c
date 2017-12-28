/* 
 * File:   wav.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. února 2017, 8:50
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


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"
#include "libs/endianity/endianity.h"
#include "libs/generic_driver/generic_driver.h"


int wav_check_riff_header ( void *handler, st_DRIVER *d ) {

    st_HANDLER *h = handler;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    uint8_t work_buffer [ sizeof ( st_WAV_RIFF_HEADER ) ];
    st_WAV_RIFF_HEADER *riff_hdr = NULL;

    if ( EXIT_SUCCESS != generic_driver_direct_read ( handler, d, 0, (void**) &riff_hdr, &work_buffer, sizeof ( work_buffer ) ) ) return EXIT_FAILURE;

    if ( 0 != strncmp ( (char*) riff_hdr->riff_tag, WAV_TAG_RIFF, sizeof ( riff_hdr->riff_tag ) ) ) return EXIT_FAILURE;
    if ( 0 != strncmp ( (char*) riff_hdr->wave_tag, WAV_TAG_WAVE, sizeof ( riff_hdr->wave_tag ) ) ) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}


int wav_check_chunk_header ( void *handler, st_DRIVER *d, uint32_t offset, const char *chunk_tag, uint32_t *chunk_size ) {

    st_HANDLER *h = handler;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    uint8_t work_buffer [ sizeof ( st_WAV_CHUNK_HEADER ) ];
    st_WAV_CHUNK_HEADER *chunk_header = NULL;

    if ( EXIT_SUCCESS != generic_driver_direct_read ( handler, d, offset, (void**) &chunk_header, &work_buffer, sizeof ( work_buffer ) ) ) return EXIT_FAILURE;

    if ( 0 != strncmp ( (char*) chunk_header->chunk_tag, chunk_tag, sizeof ( chunk_header->chunk_tag ) ) ) return EXIT_FAILURE;

    *chunk_size = endianity_bswap32_LE ( chunk_header->chunk_size );

    return EXIT_SUCCESS;
}


int wav_read_header ( void *handler, st_DRIVER *d, st_WAV_SIMPLE_HEADER *wav_sh ) {

    st_HANDLER *h = handler;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    memset ( wav_sh, 0x00, sizeof ( st_WAV_SIMPLE_HEADER ) );

    if ( EXIT_SUCCESS != wav_check_riff_header ( handler, d ) ) return EXIT_FAILURE;

    uint32_t fmt_size = 0;
    uint32_t offset = sizeof ( st_WAV_RIFF_HEADER );
    if ( EXIT_SUCCESS != wav_check_chunk_header ( handler, d, offset, WAV_TAG_FMT, &fmt_size ) ) return EXIT_FAILURE;

    offset += sizeof ( st_WAV_CHUNK_HEADER );

    uint8_t work_buffer [ sizeof ( st_WAV_FMT16 ) ];
    st_WAV_FMT16 *fmt = NULL;

    if ( EXIT_SUCCESS != generic_driver_direct_read ( handler, d, offset, (void**) &fmt, &work_buffer, sizeof ( work_buffer ) ) ) return EXIT_FAILURE;

    uint32_t data_size = 0;
    offset += fmt_size;
    if ( EXIT_SUCCESS != wav_check_chunk_header ( handler, d, offset, WAV_TAG_DATA, &data_size ) ) return EXIT_FAILURE;

    wav_sh->format_code = endianity_bswap16_LE ( fmt->format_code );
    wav_sh->channels = endianity_bswap16_LE ( fmt->channels );
    wav_sh->sample_rate = endianity_bswap32_LE ( fmt->sample_rate );
    wav_sh->bytes_per_sec = endianity_bswap32_LE ( fmt->bytes_per_sec );
    wav_sh->block_size = endianity_bswap16_LE ( fmt->block_size );
    wav_sh->bits_per_sample = endianity_bswap16_LE ( fmt->bits_per_sample );
    wav_sh->real_data_offset = offset + sizeof ( st_WAV_CHUNK_HEADER );
    wav_sh->data_size = data_size;

    return EXIT_SUCCESS;
}

/* tahle cast je jen test - pujde to pak jinam */

#include <errno.h>
#include "ui/ui_main.h"
#include "ui/ui_utils.h"
#include "ui/generic_driver/ui_file_driver.h"

#define WAV_TST_FILE "bbman.wav"


static inline void wav_resampling ( uint8_t *bitmap, uint32_t *bitmap_byte_pos, int *bitmap_bit_pos, int8_t *buffer, unsigned buffer_size ) {

    while ( buffer_size-- ) {

        if ( *bitmap_bit_pos == 0 ) {
            bitmap[*bitmap_byte_pos] = 0x00;
        };

        if ( *buffer >= 0 ) {
            bitmap[*bitmap_byte_pos] |= 1 << *bitmap_bit_pos;
        };

        if ( *bitmap_bit_pos == 7 ) {
            *bitmap_byte_pos += 1;
            *bitmap_bit_pos = 0;
        } else {
            *bitmap_bit_pos += 1;
        };

        buffer++;
    }
}


void wav_test ( void ) {

    st_HANDLER wav_handler;
    generic_driver_register_handler ( &wav_handler, HANDLER_TYPE_FILE );


    if ( EXIT_SUCCESS != ui_file_driver_open ( &wav_handler, &g_ui_file_driver, WAV_TST_FILE, FILED_RIVER_MODE_RO ) ) {
        ui_show_error ( "%s() - Can't open file '%s': %s", __func__, WAV_TST_FILE, ui_utils_strerror ( ) );
    };

    st_WAV_SIMPLE_HEADER wav_sh;

    if ( EXIT_SUCCESS != wav_read_header ( &wav_handler, &g_ui_file_driver, &wav_sh ) ) {
        printf ( "WAV - ERROR!\n" );
    } else {
        printf ( "WAV - OK!\n" );

        if ( wav_sh.format_code != WAVE_FORMAT_PCM ) {
            ui_show_error ( "%s() - file '%s' has unsiported format 0x%04x", __func__, WAV_TST_FILE, wav_sh.format_code );
        } else {

            /* tohle je funkcni jen u jednokanaloveho wav !!!*/
            uint32_t bitmap_byte_size = ( wav_sh.data_size / 8 );
            if ( ( wav_sh.data_size % 8 ) != 0 ) {
                bitmap_byte_size++;
            };

            uint8_t *bitmap = malloc ( bitmap_byte_size );
            uint32_t bitmap_byte_pos = 0;
            int bitmap_bit_pos = 0;

            int8_t buffer[256];
            int8_t *b = NULL;
            uint32_t samples = wav_sh.data_size;
            uint32_t data_offset = wav_sh.real_data_offset;
            int fl_error = 0;

            while ( samples >= sizeof ( buffer ) ) {
                if ( EXIT_SUCCESS != generic_driver_direct_read ( &wav_handler, &g_ui_file_driver, data_offset, (void**) &b, buffer, sizeof ( buffer ) ) ) {
                    fl_error = 1;
                    samples = 0;
                    break;
                };

                wav_resampling ( bitmap, &bitmap_byte_pos, &bitmap_bit_pos, b, sizeof ( buffer ) );
                samples -= sizeof ( buffer );
                data_offset += sizeof ( buffer );
            };

            if ( samples > 0 ) {
                if ( EXIT_SUCCESS != generic_driver_direct_read ( &wav_handler, &g_ui_file_driver, data_offset, (void**) &b, buffer, samples ) ) {
                    fl_error = 1;
                } else {
                    wav_resampling ( bitmap, &bitmap_byte_pos, &bitmap_bit_pos, buffer, samples );
                };
            };

            if ( fl_error == 0 ) {
                st_HANDLER handler;
                generic_driver_register_handler ( &handler, HANDLER_TYPE_FILE );


                if ( EXIT_SUCCESS != ui_file_driver_open ( &handler, &g_ui_file_driver, "output.bin", FILED_RIVER_MODE_W ) ) {
                    ui_show_error ( "%s() - Can't open file '%s': %s", __func__, "output.bin", ui_utils_strerror ( ) );
                };

                generic_driver_write ( &handler, &g_ui_file_driver, 0, bitmap, bitmap_byte_size );

                ui_file_driver_close ( &handler, &g_ui_file_driver );
            };

            free ( bitmap );
        };
    };

    ui_file_driver_close ( &wav_handler, &g_ui_file_driver );

    printf ( "OK\n" );
}


