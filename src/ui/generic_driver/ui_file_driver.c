/* 
 * File:   ui_file_driver.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 20. února 2017, 13:12
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

/*
 * 
 * Soubor je umisten v sekci UI, nicmene na ni neni zavisly, protoze prebira 
 * funkce z fs_layer.h a tim padem je pouzitelny kdekoliv.
 * 
 */

#include <stdint.h>
#include <stdlib.h>

#include "ui_file_driver.h"

#include "fs_layer.h"

st_DRIVER g_ui_file_driver;


/**
 * Obecny driver pro cteni ze souboru.
 * 
 * @param handler
 * @param driver
 * @param offset
 * @param buffer
 * @param count_bytes
 * @param readlen
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_file_driver_read_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;
    *readlen = 0;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_FILE ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( !( h->status & HANDLER_STATUS_READY ) ) {
        h->err = HANDLER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    FILE *fh = h->spec.fh;

    if ( fh == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    if ( FS_LAYER_FR_OK != FS_LAYER_FSEEK ( fh, offset ) ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    if ( FS_LAYER_FR_OK != FS_LAYER_FREAD ( fh, buffer, count_bytes, readlen ) ) {
        d->err = GENERIC_DRIVER_ERROR_READ;
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro zapis do souboru.
 * 
 * @param handler
 * @param driver
 * @param offset
 * @param buffer
 * @param count_bytes
 * @param writelen
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_file_driver_write_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;
    *writelen = 0;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_FILE ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( !( h->status & HANDLER_STATUS_READY ) ) {
        h->err = HANDLER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    if ( h->status & HANDLER_STATUS_READ_ONLY ) {
        h->err = HANDLER_ERROR_WRITE_PROTECTED;
        return EXIT_FAILURE;
    };

    FILE *fh = h->spec.fh;

    if ( fh == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    if ( FS_LAYER_FR_OK != FS_LAYER_FSEEK ( fh, offset ) ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    if ( FS_LAYER_FR_OK != FS_LAYER_FWRITE ( fh, buffer, count_bytes, writelen ) ) {
        d->err = GENERIC_DRIVER_ERROR_WRITE;
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro zkraceni souboru na pozadovanou delku.
 * 
 * @param handler
 * @param driver
 * @param size
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int ui_file_driver_truncate_cb ( void *handler, void *driver, uint32_t size ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_FILE ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( !( h->status & HANDLER_STATUS_READY ) ) {
        h->err = HANDLER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    FILE *fh = h->spec.fh;

    if ( fh == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    if ( h->status & HANDLER_STATUS_READ_ONLY ) {
        h->err = HANDLER_ERROR_WRITE_PROTECTED;
        return EXIT_FAILURE;
    };

    if ( FS_LAYER_FR_OK != FS_LAYER_FSEEK ( fh, size ) ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    if ( FS_LAYER_FR_OK != FS_LAYER_FTRUNCATE ( fh ) ) {
        d->err = GENERIC_DRIVER_ERROR_TRUNCATE;
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}


void ui_file_driver_init ( void ) {
    generic_driver_setup ( &g_ui_file_driver, ui_file_driver_read_cb, ui_file_driver_write_cb, NULL, ui_file_driver_truncate_cb );
}


int ui_file_driver_open ( void *handler, st_DRIVER *d, char *filename, en_FILE_DRIVER_MODE mode ) {

    st_HANDLER *h = handler;

    int fl_readonly = 1;
    char *txt_mode = FS_LAYER_FMODE_RO;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->status & HANDLER_STATUS_READY ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_IS_BUSY;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_FILE ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( h->spec.fh != NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_IS_BUSY;
        return EXIT_FAILURE;
    };

    d->err = GENERIC_DRIVER_ERROR_NONE;
    h->status = HANDLER_STATUS_NOT_READY;

    switch ( mode ) {
        case FILED_RIVER_MODE_RO:
            txt_mode = FS_LAYER_FMODE_RO;
            fl_readonly = 1;
            break;

        case FILED_RIVER_MODE_RW:
            txt_mode = FS_LAYER_FMODE_RW;
            fl_readonly = 0;
            break;

        case FILED_RIVER_MODE_W:
            txt_mode = FS_LAYER_FMODE_W;
            fl_readonly = 0;
            break;
    };

    if ( FS_LAYER_DISK_ERR == FS_LAYER_FOPEN ( h->spec.fh, filename, txt_mode ) ) {
        d->err = GENERIC_DRIVER_ERROR_FOPEN;
        return EXIT_FAILURE;
    };

    h->status = HANDLER_STATUS_READY;

    if ( fl_readonly ) {
        h->status |= HANDLER_STATUS_READ_ONLY;
    };

    return EXIT_SUCCESS;
}


int ui_file_driver_close ( void *handler, st_DRIVER *d ) {

    st_HANDLER *h = handler;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->status & HANDLER_STATUS_READY ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_FILE ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( !( h->status & HANDLER_STATUS_READY ) ) {
        h->err = HANDLER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    if ( h->spec.mem.ptr == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

    FS_LAYER_FCLOSE ( h->spec.fh );

    h->spec.fh = NULL;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;
    h->status = HANDLER_STATUS_NOT_READY;

    return EXIT_SUCCESS;
}
