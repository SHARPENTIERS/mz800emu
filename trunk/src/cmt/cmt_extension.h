/* 
 * File:   cmt_extension.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 30. dubna 2018, 13:30
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


#ifndef CMT_EXTENSION_H
#define CMT_EXTENSION_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "libs/cmt_stream/cmt_stream.h"
#include "libs/mzf/mzf.h"
#include "libs/mztape/mztape.h"


    typedef enum en_CMT_POLARITY {
        CMT_POLARITY_NORMAL = 0,
        CMT_POLARITY_INVERTED = 1,
    } en_CMT_POLARITY;


    typedef enum en_CMT_CONTAINER_TYPE {
        CMT_CONTAINER_TYPE_SINGLE = 0,
        CMT_CONTAINER_TYPE_MZT,
        CMT_CONTAINER_TYPE_TAP,
        CMT_CONTAINER_TYPE_TZX,
    } en_CMT_CONTAINER_TYPE;


    typedef enum en_CMT_FILETYPE {
        CMT_FILETYPE_MZF = 0,
        CMT_FILETYPE_WAV,
    } en_CMT_FILETYPE;


    typedef struct st_CMT_FILE {
        en_CMT_FILETYPE type;
        st_CMT_STREAM stream;
        void *spec;
    } st_CMT_FILE;


    typedef struct st_CMT_CONTAINER {
        en_CMT_CONTAINER_TYPE type;
        char *name;
        int count_files;
        void *spec;
    } st_CMT_CONTAINER;


    typedef int (*cmtext_cb_open ) (char * filename );
    typedef void (*cmtext_cb_eject ) (void);


    typedef struct st_CMTEXT {
        cmtext_cb_open open;
        cmtext_cb_eject eject;

        st_CMT_CONTAINER *container;
        st_CMT_FILE *cmtfile;
    } st_CMTEXT;

    extern void cmtext_cmtfile_destroy ( st_CMT_FILE *cmtfile );
    extern st_CMT_FILE* cmtext_cmtfile_new ( en_CMT_STREAM_TYPE stream_type );
    extern en_CMT_FILETYPE cmtext_cmtfile_get_filetype ( st_CMTEXT *cmtext );
    extern uint32_t cmtext_cmtfile_get_rate ( st_CMTEXT *cmtext );
    extern double cmtext_cmtfile_get_length ( st_CMTEXT *cmtext );
    extern char* cmtext_cmtfile_get_playname ( st_CMTEXT *cmtext );
    extern uint64_t cmtext_cmtfile_get_scans ( st_CMTEXT *cmtext );
    extern double cmtext_cmtfile_get_scantime ( st_CMTEXT *cmtext );
    extern en_CMT_STREAM_TYPE cmtext_cmtfile_get_stream_type ( st_CMTEXT *cmtext );
    extern st_CMT_BITSTREAM* cmtext_cmtfile_get_bitstream ( st_CMTEXT *cmtext );
    extern st_CMT_VSTREAM* cmtext_cmtfile_get_vstream ( st_CMTEXT *cmtext );
    extern st_MZF_HEADER* cmtext_cmtfile_get_mzfheader ( st_CMTEXT *cmtext );
    extern int cmtext_cmtfile_change_speed ( st_CMTEXT *cmtext, en_MZTAPE_SPEED mztape_speed );

    extern void cmtext_container_destroy ( st_CMT_CONTAINER *container );
    extern st_CMT_CONTAINER* cmtext_container_new ( en_CMT_CONTAINER_TYPE container_type );
    extern void cmtext_container_set_name ( st_CMT_CONTAINER *container, char *name );

    extern void cmtext_destroy ( st_CMTEXT *cmtext );
    extern st_CMTEXT* cmtext_new ( cmtext_cb_open cb_open, cmtext_cb_eject cb_eject, en_CMT_CONTAINER_TYPE container_type );

    extern void cmtext_eject ( st_CMTEXT *cmtext );
    extern void cmtext_play ( st_CMTEXT *cmtext );

#ifdef __cplusplus
}
#endif

#endif /* CMT_EXTENSION_H */

