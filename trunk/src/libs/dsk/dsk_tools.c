/* 
 * File:   dsk_tools.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 9. září 2016, 18:57
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

#include "dsk.h"
#include "dsk_tools.h"
#include "../generic_driver/generic_driver.h"


#define DSK_TOOLS_MIN_SECTOR_SIZE   128


/**
 * Prirazeni zaznamu v existujici strukture st_DSK_DESCRIPTION.
 * 
 * Zaznamy musi byt ulozeny vzestupne podle total track.
 * 
 * @param desc Odkaz na existujici strukturu.
 * @param rule Poradove cislo zaznamu
 * @param absolute_track Absolutni stopa od ktere pravidlo plati
 * @param sectors
 * @param ssize
 * @param sector_order
 * @param sector_map
 * @param default_value
 */
void dsk_tools_assign_description ( st_DSK_DESCRIPTION *desc, uint8_t rule, uint8_t absolute_track, uint8_t sectors, en_DSK_SECTOR_SIZE ssize, en_DSK_SECTOR_ORDER_TYPE sector_order, uint8_t *sector_map, uint8_t default_value ) {
    st_DSK_DESCRIPTION_RULE *rules = ( st_DSK_DESCRIPTION_RULE* ) & desc->rules;
    rules[rule].absolute_track = absolute_track;
    rules[rule].sectors = sectors;
    rules[rule].ssize = ssize;
    rules[rule].sector_order = sector_order;
    rules[rule].sector_map = sector_map;
    rules[rule].filler = default_value;
}


/**
 * Vytvori DSK header podle description.
 * 
 * @param handler
 * @param d
 * @param desc
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_create_image_header ( void *handler, st_DRIVER *d, st_DSK_DESCRIPTION *desc ) {

    st_DSK_HEADER dskhdr_buffer;
    st_DSK_HEADER *dskhdr = NULL;

    if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, 0, (void*) &dskhdr, &dskhdr_buffer, sizeof ( st_DSK_HEADER ) ) ) return EXIT_FAILURE;

    memset ( dskhdr, 0x00, sizeof ( st_DSK_HEADER ) );
    memcpy ( dskhdr->file_info, DSK_DEFAULT_FILEINFO, DSK_FILEINFO_FIELD_LENGTH );
    memcpy ( dskhdr->creator, DSK_DEFAULT_CREATOR, DSK_CREATOR_FIELD_LENGTH );

    dskhdr->tracks = desc->tracks;
    dskhdr->sides = desc->sides;

    st_DSK_DESCRIPTION_RULE *rules = ( st_DSK_DESCRIPTION_RULE* ) & desc->rules;

    /* create tsizes */

    uint8_t sectors = rules[0].sectors;
    en_DSK_SECTOR_SIZE ssize = rules[0].ssize;
    uint8_t rule = 0;

    uint8_t abs_track = 0;
    uint8_t track;
    for ( track = 0; track < desc->tracks; track++ ) {
        uint8_t side;
        for ( side = 0; side < desc->sides; side++ ) {
            if ( rule < desc->count_rules ) {
                if ( rules[rule].absolute_track == abs_track ) {
                    sectors = rules[rule].sectors;
                    ssize = rules[rule].ssize;
                    rule++;
                };
            };
            dskhdr->tsize[abs_track++] = dsk_encode_track_size ( sectors, ssize );
        };
    };

    return generic_driver_ppwrite ( handler, d, 0, dskhdr, sizeof ( st_DSK_HEADER ) );
}


/**
 * Vytvoreni mapy sektoru, podle definice sector order.
 * 
 * @param sectors
 * @param sector_order - typ CUSTOM je automaticky preveden na NORMAL
 * @param sector_map - navratove pole o velikosti sectors, ktere bude obsahovat vsechny sector ID v pozadovanem poradi
 */
void dsk_tools_make_sector_map ( uint8_t sectors, en_DSK_SECTOR_ORDER_TYPE sector_order, uint8_t *sector_map ) {

    if ( sectors == 0 ) return;

    uint8_t sectors_to_order = ( sectors & 1 ) ? ( sectors + 1 ) : sectors;
    uint8_t sector_pos = 0;
    uint8_t i;

    if ( ( sector_order == DSK_SEC_ORDER_CUSTOM ) || ( sector_order > DSK_SEC_ORDER_INTERLACED_LEC_HD ) ) {
        sector_order = DSK_SEC_ORDER_NORMAL;
    };

    for ( i = 0; i < sectors; i += sector_order ) {
        uint8_t j = 0;
        while ( ( j < sector_order ) && ( ( i + j ) < sectors ) ) {
            uint8_t sector_id = 1 + ( i / sector_order ) + ( sectors_to_order / sector_order ) * ( j % sector_order );
            j++;
            sector_map[sector_pos] = sector_id;
            sector_pos++;
        };
    };
}


/**
 * Vytvoreni hlavicky pro stopu.
 * 
 * @param handler
 * @param d
 * @param dsk_offset
 * @param track
 * @param side
 * @param sectors
 * @param ssize
 * @param sector_map - seznam ID jednotlivych sektoru tak, jak jdou po sobe
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_create_track_header ( void *handler, st_DRIVER *d, uint32_t dsk_offset, uint8_t track, uint8_t side, uint8_t sectors, en_DSK_SECTOR_SIZE ssize, uint8_t *sector_map ) {

    st_DSK_TRACK_INFO trkhdr_buffer;
    st_DSK_TRACK_INFO *trkhdr = NULL;

    if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, dsk_offset, (void*) &trkhdr, &trkhdr_buffer, sizeof ( st_DSK_TRACK_INFO ) ) ) return EXIT_FAILURE;

    memset ( trkhdr, 0x00, sizeof ( st_DSK_TRACK_INFO ) );
    memcpy ( trkhdr->track_info, DSK_DEFAULT_TRACKINFO, DSK_TRACKINFO_FIELD_LENGTH );
    trkhdr->track = track;
    trkhdr->side = side;
    trkhdr->sectors = sectors;
    trkhdr->ssize = ssize;

    int i;
    for ( i = 0; i < sectors; i++ ) {
        trkhdr->sinfo[i].track = track;
        trkhdr->sinfo[i].side = side;
        trkhdr->sinfo[i].sector = sector_map[i];
        trkhdr->sinfo[i].ssize = ssize;
    };

    return generic_driver_ppwrite ( handler, d, dsk_offset, trkhdr, sizeof ( st_DSK_TRACK_INFO ) );
}


/**
 * Vyplni vsechny sectory na stope defaultni hodnotou.
 * 
 * @param handler
 * @param d
 * @param dsk_offset
 * @param sectors
 * @param ssize
 * @param default_value
 * @param sectors_total_bytes
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_create_track_sectors ( void *handler, st_DRIVER *d, uint32_t dsk_offset, uint8_t sectors, en_DSK_SECTOR_SIZE ssize, uint8_t default_value, uint16_t *sectors_total_bytes ) {

    *sectors_total_bytes = 0;

    int my_ssize = dsk_decode_sector_size ( ssize ) / DSK_TOOLS_MIN_SECTOR_SIZE;

    uint8_t i;
    for ( i = 0; i < sectors; i++ ) {
        int j;
        for ( j = 0; j < my_ssize; j++ ) {

            uint8_t data_buffer [ DSK_TOOLS_MIN_SECTOR_SIZE ];
            uint8_t *sector_data = NULL;

            if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, dsk_offset, (void*) &sector_data, &data_buffer, DSK_TOOLS_MIN_SECTOR_SIZE ) ) return EXIT_FAILURE;

            memset ( sector_data, default_value, DSK_TOOLS_MIN_SECTOR_SIZE );

            generic_driver_ppwrite ( handler, d, dsk_offset, sector_data, DSK_TOOLS_MIN_SECTOR_SIZE );

            *sectors_total_bytes += DSK_TOOLS_MIN_SECTOR_SIZE;
            dsk_offset += DSK_TOOLS_MIN_SECTOR_SIZE;
        };
    };

    return EXIT_SUCCESS;
}


/**
 * Vytvoreni jedne DSK stopy.
 * 
 * @param handler
 * @param d
 * @param dsk_offset
 * @param track
 * @param side
 * @param sectors
 * @param ssize
 * @param sector_map - seznam ID jednotlivych sektoru tak, jak jdou po sobe
 * @param default_value
 * @param track_total_bytes Obsahuje celkovou velikost zapsane stopy
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_create_track ( void *handler, st_DRIVER *d, uint32_t dsk_offset, uint8_t track, uint8_t side, uint8_t sectors, en_DSK_SECTOR_SIZE ssize, uint8_t *sector_map, uint8_t default_value, uint32_t *track_total_bytes ) {

    *track_total_bytes = 0;

    if ( sectors != 0 ) {
        if ( EXIT_SUCCESS != dsk_tools_create_track_header ( handler, d, dsk_offset, track, side, sectors, ssize, sector_map ) ) return EXIT_FAILURE;

        *track_total_bytes += sizeof ( st_DSK_TRACK_INFO );
        dsk_offset += sizeof ( st_DSK_TRACK_INFO );

        uint16_t sectors_total_size = 0;
        if ( EXIT_SUCCESS != dsk_tools_create_track_sectors ( handler, d, dsk_offset, sectors, ssize, default_value, &sectors_total_size ) ) return EXIT_FAILURE;

        *track_total_bytes += sectors_total_size;
    };

    return EXIT_SUCCESS;
}


/**
 * Vytvori postupne vsechny stopy podle description.
 * 
 * @param handler
 * @param d
 * @param desc
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_create_image_tracks ( void *handler, st_DRIVER *d, st_DSK_DESCRIPTION *desc ) {

    uint32_t dsk_offset = sizeof ( st_DSK_HEADER );

    st_DSK_DESCRIPTION_RULE *rules = ( st_DSK_DESCRIPTION_RULE* ) & desc->rules;

    uint8_t sectors = rules[0].sectors;
    en_DSK_SECTOR_SIZE ssize = rules[0].ssize;
    en_DSK_SECTOR_ORDER_TYPE sector_order = rules[0].sector_order;
    uint8_t default_value = rules[0].filler;
    uint8_t rule = 0;

    uint8_t abs_track = 0;
    uint8_t track;

    en_DSK_SECTOR_ORDER_TYPE last_sector_order = sector_order;
    uint8_t local_sector_map [ DSK_MAX_SECTORS ];
    uint8_t *sector_map;

    if ( ( sector_order == DSK_SEC_ORDER_CUSTOM ) && ( rules[0].sector_map != NULL ) ) {
        sector_map = rules[0].sector_map;
    } else {
        dsk_tools_make_sector_map ( sectors, sector_order, local_sector_map );
        sector_map = local_sector_map;
    };

    for ( track = 0; track < desc->tracks; track++ ) {
        uint8_t side;
        for ( side = 0; side < desc->sides; side++ ) {
            if ( rule < desc->count_rules ) {
                if ( rules[rule].absolute_track == abs_track ) {
                    sectors = rules[rule].sectors;
                    ssize = rules[rule].ssize;
                    sector_order = rules[rule].sector_order;
                    default_value = rules[rule].filler;


                    if ( ( sector_order == DSK_SEC_ORDER_CUSTOM ) && ( rules[rule].sector_map != NULL ) ) {
                        sector_map = rules[rule].sector_map;
                    } else {
                        if ( sector_order != last_sector_order ) {
                            dsk_tools_make_sector_map ( sectors, sector_order, local_sector_map );
                            sector_map = local_sector_map;
                        };
                    };
                    last_sector_order = sector_order;

                    rule++;
                };
            };

            /* vytvoreni stopy */
            uint32_t track_total_bytes = 0;

            if ( EXIT_SUCCESS != dsk_tools_create_track ( handler, d, dsk_offset, track, side, sectors, ssize, sector_map, default_value, &track_total_bytes ) ) return EXIT_FAILURE;
            dsk_offset += track_total_bytes;

            abs_track++;
        };
    };

    return EXIT_SUCCESS;
}


/**
 * Vytvoreni DSK podle popisu v desc.
 * 
 * @param handler
 * @param d
 * @param desc
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_create_image ( void *handler, st_DRIVER *d, st_DSK_DESCRIPTION *desc ) {
    if ( EXIT_SUCCESS != dsk_tools_create_image_header ( handler, d, desc ) ) return EXIT_FAILURE;
    return dsk_tools_create_image_tracks ( handler, d, desc );
}


/**
 * Zmena parametru a default obsahu konkretni absolutni stopy.
 * 
 * @param handler
 * @param d
 * @param short_image_info
 * @param abstrack
 * @param sectors
 * @param ssize
 * @param sector_map - seznam ID jednotlivych sektoru tak, jak jdou po sobe
 * @param default_value
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int dsk_tools_change_track ( void *handler, st_DRIVER *d, st_DSK_SHORT_IMAGE_INFO *short_image_info, uint8_t abstrack, uint8_t sectors, en_DSK_SECTOR_SIZE ssize, uint8_t *sector_map, uint8_t default_value ) {

    st_DSK_SHORT_IMAGE_INFO local_short_image_info;
    st_DSK_SHORT_IMAGE_INFO *iinfo = short_image_info;

    if ( iinfo == NULL ) {
        if ( EXIT_SUCCESS != dsk_read_short_image_info ( handler, d, &local_short_image_info ) ) return EXIT_FAILURE;
        iinfo = &local_short_image_info;
    };

    if ( abstrack >= ( iinfo->tracks * iinfo->sides ) ) {
        st_HANDLER *h = handler;
        h->err = DSK_ERROR_TRACK_NOT_FOUND;
        return EXIT_FAILURE;
    };

    uint32_t track_offset = dsk_compute_track_offset ( abstrack, iinfo->tsize );
    uint16_t track_size = dsk_decode_track_size ( iinfo->tsize[abstrack] );

    uint16_t new_track_size = dsk_decode_track_size ( dsk_encode_track_size ( sectors, ssize ) );

    uint8_t last_track = ( iinfo->tracks * iinfo->sides ) - 1;
    uint32_t last_track_offset = dsk_compute_track_offset ( last_track, iinfo->tsize );
    uint16_t last_track_size = dsk_decode_track_size ( iinfo->tsize[last_track] );

    uint32_t last_image_byte = last_track_offset + last_track_size;
    uint32_t new_last_image_byte = last_image_byte;

    if ( track_size != new_track_size ) {

        uint8_t buffer [ DSK_TOOLS_MIN_SECTOR_SIZE ];

        uint32_t next_track_offset = track_offset + track_size;

        uint32_t src_offset;
        uint32_t dst_offset;
        int32_t step = 0;

        if ( track_size < new_track_size ) {
            uint16_t size_diference = new_track_size - track_size;
            src_offset = last_image_byte - sizeof ( buffer );
            dst_offset = src_offset + size_diference;
            step -= sizeof ( buffer );
            new_last_image_byte += size_diference;
        } else {
            uint16_t size_diference = track_size - new_track_size;
            src_offset = next_track_offset;
            dst_offset = src_offset - size_diference;
            step = sizeof ( buffer );
            new_last_image_byte -= size_diference;
        };

        uint32_t i;
        for ( i = ( last_image_byte - next_track_offset ); i > 0; i -= sizeof ( buffer ) ) {
            if ( EXIT_SUCCESS != dsk_read_on_offset ( handler, d, src_offset, &buffer, sizeof ( buffer ) ) ) return EXIT_FAILURE;
            if ( EXIT_SUCCESS != dsk_write_on_offset ( handler, d, dst_offset, &buffer, sizeof ( buffer ) ) ) return EXIT_FAILURE;
            src_offset += step;
            dst_offset += step;
        };

        if ( track_size > new_track_size ) {
            if ( EXIT_SUCCESS != generic_driver_truncate ( handler, d, new_last_image_byte ) ) return EXIT_FAILURE;
        };

        iinfo->tsize[abstrack] = dsk_encode_track_size ( sectors, ssize );
        uint32_t offset = DSK_FILEINFO_FIELD_LENGTH + DSK_CREATOR_FIELD_LENGTH + 4 + abstrack;
        if ( EXIT_SUCCESS != dsk_write_on_offset ( handler, d, offset, &iinfo->tsize[abstrack], 1 ) ) return EXIT_FAILURE;
    };

    uint8_t side = ( iinfo->sides == 1 ) ? 0 : ( abstrack & 1 );
    uint8_t track = abstrack / iinfo->sides;

    if ( sectors != 0 ) {
        if ( EXIT_SUCCESS != dsk_tools_create_track_header ( handler, d, track_offset, track, side, sectors, ssize, sector_map ) ) return EXIT_FAILURE;

        uint16_t sectors_total_bytes;
        if ( EXIT_SUCCESS != dsk_tools_create_track_sectors ( handler, d, track_offset + sizeof ( st_DSK_TRACK_INFO ), sectors, ssize, default_value, &sectors_total_bytes ) ) return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}
