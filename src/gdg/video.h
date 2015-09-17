/* 
 * File:   video.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. června 2015, 15:37
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

#ifndef VIDEO_H
#define	VIDEO_H

#ifdef	__cplusplus
extern "C" {
#endif


    /*
     *
     * 	Generator video signalu Sharp RGBI PAL (JP1 spojeno - zakladni nastaveni pro Evropu):
     *
     *
     *
     * 	Radkove casovani:
     *	=================
     *
     * 	Hsync:		   80 T
     * 	Back Porch:	  106 T
     * 	Video Enable:	  928 T
     * 	Front Porch:	   22 T
     *  	-----------------------
     * 	Cely radek:	1 136 T
     *
     *
     * 	Snimkove casovani:
     * 	==================
     *
     * 	Vsync:		  3 408 T	( 3  radky )
     * 	Back porch:	 21 586 T	( 19 radku + 2 T )
     * 	Video enable:	326 032 T	( 287 radku )
     * 	Front porch:	  3 406 T	( 3 radky )
     * 	---------------------------------------------------
     * 	Cely snimek:	354 432 T	( 312 radku )
     *
     *
     * 	Horni border:	136 T + 45 radku 
     * 	Screen:		200 radku
     * 	Dolni border:	41 radku + 792 T
     *
     *
     * 	Levy border:	154 T
     * 	Screen:		640 T
     * 	Pravy border:	134 T
     *
     *
     * 
     * Udalosti signalu:
     * 
     * vvv - visible rows (top border + screen + botom border)
     * sss - screen rows
     * aaa - all beam rows
     * 
     * 0, 0       - pocatecni pixel obrazu
     * vvv, 0     - zacatek leveho borderu
     * vvv, 153   - posledni pixel leveho borderu (154 pixelu)
     * sss, 154   - pocatek screenu
     * 45, 790    - konec (1) PIOZ80 PA5 (VBLN) (112 radku == 127 232 pixelu, neaktivni: 200 radku == 227 200 pixelu)
     * 245, 790   - zacatek (0) PIOZ80 PA5 (VBLN)
     * 290, 790   - zacatek (0) real_Vsync
     * 293, 790   - konec (1) real_Vsync (3 radky == 3 408 pixelu)
     * 0, 792     - beam_enabled_start - az tady se zacina kreslit prvni radek horniho borderu
     * 288, 792   - beam_enabled_end - uz tady konci posledni zobrazeny radek dolniho borderu
     * sss, 793   - posledni pixel screenu (640 pixelu)
     * sss, 794   - pocatek praveho borderu
     * vvv, 928   - konec praveho borderu (134 pixelu) posledni zobrazeny pixel na radku
     * aaa, 950   - zacatek (0) real_Hsync
     * aaa, 1030  - konec (1) real_Hsync (80 pixelu)
     * aaa, 1078  - konec (1) sts_Hsync (128 pixelu)
     * aaa, 1135  - posledni pixel obrazoveho radku (1136 pixelu)
     * 
     * 
     * Vazby:
     * 
     * CLK1 - real HSync (zajima nas jen sestupna hrana)
     * PIO8255_PC7 - VBLN
     * PIOZ80_PA5 - VBLN
     * 
     */


    /* viditelny obraz */
#define DISPLAY_BORDER_LEFT_WIDTH       154
#define DISPLAY_SCREEN_WIDTH            640
#define DISPLAY_BORDER_RIGHT_WIDTH      134

#define DISPLAY_BORDER_TOP_HEIGHT       46
#define DISPLAY_SCREEN_HEIGHT           200
#define DISPLAY_BORDER_BOTOM_HEIGHT     42


    /* celkove rozmery displeje */
#define DISPLAY_VISIBLE_WIDTH               ( DISPLAY_BORDER_LEFT_WIDTH + DISPLAY_SCREEN_WIDTH + DISPLAY_BORDER_RIGHT_WIDTH )
#define DISPLAY_VISIBLE_HEIGHT              ( DISPLAY_BORDER_TOP_HEIGHT + DISPLAY_SCREEN_HEIGHT + DISPLAY_BORDER_BOTOM_HEIGHT )



#define DISPLAY_BORDER_TOP_WIDTH            DISPLAY_VISIBLE_WIDTH
#define DISPLAY_BORDER_TOP_FIRST_COLUMN     0
#define DISPLAY_BORDER_TOP_LAST_COLUMN      ( DISPLAY_VISIBLE_WIDTH - 1 )
#define DISPLAY_BORDER_TOP_FIRST_ROW        0
#define DISPLAY_BORDER_TOP_LAST_ROW         ( DISPLAY_BORDER_TOP_HEIGHT - 1 )

#define DISPLAY_SCREEN_FIRST_COLUMN         ( DISPLAY_BORDER_LEFT_WIDTH )
#define DISPLAY_SCREEN_LAST_COLUMN          ( DISPLAY_SCREEN_FIRST_COLUMN + DISPLAY_SCREEN_WIDTH - 1 )
#define DISPLAY_SCREEN_FIRST_ROW            ( DISPLAY_BORDER_TOP_LAST_ROW + 1 )
#define DISPLAY_SCREEN_LAST_ROW             ( DISPLAY_SCREEN_FIRST_ROW + DISPLAY_SCREEN_HEIGHT - 1 )

#define DISPLAY_BORDER_LEFT_HEIGHT          DISPLAY_SCREEN_HEIGHT
#define DISPLAY_BORDER_LEFT_FIRST_COLUMN    0
#define DISPLAY_BORDER_LEFT_LAST_COLUMN     ( DISPLAY_BORDER_LEFT_WIDTH - 1 )
#define DISPLAY_BORDER_LEFT_FIRST_ROW       ( DISPLAY_SCREEN_FIRST_ROW )
#define DISPLAY_BORDER_LEFT_LAST_ROW        ( DISPLAY_SCREEN_LAST_ROW )

#define DISPLAY_BORDER_RIGHT_HEIGHT          DISPLAY_SCREEN_HEIGHT
#define DISPLAY_BORDER_RIGHT_FIRST_COLUMN   ( DISPLAY_SCREEN_LAST_COLUMN + 1 )
#define DISPLAY_BORDER_RIGHT_LAST_COLUMN    ( DISPLAY_BORDER_RIGHT_FIRST_COLUMN + DISPLAY_BORDER_RIGHT_WIDTH - 1 )
#define DISPLAY_BORDER_RIGHT_FIRST_ROW      ( DISPLAY_SCREEN_FIRST_ROW )
#define DISPLAY_BORDER_RIGHT_LAST_ROW       ( DISPLAY_SCREEN_LAST_ROW )

#define DISPLAY_BORDER_BOTOM_WIDTH          DISPLAY_VISIBLE_WIDTH
#define DISPLAY_BORDER_BOTOM_FIRST_COLUMN   0
#define DISPLAY_BORDER_BOTOM_LAST_COLUMN    ( DISPLAY_VISIBLE_WIDTH - 1 )
#define DISPLAY_BORDER_BOTOM_FIRST_ROW      ( DISPLAY_SCREEN_LAST_ROW + 1 )
#define DISPLAY_BORDER_BOTOM_LAST_ROW       ( DISPLAY_BORDER_BOTOM_FIRST_ROW + DISPLAY_BORDER_BOTOM_HEIGHT - 1 )

#define DISPLAY_VISIBLE_LAST_COLUMN         ( DISPLAY_VISIBLE_WIDTH - 1 )
#define DISPLAY_VISIBLE_LAST_ROW            ( DISPLAY_VISIBLE_HEIGHT - 1 )

    /* skutecny Hsync */
#define BEAM_H_SYNC_TICKS               80
#define BEAM_H_BACK_PORCH_TICKS         106
#define BEAM_H_VIDEO_ENABLE_TICKS       DISPLAY_VISIBLE_WIDTH
#define BEAM_H_FRONT_PORCH_TICKS        22

#define BEAM_TOTAL_COLS                  ( BEAM_H_SYNC_TICKS + BEAM_H_BACK_PORCH_TICKS + BEAM_H_VIDEO_ENABLE_TICKS + BEAM_H_FRONT_PORCH_TICKS )
#define BEAM_TOTAL_ROWS                  312
#define PICTURE_TICKS                    ( BEAM_TOTAL_ROWS * BEAM_TOTAL_COLS )

#define BEAM_V_SYNC_TICKS               ( 3 * BEAM_TOTAL_COLS )
#define BEAM_V_BACK_PORCH_TICKS         ( 19 * BEAM_TOTAL_COLS + 2 )
#define BEAM_V_VIDEO_ENABLE_TICKS       ( 287 * BEAM_TOTAL_COLS )
#define BEAM_V_FRONT_PORCH_TICKS        ( 3 * BEAM_TOTAL_COLS )

#define BEAM_VSYNC_START_ROW            270
#define BEAM_VSYNC_END_ROW              311

#define BEAM_SCREEN_TICKS               ( BEAM_V_SYNC_TICKS + BEAM_V_BACK_PORCH_TICKS + BEAM_V_VIDEO_ENABLE_TICKS + BEAM_V_FRONT_PORCH_TICKS )

#define BEAM_HBLN_START_COLUMN          ( DISPLAY_SCREEN_LAST_COLUMN - 3 )

#define BEAM_ROW(ticks) ( ticks / BEAM_TOTAL_COLS )
#define BEAM_COL(ticks) ( ticks % BEAM_TOTAL_COLS )


#ifdef	__cplusplus
}
#endif

#endif	/* VIDEO_H */

