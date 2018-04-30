/* 
 * File:   cmt_mzf.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 24. dubna 2018, 18:03
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


#ifndef CMT_MZF_H
#define CMT_MZF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libs/cmt_stream/cmt_stream.h"
#include "libs/mztape/mztape.h"
#include "cmt_extension.h"


    typedef struct st_CMT_MZF {
        st_CMT_STREAM *stream;
        st_MZTAPE_MZF *mztmzf;
        st_MZF_HEADER hdr;
        char *playfile_name;
    } st_CMT_MZF;

    extern st_CMT_MZF g_cmt_mzf;
    extern st_CMT_EXTENSION g_cmt_mzf_extension;

    extern void cmt_mzf_init ( void );
    extern void cmt_mzf_exit ( void );

#define CMT_MZF_DEFAULT_MZTAPE_RATE 44100

#ifdef __cplusplus
}
#endif

#endif /* CMT_MZF_H */

