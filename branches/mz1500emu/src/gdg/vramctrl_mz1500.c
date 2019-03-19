/* 
 * File:   vramctrl_mz1500.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 18:49
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
 * 	Radic pameti VRAM
 *
 *
 *
 *	V rezimu MZ1500 je pouzita jedna pametova banka VRAM (4kB), ktera se mapuje:
 *
 *	MZ ADDR		VRAM ADDR
 *	============================================
 *	0xd000:		0x1000 - 0x07ff: Character VRAM
 *		        0x1800 - 0x0fff: Attribute VRAM
 *
 *
 */

#include "mz800emu_cfg.h"

#include <stdio.h>

#ifdef MACHINE_EMU_MZ1500

#include "vramctrl_mz1500.h"

st_VRAMCTRL_MZ1500 g_vramctrl_mz1500;


void vramctrl_mz1500_reset ( void ) {
    g_vramctrl_mz1500.wr_latch_is_used = 0;
}


#endif
