/* 
 * File:   ui_memory_driver.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 20. února 2017, 17:11
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

#include "ui_memory_driver.h"

#include "src/ui/ui_utils.h"

st_DRIVER g_ui_memory_driver_static;
st_DRIVER g_ui_memory_driver_realloc;


/**
 * Obecny driver pro pripravu pozadovane casti obrazu do pameti - bez realokace.
 * 
 * @param handler
 * @param offset
 * @param buffer
 * @param count_bytes
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_memory_driver_prepare_static_cb ( st_HANDLER *h, uint32_t offset, void **buffer, uint32_t count_bytes ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    if ( EXIT_SUCCESS != generic_driver_memory_operation_internal_bootstrap ( h ) ) return EXIT_FAILURE;

    uint32_t need_size = offset + count_bytes;
    *buffer = NULL;

    if ( offset > memspec->size ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    if ( need_size > memspec->size ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    *buffer = &memspec->ptr[offset];

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro pripravu pozadovane casti obrazu do pameti - s povolenou realokaci.
 * 
 * @param handler
 * @param offset
 * @param buffer
 * @param count_bytes
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_memory_driver_prepare_realloc_cb ( st_HANDLER *h, uint32_t offset, void **buffer, uint32_t count_bytes ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    if ( EXIT_SUCCESS != generic_driver_memory_operation_internal_bootstrap ( h ) ) return EXIT_FAILURE;

    uint32_t need_size = offset + count_bytes;
    *buffer = NULL;

    if ( ( offset > memspec->size ) || ( need_size > memspec->size ) ) {

        if ( h->status & HANDLER_STATUS_READ_ONLY ) {
            h->err = HANDLER_ERROR_WRITE_PROTECTED;
            return EXIT_FAILURE;
        };

        uint8_t *new = ui_utils_mem_realloc ( memspec->ptr, need_size );

        if ( new == NULL ) {
            d->err = GENERIC_DRIVER_ERROR_REALLOC;
            return EXIT_FAILURE;
        };

        memspec->ptr = new;
        memspec->size = need_size;
    };

    *buffer = &memspec->ptr[offset];

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro cteni z pameti.
 * 
 * @param handler
 * @param offset
 * @param buffer
 * @param count_bytes
 * @param readlen
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_memory_driver_read_cb ( st_HANDLER *h, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    *readlen = 0;

    if ( EXIT_SUCCESS != generic_driver_memory_operation_internal_bootstrap ( h ) ) return EXIT_FAILURE;

    if ( offset > memspec->size ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    uint32_t need_size = offset + count_bytes;
    if ( need_size > memspec->size ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    *readlen = count_bytes;
    if ( &memspec->ptr[offset] != buffer ) {
        memmove ( buffer, &memspec->ptr[offset], count_bytes );
    };

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro zapis do pameti.
 * 
 * @param handler
 * @param offset
 * @param buffer
 * @param count_bytes
 * @param writelen
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_memory_driver_write_cb ( st_HANDLER *h, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    *writelen = 0;

    if ( EXIT_SUCCESS != generic_driver_memory_operation_internal_bootstrap ( h ) ) return EXIT_FAILURE;

    if ( h->status & HANDLER_STATUS_READ_ONLY ) {
        h->err = HANDLER_ERROR_WRITE_PROTECTED;
        return EXIT_FAILURE;
    };

    if ( offset > memspec->size ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    uint32_t need_size = offset + count_bytes;
    if ( need_size > memspec->size ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    *writelen = count_bytes;
    if ( &memspec->ptr[offset] != buffer ) {
        memmove ( &memspec->ptr[offset], buffer, count_bytes );
    };

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro zkraceni obrazu ulozeneho v pameti.
 * 
 * @param handler
 * @param size
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_memory_driver_truncate_cb ( st_HANDLER *h, uint32_t size ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    if ( EXIT_SUCCESS != generic_driver_memory_operation_internal_bootstrap ( h ) ) return EXIT_FAILURE;

    if ( h->status & HANDLER_STATUS_READ_ONLY ) {
        h->err = HANDLER_ERROR_WRITE_PROTECTED;
        return EXIT_FAILURE;
    };

    if ( size < 1 ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    uint8_t *new = ui_utils_mem_realloc ( memspec->ptr, size );

    if ( new == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_REALLOC;
        return EXIT_FAILURE;
    };

    memspec->ptr = new;
    memspec->size = size;

    return EXIT_SUCCESS;
}


int ui_memory_driver_open_cb ( st_HANDLER *h ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    if ( h->status & HANDLER_STATUS_READY ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_IS_BUSY;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_MEMORY ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( memspec->ptr != NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_IS_BUSY;
        return EXIT_FAILURE;
    };

    if ( memspec->open_size < 1 ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    d->err = GENERIC_DRIVER_ERROR_NONE;
    h->status = HANDLER_STATUS_NOT_READY;

    uint8_t *new_ptr = ui_utils_mem_alloc0 ( memspec->open_size );

    if ( new_ptr == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_MALLOC;
        return EXIT_FAILURE;
    };

    memspec->ptr = new_ptr;
    memspec->size = memspec->open_size;

    h->status = HANDLER_STATUS_READY;

    return EXIT_SUCCESS;
}


int ui_memory_driver_close_cb ( st_HANDLER *h ) {

    st_DRIVER *d = (st_DRIVER *) h->driver;
    st_HANDLER_MEMSPC *memspec = &h->spec.memspec;

    if ( EXIT_SUCCESS != generic_driver_memory_operation_internal_bootstrap ( h ) ) return EXIT_FAILURE;

    ui_utils_mem_free ( memspec->ptr );

    memspec->ptr = NULL;
    memspec->size = 0;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;
    h->status = HANDLER_STATUS_NOT_READY;

    return EXIT_SUCCESS;
}


void ui_memory_driver_init ( void ) {
    generic_driver_setup ( &g_ui_memory_driver_static, ui_memory_driver_open_cb, ui_memory_driver_close_cb, ui_memory_driver_read_cb, ui_memory_driver_write_cb, ui_memory_driver_prepare_static_cb, ui_memory_driver_truncate_cb );
    generic_driver_setup ( &g_ui_memory_driver_realloc, ui_memory_driver_open_cb, ui_memory_driver_close_cb, ui_memory_driver_read_cb, ui_memory_driver_write_cb, ui_memory_driver_prepare_realloc_cb, ui_memory_driver_truncate_cb );
}

