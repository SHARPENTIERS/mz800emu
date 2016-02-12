/* 
 * File:   fs_layer.h
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

#ifndef FS_LAYER_H
#define	FS_LAYER_H

#ifdef	__cplusplus
extern "C" {
#endif


#ifdef FS_LAYER_FATFS

#include "ff.h"

#define FILE_FOPEN(fh,path,mode) f_open ( &fh, path, mode )
#define FILE_FCLOSE(fh) f_close ( &fh )

#define FILE_FSEEK(fh,offset)    f_lseek ( &fh, offset )

#define FILE_FREAD(fh,buffer,count_bytes,readlen) f_read( &fh, buffer, count_bytes, readlen )
#define FILE_FWRITE(fh,buffer,count_bytes,writelen) f_write( &fh, buffer, count_bytes, writelen )

#define FILE_FSYNC(fh) f_sync ( &fh )
#define FILE_FTRUNCATE(fh) f_truncate ( &fh )

#define FILE_MODE_RO    ( FA_READ )
#define FILE_MODE_RW    ( FA_READ | FA_WRITE )
#define FILE_MODE_W     ( FA_CREATE_NEW | FA_WRITE )

#else /* FS_LAYER_FATFS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui/ui_utils.h"

    extern int std_FOPEN ( FILE **fh, char *path, char *mode );
    extern int std_FREAD ( FILE **fh, void *buffer, int count_bytes, unsigned int *readlen );
    extern int std_FWRITE ( FILE **fh, void *buffer, int count_bytes, unsigned int *writelen );

#define FR_OK           0
#define FR_DISK_ERR     1

#define FILE_FOPEN(fh,path,mode) std_FOPEN ( &fh, path, mode )
#define FILE_FCLOSE(fh) fclose ( fh )

#define FILE_FSEEK(fh,offset)    fseek ( fh, offset, SEEK_SET )

#define FILE_FREAD(fh,buffer,count_bytes,readlen) std_FREAD( &fh, buffer, count_bytes, readlen )
#define FILE_FWRITE(fh,buffer,count_bytes,writelen) std_FWRITE( &fh, buffer, count_bytes, writelen )

#define FILE_FSYNC(fh) fflush ( fh )

#ifdef WIN32

#include <windows.h>

    extern int win32_FTRUNCATE ( FILE **fh );

#define FILE_FTRUNCATE(fh) win32_FTRUNCATE ( &fh )

#elif LINUX

    /* pri kompilaci pridat -D_XOPEN_SOURCE=500 */
#include <unistd.h>
#define FILE_FTRUNCATE(fh) ftruncate ( fileno ( fh ), ftell ( fh ) )

#endif

#define FILE_MODE_RO   "rb"
#define FILE_MODE_RW  "r+b"
#define FILE_MODE_W   "w"

#endif /* ! FS_LAYER_FATFS */

#ifdef	__cplusplus
}
#endif

#endif	/* FS_LAYER_H */

