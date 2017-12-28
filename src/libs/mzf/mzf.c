/* 
 * File:   mzf.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 30. září 2016, 16:27
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
#include <stdlib.h>
#include <stdint.h>

#include "mzf.h"
#include "../generic_driver/generic_driver.h"
#include "../endianity/endianity.h"

#define MZF_FIX_FILENAME_TERMINATOR


int mzf_read_header_on_offset ( void *handler, st_DRIVER *d, uint32_t offset, st_MZF_HEADER *mzfhdr ) {

    if ( EXIT_SUCCESS != generic_driver_read ( handler, d, offset, mzfhdr, sizeof ( st_MZF_HEADER ) ) ) return EXIT_FAILURE;

    mzfhdr->fsize = endianity_bswap16_LE ( mzfhdr->fsize );
    mzfhdr->fstrt = endianity_bswap16_LE ( mzfhdr->fstrt );
    mzfhdr->fexec = endianity_bswap16_LE ( mzfhdr->fexec );

#ifdef MZF_FIX_FILENAME_TERMINATOR
    mzfhdr->fname.terminator = MZF_FNAME_TERMINATOR;
#endif

    return EXIT_SUCCESS;
}


static int mzf_write_word_on_offset ( void *handler, st_DRIVER *d, uint32_t offset, uint16_t w ) {
    uint8_t buffer = w & 0xff;
    if ( EXIT_SUCCESS != generic_driver_write ( handler, d, offset, &buffer, 1 ) ) return EXIT_FAILURE;
    buffer = w >> 8;
    return generic_driver_write ( handler, d, ( offset + 1 ), &buffer, 1 );
}


int mzf_write_header_on_offset ( void *handler, st_DRIVER *d, uint32_t offset, st_MZF_HEADER *mzfhdr ) {

#ifdef MZF_FIX_FILENAME_TERMINATOR
    mzfhdr->fname.terminator = MZF_FNAME_TERMINATOR;
#endif

    if ( EXIT_SUCCESS != generic_driver_read ( handler, d, offset, mzfhdr, sizeof (mzfhdr->ftype ) + MZF_FNAME_FULL_LENGTH ) ) return EXIT_FAILURE;
    offset += sizeof (mzfhdr->ftype ) + MZF_FNAME_FULL_LENGTH;

    if ( EXIT_SUCCESS != mzf_write_word_on_offset ( handler, d, offset, mzfhdr->fsize ) ) return EXIT_FAILURE;
    offset += sizeof (mzfhdr->fsize );

    if ( EXIT_SUCCESS != mzf_write_word_on_offset ( handler, d, offset, mzfhdr->fstrt ) ) return EXIT_FAILURE;
    offset += sizeof (mzfhdr->fstrt );

    if ( EXIT_SUCCESS != mzf_write_word_on_offset ( handler, d, offset, mzfhdr->fexec ) ) return EXIT_FAILURE;
    offset += sizeof (mzfhdr->fexec );

    if ( EXIT_SUCCESS != generic_driver_write ( handler, d, offset, &mzfhdr->cmnt, MZF_CMNT_LENGTH ) ) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}


int mzf_read_header ( void *handler, st_DRIVER *d, st_MZF_HEADER *mzfhdr ) {
    return mzf_read_header_on_offset ( handler, d, 0, mzfhdr );
}


int mzf_write_header ( void *handler, st_DRIVER *d, st_MZF_HEADER *mzfhdr ) {
    return mzf_write_header_on_offset ( handler, d, 0, mzfhdr );
}


int mzf_read_body_on_offset ( void *handler, st_DRIVER *d, uint32_t offset, uint8_t *buffer, uint16_t buffer_size ) {
    return generic_driver_read ( handler, d, offset, buffer, buffer_size );
}


int mzf_read_body ( void *handler, st_DRIVER *d, uint8_t *buffer, uint16_t buffer_size ) {
    return mzf_read_body_on_offset ( handler, d, sizeof ( st_MZF_HEADER ), buffer, buffer_size );
}


const char* mzf_error_message ( void *handler, st_DRIVER *d ) {
    return generic_driver_error_message ( handler, d );
}

