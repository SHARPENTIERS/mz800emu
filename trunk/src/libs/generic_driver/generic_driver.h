/* 
 * File:   generic_driver.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. února 2017, 9:24
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


#ifndef GENERIC_DRIVER_H
#define GENERIC_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define GENERIC_DRIVER_FILE
#define GENERIC_DRIVER_MEMORY

    //#define GENERIC_DRIVER_FILE_CB

    //#define GENERIC_DRIVER_MEMORY_CB
    //#define GENERIC_DRIVER_MEMORY_CB_USE_REALLOC


    typedef enum en_HANDLER_ERROR {
        HANDLER_ERROR_NONE = 0,
        HANDLER_ERROR_NOT_READY = 1,
        HANDLER_ERROR_WRITE_PROTECTED = 2,
        HANDLER_ERROR_USER
    } en_HANDLER_ERROR;


    typedef enum en_GENERIC_DRIVER_ERROR {
        GENERIC_DRIVER_ERROR_NONE = 0,
        GENERIC_DRIVER_ERROR_NOT_READY,
        GENERIC_DRIVER_ERROR_SEEK,
        GENERIC_DRIVER_ERROR_READ,
        GENERIC_DRIVER_ERROR_WRITE,
        GENERIC_DRIVER_ERROR_SIZE,
        GENERIC_DRIVER_ERROR_MALLOC,
        GENERIC_DRIVER_ERROR_REALLOC,
        GENERIC_DRIVER_ERROR_TRUNCATE,
        GENERIC_DRIVER_ERROR_HANDLER_TYPE,
        GENERIC_DRIVER_ERROR_HANDLER,
        GENERIC_DRIVER_ERROR_HANDLER_IS_BUSY,
        GENERIC_DRIVER_ERROR_FOPEN,
        GENERIC_DRIVER_ERROR_UNKNOWN
    } en_GENERIC_DRIVER_ERROR;


    typedef enum en_HANDLER_TYPE {
        HANDLER_TYPE_UNKNOWN = 0,
        HANDLER_TYPE_FILE,
        HANDLER_TYPE_MEMORY,
    } en_HANDLER_TYPE;


    typedef enum en_HANDLER_STATUS {
        HANDLER_STATUS_NOT_READY = 0,
        HANDLER_STATUS_READY = 1,
        HANDLER_STATUS_READ_ONLY = 2,
    } en_HANDLER_STATUS;


    typedef struct st_HANDLER_MEM {
        uint8_t *ptr;
        size_t size;
    } st_HANDLER_MEM;


    typedef union un_HANDLER_SPEC {
#ifdef GENERIC_DRIVER_FILE
        FILE *fh;
#endif
#ifdef GENERIC_DRIVER_MEMORY
        st_HANDLER_MEM mem;
#endif
    } un_HANDLER_SPEC;


    typedef struct st_HANDLER {
        en_HANDLER_TYPE type;
        en_HANDLER_STATUS status;
        en_HANDLER_ERROR err;
        un_HANDLER_SPEC spec;
    } st_HANDLER;


    typedef int (*generic_driver_prepare_cb )(void *handler, void *driver, uint32_t offset, void **prepared_buffer, uint32_t count_bytes );
    typedef int (*generic_driver_read_cb )(void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen );
    typedef int (*generic_driver_write_cb )(void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen );
    typedef int (*generic_driver_truncate_cb )(void *handler, void *driver, uint32_t new_size );


    typedef struct st_DRIVER {
        generic_driver_read_cb read_cb;
        generic_driver_write_cb write_cb;
        generic_driver_prepare_cb prepare_cb;
        generic_driver_truncate_cb truncate_cb;
        en_GENERIC_DRIVER_ERROR err;
    } st_DRIVER;


    extern void generic_driver_setup ( st_DRIVER *d, generic_driver_read_cb rdcb, generic_driver_write_cb wrcb, generic_driver_prepare_cb prepcb, generic_driver_truncate_cb trunccb );
    extern const char* generic_driver_error_message ( void *handler, st_DRIVER *d );
    extern void generic_driver_register_handler ( void *handler, en_HANDLER_TYPE type );
    extern void generic_driver_manage_handler_readonly_sts ( void *handler, int readonly );

    extern int generic_driver_prepare ( void *handler, st_DRIVER *d, uint32_t offset, void **buffer, void *tmpbuffer, uint32_t size );
    extern int generic_driver_ppread ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint32_t size );
    extern int generic_driver_ppwrite ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint32_t size );

    extern int generic_driver_read ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint16_t buffer_size );
    extern int generic_driver_write ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint16_t buffer_size );
    extern int generic_driver_truncate ( void *handler, st_DRIVER *d, uint32_t size );

    extern int generic_driver_direct_read ( void *handler, st_DRIVER *d, uint32_t offset, void **buffer, void *work_buffer, uint32_t buffer_size );


    /* Preddefinovane callbacky */

#ifdef GENERIC_DRIVER_FILE_CB
    extern int generic_driver_read_file_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen );
    extern int generic_driver_write_file_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen );
    extern int generic_driver_truncate_file_cb ( void *handler, void *driver, uint32_t size );
#endif


#ifdef GENERIC_DRIVER_MEMORY_CB
    extern int generic_driver_prepare_memory_cb ( void *handler, void *driver, uint32_t offset, void **buffer, uint32_t count_bytes );
    extern int generic_driver_read_memory_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen );
    extern int generic_driver_write_memory_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen );
    extern int generic_driver_truncate_memory_cb ( void *handler, void *driver, uint32_t size );
#endif


#ifdef __cplusplus
}
#endif

#endif /* GENERIC_DRIVER_H */
