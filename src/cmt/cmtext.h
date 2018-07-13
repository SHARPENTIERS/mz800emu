/* 
 * File:   cmtext.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. května 2018, 13:21
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


#ifndef CMTEXT_H
#define CMTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "libs/cmt_stream/cmt_stream.h"
#include "cmtext_container.h"
#include "cmtext_block.h"


    typedef struct st_CMTEXT_INFO {
        char *name;
        char **fileext; // podporovane pripony, zakonceno NULL
        char *description;
    } st_CMTEXT_INFO;


    typedef void ( *cmtext_cb_init ) (void);
    typedef void ( *cmtext_cb_exit ) (void);
    typedef int ( *cmtext_cb_open ) (char *filename );
    typedef void ( *cmtext_cb_eject ) (void);


    typedef struct st_CMTEXT {
        st_CMTEXT_INFO *info;
        st_CMTEXT_CONTAINER *container;
        st_CMTEXT_BLOCK *block; // prave prehravany datovy blok
        cmtext_cb_init cb_init;
        cmtext_cb_exit cb_exit;
        cmtext_cb_open cb_open;
        cmtext_cb_eject cb_eject;
    } st_CMTEXT;


    extern void cmtext_init ( void );
    extern void cmtext_exit ( void );

    extern st_CMTEXT* cmtext_get_extension ( const char *filename );
    extern const char* cmtext_get_description ( st_CMTEXT *ext );
    extern const char* cmtext_get_name ( st_CMTEXT *ext );
    extern st_CMTEXT_CONTAINER* cmtext_get_container ( st_CMTEXT *ext );

    extern int cmtext_container_open ( const char *filename );
    
    extern const char* cmtext_get_filename_extension ( const char *filename );
    



#ifdef __cplusplus
}
#endif

#endif /* CMTEXT_H */

