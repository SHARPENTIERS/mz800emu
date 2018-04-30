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

#include "libs/cmt_stream/cmt_stream.h"
#include "libs/mzf/mzf.h"


    typedef enum en_CMT_SPEED {
        CMT_SPEED_1200 = 0, // 1:1
        CMT_SPEED_2400, // 2:1
        CMT_SPEED_3600, // 3:1
    } en_CMT_SPEED;


    typedef enum en_CMT_EXTENSION_NAME {
        CMT_EXTENSION_NAME_MZF = 0,
        CMT_EXTENSION_NAME_WAV,
    } en_CMT_EXTENSION_NAME;

    typedef int ( *cmt_extension_open_cb ) (char *filename );
    typedef void ( *cmt_extension_eject_cb ) (void);
    typedef en_CMT_EXTENSION_NAME ( *cmt_extension_get_name_cb ) (void);
    typedef st_CMT_STREAM* ( *cmt_extension_get_stream_cb ) (void);
    typedef int ( *cmt_extension_change_speed_cb ) ( en_CMT_SPEED speed );
    typedef st_MZF_HEADER* ( *cmt_extension_get_mzfheader_cb ) (void);
    typedef char* ( *cmt_extension_get_playfile_name_cb ) (void);


    typedef struct st_CMT_EXTENSION {
        cmt_extension_open_cb open;
        cmt_extension_eject_cb eject;
        cmt_extension_change_speed_cb change_speed;
        cmt_extension_get_stream_cb get_stream;
        cmt_extension_get_name_cb get_name;
        cmt_extension_get_mzfheader_cb get_mzfheader;
        cmt_extension_get_playfile_name_cb get_playfile_name;
    } st_CMT_EXTENSION;

#ifdef __cplusplus
}
#endif

#endif /* CMT_EXTENSION_H */

