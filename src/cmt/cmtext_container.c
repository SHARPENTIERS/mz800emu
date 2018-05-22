/* 
 * File:   cmtext_container.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. května 2018, 13:20
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
#include <assert.h>

#include "libs/generic_driver/generic_driver.h"

#include "ui/ui_utils.h"

#include "cmtext_container.h"


void cmtext_container_tapeindex_destroy ( st_CMTEXT_TAPE_INDEX *index, int count_blocks ) {
    if ( !index ) return;
    int i;
    for ( i = 0; i < count_blocks; i++ ) {
        if ( index[i].bltype == CMTEXT_BLOCK_TYPE_MZF ) {
            if ( index[i].item.mzf.fname ) ui_utils_mem_free ( index[i].item.mzf.fname );
        } else if ( index[i].bltype == CMTEXT_BLOCK_TYPE_TAPHEADER ) {
            if ( index[i].item.taphdr.fname ) ui_utils_mem_free ( index[i].item.taphdr.fname );
        };
    };
    ui_utils_mem_free ( index );
}


void cmtext_container_tape_destroy ( st_CMTEXT_CONTAINER_TAPE *tape, int count_blocks ) {
    if ( !tape ) return;
    if ( tape->h ) {
        generic_driver_close ( tape->h );
    };
    cmtext_container_tapeindex_destroy ( tape->index, count_blocks );
    ui_utils_mem_free ( tape );
}


void cmtext_container_destroy ( st_CMTEXT_CONTAINER *container ) {
    if ( !container ) return;
    if ( container->name ) ui_utils_mem_free ( container->name );
    cmtext_container_tape_destroy ( container->tape, container->count_blocks );
    ui_utils_mem_free ( container );
}


st_CMTEXT_TAPE_INDEX* cmtext_container_tape_index_aloc ( st_CMTEXT_TAPE_INDEX *index, int count_items ) {
    uint32_t size = ( count_items + 1 ) * sizeof ( st_CMTEXT_TAPE_INDEX );

    if ( !index ) {
        index = (st_CMTEXT_TAPE_INDEX*) ui_utils_mem_alloc ( size );
    } else {
        index = (st_CMTEXT_TAPE_INDEX*) ui_utils_mem_realloc ( index, size );
    };

    if ( !index ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d)\n", __func__, __LINE__, size );
        return NULL;
    };

    return index;
}


st_CMTEXT_CONTAINER_TAPE* cmtext_container_tape_new ( st_HANDLER *h, st_CMTEXT_TAPE_INDEX *index ) {
    st_CMTEXT_CONTAINER_TAPE *tape = (st_CMTEXT_CONTAINER_TAPE*) ui_utils_mem_alloc0 ( sizeof ( st_CMTEXT_CONTAINER_TAPE ) );
    if ( !tape ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d).", __func__, __LINE__, (int) sizeof ( st_CMTEXT_CONTAINER_TAPE ) );
        return NULL;
    };
    tape->h = h;
    tape->index = index;
    return tape;
}


st_CMTEXT_CONTAINER* cmtext_container_new (
                                            en_CMTEXT_CONTAINER_TYPE type,
                                            char *name,
                                            int count_blocks,
                                            st_CMTEXT_CONTAINER_TAPE *tape,
                                            cmtext_container_cb_next_block cb_next_block,
                                            cmtext_container_cb_previous_block cb_previous_block,
                                            cmtext_container_cb_open_block cb_open_block
                                            ) {

    st_CMTEXT_CONTAINER *container = (st_CMTEXT_CONTAINER*) ui_utils_mem_alloc0 ( sizeof ( st_CMTEXT_CONTAINER ) );
    if ( !container ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d).", __func__, __LINE__, (int) sizeof ( st_CMTEXT_CONTAINER ) );
        return NULL;
    };
    container->type = type;
    container->count_blocks = count_blocks;
    int name_size = sizeof ( char ) * ( strlen ( name ) + 1 );
    container->name = (char*) ui_utils_mem_alloc0 ( name_size );
    if ( !container->name ) {
        fprintf ( stderr, "%s():%d - Can't alocate memory (%d).", __func__, __LINE__, name_size );
        cmtext_container_destroy ( container );
        return NULL;
    };
    container->name = name;
    container->tape = tape;
    container->cb_next_block = cb_next_block;
    container->cb_previous_block = cb_previous_block;
    container->cb_open_block = cb_open_block;
    return container;
}


const char* cmtext_container_get_name ( st_CMTEXT_CONTAINER *container ) {
    assert ( container );
    assert ( container->name );
    return container->name;
}


en_CMTEXT_CONTAINER_TYPE cmtext_container_get_type ( st_CMTEXT_CONTAINER *container ) {
    assert ( container );
    return container->type;
}


int cmtext_container_get_count_blocks ( st_CMTEXT_CONTAINER *container ) {
    assert ( container );
    return container->count_blocks;
}
