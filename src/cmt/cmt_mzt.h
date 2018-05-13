/* 
 * File:   cmt_mzt.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. května 2018, 7:27
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


#ifndef CMT_MZT_H
#define CMT_MZT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "cmt_mzf.h"

    extern st_CMTEXT *g_cmtmzt_extension;


    typedef struct st_CMT_MZT_CONSPEC {
        int count_files;
        st_CMT_MZF_FILESPEC **fspecs;
    } st_CMT_MZT_CONSPEC;


    extern void cmtmzt_init ( void );
    extern void cmtmzt_exit ( void );

#ifdef __cplusplus
}
#endif

#endif /* CMT_MZT_H */

