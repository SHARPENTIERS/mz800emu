/* 
 * File:   generic_driver.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. února 2017, 9:23
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
 * Genericky driver
 * ================
 * 
 * Tato knihovna slouzi jako univerzalni prostredek pro pristup k datovym blokum.
 * Jeji hlavni smysl tkvi v tom, ze virtualizuje datovou vrstvu pod kterou se muze 
 * skryvat napr.:
 * 
 * - image ulozeny v souboru na disku
 * - image umisteny v pameti
 * - image umisteny v bankovanem ramdisku
 * 
 * Nadrazena vrstva, ktera pouziva tento univerzalni driver by mela pri praci s daty 
 * vyuzivat vzdy ty same metody. Rozdily v pristupu k datum budou urceny jen 
 * pouzitym driverem a handlerem.
 * 
 * driver
 * ------
 * 
 * - obsahuje zakladni callbacky pro operace cteni a zapisu z konkretniho offsetu
 * 
 *      generic_driver_read_cb
 *      generic_driver_write_cb
 * 
 * - muze obsahovat take callback generic_driver_truncate_cb
 * 
 * - predevsim u ruznych memory handleru pak muze byt potreba pred kazdou 
 * operaci zavolat pripravny callback generic_driver_prepare_cb
 * 
 * handler
 * -------
 * 
 * Existuje prozatim ve dvou zakladnich variantach - souborovy a pametovy. U souboroveho handleru
 * je udrzovan pouze skutecny systemovy file pointer. U pametoveho handleru udrzujeme 
 * informaci o jeho rozmeru v poctu bajtu a pointer, ktery identifikuje pocatecni 
 * misto v pameti. 
 * Handler pro praci s ramdiskem, ci s unikartou prozatim jeste neexistuje.
 * 
 * V definici hlavicek callbacku se zamerne nepouziva st_HANDLER*, ale jen void*,
 * coz umoznuje lokalne definovat nejakou vlastni strukturu handleru.
 * Lokalne definovane handlery vsak musi striktne dodrzovat to, ze na zacatku struktury jsou stejne, jako
 * genericky st_HANDLER, ale dale mohou mit pripojeny libovolne dalsi prvky, ktere mohou 
 * byt pouzity napr. uvnitr uzivatelskych callbacku.
 * 
 * Pro pametove handlery lze pouzit rychly, avsak potencionalne nebezpecny zpusob primeho pristupu:
 * 
 *   uint8_t work_buffer[256];
 *   uint8_t *b;
 * 
 *   if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, offset, (void**) &buffer, &work_buffer, sizeof ( work_buffer ) ) ) return EXIT_FAILURE;
 *   if ( EXIT_SUCCESS != generic_driver_ppread ( handler, d, offset, buffer, sizeof ( work_buffer ) ) ) return EXIT_FAILURE;
 *
 *   // Pri cteni muzeme rovnou pouzit:
 *   if ( EXIT_SUCCESS != generic_driver_direct_read ( handler, d, offset, (void**) &buffer, &work_buffer, sizeof ( work_buffer ) ) ) return EXIT_FAILURE;
 * 
 * Uvnitr generic_driver_prepare() se krom jineho rozhodne, zda *b bude ukazovat na &work_buffer,
 * nebo v pripade pametoveho handleru muze *b obdrzet hodnotu, ktera ukazuje jiz na 
 * konkretni misto image ulozeneho v pameti a volani ppread pak zavola generic_driver_read_cb(),
 * ktery pozna, ze buffer odkazuje na stejne misto se kterym se chystame pracovat a tedy jiz do bufferu nic neprenasi.
 * Timto se usetri jeden datovy prenos, ale je zde potencionalni nebezpeci v tom, ze *b ukazuje primo do image.
 * 
 */

#include <string.h>

#ifdef WINDOWS
#include <windows.h>
#elif LINUX
/* pri kompilaci pridat -D_XOPEN_SOURCE=500 */
#include <unistd.h>
#endif

#include "generic_driver.h"


/**
 * Vygenerovani error mesage pro driver, nebo handler.
 * 
 * @param handler
 * @param d
 * @return 
 */
const char* generic_driver_error_message ( void *handler, st_DRIVER *d ) {

    st_HANDLER *h = handler;

    char *no_err_msg = "no error";
    char *unknown_err_msg = "unknown error";

    const char *handler_err_msg[] = {
                                     "handler not ready",
                                     "handler is write protected",
    };

    const char *driver_err_msg[] = {
                                    "driver not ready",
                                    "seek error",
                                    "read error",
                                    "write error",
                                    "size error",
                                    "malloc error",
                                    "realloc error",
                                    "truncate error",
                                    "bad handler type",
                                    "handler is busy",
                                    "fopen error"
    };

    if ( d->err != GENERIC_DRIVER_ERROR_NONE ) {
        if ( d->err >= GENERIC_DRIVER_ERROR_UNKNOWN ) return unknown_err_msg;
        return driver_err_msg [ d->err - 1 ];
    };

    if ( h->err == HANDLER_ERROR_NONE ) return no_err_msg;
    if ( h->err >= HANDLER_ERROR_USER ) return unknown_err_msg;
    return handler_err_msg [ h->err - 1 ];
}


/**
 * Nastaveni callbacku driveru.
 * 
 * 
 * @param d
 * @param rdcb Callback volany pri operaci READ
 * @param wrcb Callback volany pri operaci WRITE
 * @param prepcb Callback volany pred kazdou operaci READ / WRITE
 * @param trucb Callback volany po obecne operaci, ktera zpusobila zmenseni datoveho zdroje
 */
void generic_driver_setup ( st_DRIVER *d, generic_driver_read_cb rdcb, generic_driver_write_cb wrcb, generic_driver_prepare_cb prepcb, generic_driver_truncate_cb trucb ) {
    d->read_cb = rdcb;
    d->write_cb = wrcb;
    d->prepare_cb = prepcb;
    d->truncate_cb = trucb;
    d->err = GENERIC_DRIVER_ERROR_NONE;
}


/**
 * Vynuluje nastaveni handleru a prideli mu pozadovany typ.
 * 
 * @param handler 
 * @param type
 */
void generic_driver_register_handler ( void *handler, en_HANDLER_TYPE type ) {
    st_HANDLER *h = handler;
    memset ( handler, 0x00, sizeof ( st_HANDLER ) );
    h->type = type;
    h->err = HANDLER_ERROR_NONE;
}


/**
 * Set/Reset readonly status handleru.
 * 
 * @param handler
 * @param readonly
 */
void generic_driver_manage_handler_readonly_sts ( void *handler, int readonly ) {
    st_HANDLER *h = handler;
    if ( readonly == 0 ) {
        h->status &= ~HANDLER_ERROR_WRITE_PROTECTED;
    } else {
        h->status |= HANDLER_ERROR_WRITE_PROTECTED;
    }
}


/**
 * Priprav driver na nasledujici operaci.
 * 
 * @param handler
 * @param d
 * @param offset
 * @param buffer
 * @param work_buffer
 * @param buffer_size
 * @return 
 */
int generic_driver_prepare ( void *handler, st_DRIVER *d, uint32_t offset, void **buffer, void *work_buffer, uint32_t buffer_size ) {
    if ( d->prepare_cb != NULL ) {
        if ( EXIT_SUCCESS != d->prepare_cb ( handler, d, offset, buffer, buffer_size ) ) return EXIT_FAILURE;
    };
    if ( *buffer == NULL ) {
        *buffer = work_buffer;
    };
    return EXIT_SUCCESS;
}


/**
 * Cteni z jiz pripraveneho driveru.
 * 
 * @param handler
 * @param d
 * @param offset
 * @param buffer
 * @param buffer_size
 * @return 
 */
int generic_driver_ppread ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint32_t buffer_size ) {
    if ( d->read_cb != NULL ) {
        uint32_t result_len;
        int result = d->read_cb ( handler, d, offset, buffer, buffer_size, &result_len );
        if ( ( result != EXIT_SUCCESS ) || ( result_len != buffer_size ) ) {
            return EXIT_FAILURE;
        };
    };
    return EXIT_SUCCESS;
}


/**
 * Zapis do jiz pripraveneho driveru.
 * 
 * @param handler
 * @param d
 * @param offset
 * @param buffer
 * @param buffer_
 * @return 
 */
int generic_driver_ppwrite ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint32_t buffer_ ) {
    if ( d->write_cb != NULL ) {
        uint32_t result_len;
        int result = d->write_cb ( handler, d, offset, buffer, buffer_, &result_len );
        if ( ( result != EXIT_SUCCESS ) || ( result_len != buffer_ ) ) {
            return EXIT_FAILURE;
        };
    };
    return EXIT_SUCCESS;
}


/**
 * Provede prepare + cteni.
 * 
 * @param handler
 * @param d
 * @param offset
 * @param buffer
 * @param buffer_size
 * @return 
 */
int generic_driver_read ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint16_t buffer_size ) {
    void *work_buffer = NULL;
    if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, offset, &work_buffer, NULL, buffer_size ) ) return EXIT_FAILURE;
    return generic_driver_ppread ( handler, d, offset, buffer, buffer_size );
}


/**
 * Provede prepare + cteni do predem pripraveneho work_buffer.
 * 
 * @param handler
 * @param d
 * @param offset
 * @param buffer
 * @param work_buffer
 * @param buffer_size
 * @return 
 */
int generic_driver_direct_read ( void *handler, st_DRIVER *d, uint32_t offset, void **buffer, void *work_buffer, uint32_t buffer_size ) {
    if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, offset, buffer, work_buffer, buffer_size ) ) return EXIT_FAILURE;
    return generic_driver_ppread ( handler, d, offset, *buffer, buffer_size );
}


/**
 * Provede prepare + zapis.
 * 
 * @param handler
 * @param d
 * @param offset
 * @param buffer
 * @param buffer_size
 * @return 
 */
int generic_driver_write ( void *handler, st_DRIVER *d, uint32_t offset, void *buffer, uint16_t buffer_size ) {
    void *work_buffer = NULL;
    if ( EXIT_SUCCESS != generic_driver_prepare ( handler, d, offset, &work_buffer, NULL, buffer_size ) ) return EXIT_FAILURE;
    return generic_driver_ppwrite ( handler, d, offset, buffer, buffer_size );
}


/**
 * Provede zarovnani media.
 * 
 * @param handler
 * @param d
 * @param size
 * @return 
 */
int generic_driver_truncate ( void *handler, st_DRIVER *d, uint32_t size ) {
    if ( d->truncate_cb != NULL ) {
        if ( EXIT_SUCCESS != d->truncate_cb ( handler, d, size ) ) return EXIT_FAILURE;
    };
    return EXIT_SUCCESS;
}


/*
 * 
 * 
 * Priklady callbacku
 * 
 * 
 */



/*
 * 
 * Driver pro praci s obrazem v souboru.
 * 
 */
#ifdef GENERIC_DRIVER_FILE_CB


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
int generic_driver_read_file_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen ) {

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

    if ( EXIT_SUCCESS != fseek ( fh, offset, SEEK_SET ) ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    *readlen = fread ( buffer, 1, count_bytes, fh );
    if ( *readlen != count_bytes ) {
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
int generic_driver_write_file_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen ) {

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

    if ( EXIT_SUCCESS != fseek ( fh, offset, SEEK_SET ) ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    *writelen = fwrite ( buffer, 1, count_bytes, fh );
    if ( *writelen != count_bytes ) {
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
int generic_driver_truncate_file_cb ( void *handler, void *driver, uint32_t size ) {

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

#ifdef WINDOWS
    if ( EXIT_SUCCESS != fseek ( fh, size, SEEK_SET ) ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };
    if ( EXIT_SUCCESS != SetEndOfFile ( fh ) ) {
        d->err = GENERIC_DRIVER_ERROR_TRUNCATE;
        return EXIT_FAILURE;
    };
#elif LINUX
    if ( EXIT_SUCCESS != ftruncate ( fileno ( fh ), size ) ) {
        d->err = GENERIC_DRIVER_ERROR_TRUNCATE;
        return EXIT_FAILURE;
    };
#endif

    return EXIT_SUCCESS;
}
#endif


/*
 * 
 * Driver pro praci s obrazem v pameti.
 * 
 */
#ifdef GENERIC_DRIVER_MEMORY_CB


/**
 * Obecny driver pro pripravu pozadovane casti obrazu do pameti.
 * 
 * @param handler
 * @param driver
 * @param offset
 * @param buffer
 * @param count_bytes
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int generic_driver_prepare_memory_cb ( void *handler, void *driver, uint32_t offset, void **buffer, uint32_t count_bytes ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_MEMORY ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER_TYPE;
        return EXIT_FAILURE;
    };

    if ( !( h->status & HANDLER_STATUS_READY ) ) {
        h->err = HANDLER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };

#ifndef GENERIC_DRIVER_MEMORY_CB_USE_REALLOC
    if ( h->spec.mem.ptr == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_NOT_READY;
        return EXIT_FAILURE;
    };
#endif

    uint32_t need_size = offset + count_bytes;
    *buffer = NULL;

#ifdef GENERIC_DRIVER_MEMORY_CB_USE_REALLOC

    if ( ( offset > h->spec.mem.size ) || ( need_size > h->spec.mem.size ) ) {

        if ( h->status & HANDLER_STATUS_READ_ONLY ) {
            h->err = HANDLER_ERROR_WRITE_PROTECTED;
            return EXIT_FAILURE;
        };

        uint8_t *new = realloc ( h->spec.mem.ptr, need_size );

        if ( new == NULL ) {
            d->err = GENERIC_DRIVER_ERROR_REALLOC;
            return EXIT_FAILURE;
        };

        h->spec.mem.ptr = new;
        h->spec.mem.size = need_size;
    };

#else

    if ( offset > h->spec.mem.size ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    if ( need_size > h->spec.mem.size ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

#endif

    *buffer = &h->spec.mem.ptr[offset];

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro cteni z pameti.
 * 
 * @param handler
 * @param driver
 * @param offset
 * @param buffer
 * @param count_bytes
 * @param readlen
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int generic_driver_read_memory_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *readlen ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;
    *readlen = 0;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_MEMORY ) {
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

    if ( offset > h->spec.mem.size ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    uint32_t need_size = offset + count_bytes;
    if ( need_size > h->spec.mem.size ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    *readlen = count_bytes;
    if ( &h->spec.mem.ptr[offset] != buffer ) {
        memmove ( buffer, &h->spec.mem.ptr[offset], count_bytes );
    };

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro zapis do pameti.
 * 
 * @param handler
 * @param driver
 * @param offset
 * @param buffer
 * @param count_bytes
 * @param writelen
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int generic_driver_write_memory_cb ( void *handler, void *driver, uint32_t offset, void *buffer, uint32_t count_bytes, uint32_t *writelen ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;
    *writelen = 0;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_MEMORY ) {
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

    if ( h->status & HANDLER_STATUS_READ_ONLY ) {
        h->err = HANDLER_ERROR_WRITE_PROTECTED;
        return EXIT_FAILURE;
    };


    if ( offset > h->spec.mem.size ) {
        d->err = GENERIC_DRIVER_ERROR_SEEK;
        return EXIT_FAILURE;
    };

    uint32_t need_size = offset + count_bytes;
    if ( need_size > h->spec.mem.size ) {
        d->err = GENERIC_DRIVER_ERROR_SIZE;
        return EXIT_FAILURE;
    };

    *writelen = count_bytes;
    if ( &h->spec.mem.ptr[offset] != buffer ) {
        memmove ( &h->spec.mem.ptr[offset], buffer, count_bytes );
    };

    return EXIT_SUCCESS;
}


/**
 * Obecny driver pro zkraceni obrazu ulozeneho v pameti.
 * 
 * @param handler
 * @param driver
 * @param size
 * @return EXIT_FAILURE | EXIT_SUCCESS
 */
int generic_driver_truncate_memory_cb ( void *handler, void *driver, uint32_t size ) {

    st_HANDLER *h = handler;
    st_DRIVER *d = driver;

    h->err = HANDLER_ERROR_NONE;
    d->err = GENERIC_DRIVER_ERROR_NONE;

    if ( h == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_HANDLER;
        return EXIT_FAILURE;
    };

    if ( h->type != HANDLER_TYPE_MEMORY ) {
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

    if ( h->status & HANDLER_STATUS_READ_ONLY ) {
        h->err = HANDLER_ERROR_WRITE_PROTECTED;
        return EXIT_FAILURE;
    };

    uint8_t *new = realloc ( h->spec.mem.ptr, size );

    if ( new == NULL ) {
        d->err = GENERIC_DRIVER_ERROR_REALLOC;
        return EXIT_FAILURE;
    };

    h->spec.mem.ptr = new;
    h->spec.mem.size = size;

    return EXIT_SUCCESS;
}
#endif
