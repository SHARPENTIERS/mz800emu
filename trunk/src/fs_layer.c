/* 
 * File:   fs_layer.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. února 2016, 7:27
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


#include "fs_layer.h"

#ifndef FS_LAYER_FATFS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui/ui_utils.h"

int fs_layer_fopen ( FILE **fh, char *path, char *mode ) {
    int retval;
    //    *fh = fopen ( path, mode );
    *fh = ui_utils_fopen ( path, mode );
    if ( *fh != NULL ) {
        retval = FS_LAYER_FR_OK;
    } else {
        retval = FS_LAYER_DISK_ERR;
    };
    return retval;
}


int fs_layer_fread ( FILE **fh, void *buffer, int count_bytes, unsigned int *readlen ) {
    char retval;
    *readlen = fread ( buffer, 1, count_bytes, *fh );
    if ( *readlen == count_bytes ) {
        retval = FS_LAYER_FR_OK;
    } else {
        retval = FS_LAYER_DISK_ERR;
    };
    return retval;
}


int fs_layer_fwrite ( FILE **fh, void *buffer, int count_bytes, unsigned int *writelen ) {
    char retval;
    *writelen = fwrite ( buffer, 1, count_bytes, *fh );
    if ( *writelen == count_bytes ) {
        retval = FS_LAYER_FR_OK;
    } else {
        retval = FS_LAYER_DISK_ERR;
    };
    return retval;
}


#ifdef WIN32
#include <windows.h>

int fs_layer_win32_truncate ( FILE **fh ) {
    int retval = SetEndOfFile ( *fh );
    if ( retval ) {
        retval = FS_LAYER_DISK_ERR;
    } else {
        retval = FS_LAYER_FR_OK;
    };
    return retval;
}
#endif /* WIN32 */


#endif
