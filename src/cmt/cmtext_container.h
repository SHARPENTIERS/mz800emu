/* 
 * File:   cmtext_container.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. května 2018, 13:22
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


#ifndef CMTEXT_CONTAINER_H
#define CMTEXT_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "libs/generic_driver/generic_driver.h"
#include "libs/mztape/cmtspeed.h"

#include "cmtext_block_defs.h"


    typedef enum en_CMTEXT_CONTAINER_TYPE {
        CMTEXT_CONTAINER_TYPE_SINGLE = 0,
        CMTEXT_CONTAINER_TYPE_SIMPLE_TAPE,
    } en_CMTEXT_CONTAINER_TYPE;


    typedef struct st_CMTEXT_TAPE_ITEM_MZF {
        char *fname;
        uint8_t ftype;
        uint16_t fsize;
        uint16_t fexec;
        uint16_t fstrt;
        en_CMTSPEED cmtspeed;
    } st_CMTEXT_TAPE_ITEM_MZF;


    typedef struct st_CMTEXT_TAPE_ITEM_TAPHDR {
        uint16_t size; // skuitecna delka bloku (flag + data + checksum)
        char *fname;
        uint8_t code;
        uint16_t data_size;
        uint16_t param1;
        uint16_t param2;
        en_CMTSPEED cmtspeed;
    } st_CMTEXT_TAPE_ITEM_TAPHDR;


    typedef struct st_CMTEXT_TAPE_ITEM_TAPDATA {
        uint16_t size; // skuitecna delka bloku (flag + data + checksum)
        en_CMTSPEED cmtspeed;
    } st_CMTEXT_TAPE_ITEM_TAPDATA;


    typedef union un_CMTEXT_TAPE_ITEM {
        st_CMTEXT_TAPE_ITEM_MZF mzf;
        st_CMTEXT_TAPE_ITEM_TAPHDR taphdr;
        st_CMTEXT_TAPE_ITEM_TAPDATA tapdata;
    } un_CMTEXT_TAPE_ITEM;


    typedef struct st_CMTEXT_TAPE_INDEX {
        int block_id;
        uint32_t offset;
        en_CMTEXT_BLOCK_TYPE bltype;
        en_CMTEXT_BLOCK_SPEED blspeed;
        un_CMTEXT_TAPE_ITEM item;
        uint16_t pause_after;
    } st_CMTEXT_TAPE_INDEX;


    typedef struct st_CMTEXT_CONTAINER_TAPE {
        st_HANDLER *h;
        st_CMTEXT_TAPE_INDEX *index;
    } st_CMTEXT_CONTAINER_TAPE;


    typedef int ( *cmtext_container_cb_next_block ) (void);
    typedef int ( *cmtext_container_cb_previous_block ) (void);
    typedef int ( *cmtext_container_cb_open_block ) (int block_id);


    typedef struct st_CMTEXT_CONTAINER {
        char *name; // jmeno zdrojoveho souboru
        en_CMTEXT_CONTAINER_TYPE type;
        int count_blocks;
        st_CMTEXT_CONTAINER_TAPE *tape;
        cmtext_container_cb_next_block cb_next_block;
        cmtext_container_cb_previous_block cb_previous_block;
        cmtext_container_cb_open_block cb_open_block;
    } st_CMTEXT_CONTAINER;


    extern void cmtext_container_tapeindex_destroy ( st_CMTEXT_TAPE_INDEX *index, int count_blocks );

    extern st_CMTEXT_CONTAINER_TAPE* cmtext_container_tape_new ( st_HANDLER *h, st_CMTEXT_TAPE_INDEX *index );
    extern void cmtext_container_tape_destroy ( st_CMTEXT_CONTAINER_TAPE *tape, int count_blocks );

    extern st_CMTEXT_TAPE_INDEX* cmtext_container_tape_index_aloc ( st_CMTEXT_TAPE_INDEX *index, int count_items );

    extern st_CMTEXT_CONTAINER* cmtext_container_new (
                                                       en_CMTEXT_CONTAINER_TYPE type,
                                                       char *name,
                                                       int count_blocks,
                                                       st_CMTEXT_CONTAINER_TAPE *tape,
                                                       cmtext_container_cb_next_block cb_next_block,
                                                       cmtext_container_cb_previous_block cb_previous_block,
                                                       cmtext_container_cb_open_block cb_open_block
                                                       );

    extern void cmtext_container_destroy ( st_CMTEXT_CONTAINER *container );

    extern const char* cmtext_container_get_name ( st_CMTEXT_CONTAINER *container );
    extern en_CMTEXT_CONTAINER_TYPE cmtext_container_get_type ( st_CMTEXT_CONTAINER *container );
    extern int cmtext_container_get_count_blocks ( st_CMTEXT_CONTAINER *container );

#ifdef __cplusplus
}
#endif

#endif /* CMTEXT_CONTAINER_H */

