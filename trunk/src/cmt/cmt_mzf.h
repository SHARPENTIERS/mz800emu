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

#include "cmt_extension.h"
#include "libs/generic_driver/generic_driver.h"
#include "libs/mztape/mztape.h"
#include "libs/mzf/mzf.h"
#include "libs/cmt_stream/cmt_bitstream.h"


    extern st_CMTEXT *g_cmtmzf_extension;


    typedef struct st_CMT_MZF_FILESPEC {
        st_MZF_HEADER *hdr;
        st_MZTAPE_MZF *mztmzf;
    } st_CMT_MZF_FILESPEC;

    extern void cmtmzf_filespec_destroy ( st_CMT_MZF_FILESPEC *filespec );
    extern void cmtmzf_init ( void );
    extern void cmtmzf_exit ( void );
    extern int cmtmzf_open ( st_CMTEXT *cmtext, st_HANDLER *h, en_MZTAPE_SPEED mztape_speed );
    extern st_CMT_BITSTREAM* cmtmzf_generate_bitstream ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_SPEED mztape_speed );
    extern st_CMT_VSTREAM* cmtmzf_generate_vstream ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_SPEED mztape_speed );


#define CMTMZF_DEFAULT_STREAM_RATE 44100
//#define CMTMZF_DEFAULT_STREAM_RATE 192000


#ifdef __cplusplus
}
#endif

#endif /* CMT_MZF_H */

