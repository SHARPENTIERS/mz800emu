/* 
 * File:   cmt_wav.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 22. dubna 2018, 9:38
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


#ifndef CMT_WAV_H
#define CMT_WAV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libs/cmt_stream/cmt_stream.h"
#include "cmt_extension.h"


    typedef struct st_CMT_WAV {
        st_CMT_STREAM *stream;
        char *playfile_name;
    } st_CMT_WAV;

    extern st_CMT_WAV g_cmt_wav;
    extern st_CMT_EXTENSION g_cmt_wav_extension;

    extern void cmt_wav_init ( void );
    extern void cmt_wav_exit ( void );

#ifdef __cplusplus
}
#endif

#endif /* CMT_WAV_H */

