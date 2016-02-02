/***********************************************************************************
** Copyright (c) 2011, University of British Columbia (UBC); All rights reserved. **
**                                                                                **
** Redistribution  and  use  in  source   and  binary  forms,   with  or  without **
** modification,  are permitted  provided that  the following conditions are met: **
**   * Redistributions   of  source   code  must  retain   the   above  copyright **
**     notice,  this   list   of   conditions   and   the  following  disclaimer. **
**   * Redistributions  in  binary  form  must  reproduce  the  above   copyright **
**     notice, this  list  of  conditions  and the  following  disclaimer in  the **
**     documentation and/or  other  materials  provided  with  the  distribution. **
**   * Neither the name of the University of British Columbia (UBC) nor the names **
**     of   its   contributors  may  be  used  to  endorse  or   promote products **
**     derived from  this  software without  specific  prior  written permission. **
**                                                                                **
** THIS  SOFTWARE IS  PROVIDED  BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" **
** AND  ANY EXPRESS  OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE **
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE **
** DISCLAIMED.  IN NO  EVENT SHALL University of British Columbia (UBC) BE LIABLE **
** FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL **
** DAMAGES  (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR **
** SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER **
** CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN CONTRACT, STRICT LIABILITY, **
** OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE **
** OF  THIS SOFTWARE,  EVEN  IF  ADVISED  OF  THE  POSSIBILITY  OF  SUCH  DAMAGE. **
***********************************************************************************/

/***********************************************************************************
** annealing.c:  Simulated-Annealing Cell-Based Placement Tool - main             **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**               University of British Columbia (UBC), March 2011                 **
***********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include "geometry.h"
#include "graphics.h"
#include "allocation.h"
#include "floorplan.h"
#include "array.h"

/* global variables declaration									*/
floorplan *gfp					; /* global floorplan database	*/
char	   gFooterLabel[1024]=""; /* global footer text message	*/
char	   gHeaderLabel[1024]=""; /* global header text message	*/
float	   gWorldStep			; /* grid step					*/
float	   gWorldX				; /* world X dimension			*/
float	   gWorldY				; /* world X dimension			*/


/* commandline arguments parsing, sets global variables			*/
/* returns filename index in argv								*/
unsigned int commandlineParse(int argc, char *argv[]			);
/* results used to set the following global variables			*/
int          gVerbose     = 0	; /* enable verbose logging		*/
int          gGUI         = 0	; /* enable GUI mode			*/
int          gPostScript  = 0	; /* enable postscript plotting	*/
unsigned int gRefreshRate = 10	; /* refresh rate / temp change	*/
double       gInitTemp    = 200	; /* initial temperature		*/
double       gFreezeTemp  = 5e-6; /* freezing temperature		*/
double       gCoolRate    = 0.95; /* cooling rate				*/
unsigned int gMoveTemp    = 10	; /* moves per temperature		*/

/* functions associated with buttons																	*/
inline void enablePS (void (*drawScreen_ptr)(void)) {gPostScript=1        ;} /* enable  postscript		*/
inline void disablePS(void (*drawScreen_ptr)(void)) {gPostScript=0        ;} /* disable postscript		*/
inline void run1     (void (*drawScreen_ptr)(void)) {gRefreshRate=1       ;} /* refresh every 1 temp	*/
inline void run10    (void (*drawScreen_ptr)(void)) {gRefreshRate=10      ;} /* refresh every 10 temp	*/
inline void run100   (void (*drawScreen_ptr)(void)) {gRefreshRate=100     ;} /* refresh every 100 temp	*/
inline void run1000  (void (*drawScreen_ptr)(void)) {gRefreshRate=1000    ;} /* refresh every 1000 temp	*/
inline void runAll   (void (*drawScreen_ptr)(void)) {gRefreshRate=UINT_MAX;} /* refresh at end only		*/

/* redrawing routine for still pictures. Redraw if user changes the window								*/
void drawScreen () { clearscreen(); fpDraw(gfp,gWorldX,gWorldY); } /* clear and redraw					*/

/* called whenever event_loop gets a button press in the graphics area.									*/
void buttonPress (float x, float y, int flags) { }

/* receives the current mouse position in the current world as in init_world							*/
void mouseMove (float x, float y) { }

/* function to handle keyboard press event, the ASCII character is returned								*/
void keyPress (int i) { }

/* show global message, wait until 'Proceed' pressed then draw screen									*/
void waitLoop () {
	event_loop(buttonPress,mouseMove,keyPress,drawScreen);
	drawScreen();
	update_message(gFooterLabel);
}


/***********************************************  main  *************************************************/

int main(int argc, char *argv[]) {

	/* parse commandline returns filename index in argv */
	/* create floorplan data structure					*/
	gfp=fpCreate(argv[commandlineParse(argc,argv)]);

	if (gPostScript	) gGUI 		= 1 ;	/* enable gui if post script required		*/
	if (!gGUI		) gVerbose	= 1 ;	/* enable verbose if if gui is not enabled	*/

	/* change random seed into current time */
	srandom(time(NULL));

	if (gGUI) {

		/*  calculating world and drawing dimentions							*/
		gWorldStep=MIN((880/((gfp->ny)*2)),(1000/((gfp->nx)+1))); /* grid step	*/
		gWorldX   =((gfp->nx)+1)*gWorldStep+240; /* world X dimension 			*/
		gWorldY   =(gfp->ny)*2*gWorldStep+180  ; /* world Y dimension 			*/	

		/* initialize display with WHITE 1000x1000 background */
		init_graphics((char*)"Simulated-Annealing Cell-Based Placement Tool", WHITE, NULL);
		init_world (0.,0.,gWorldX,gWorldY);

		/* Create new buttons */
		create_button ((char*)"Window"    , (char*)"---1"      , NULL     ); /* Separator				*/
		create_button ((char*)"---1"      , (char*)"Enable  PS", enablePS ); /* enable PS				*/
		create_button ((char*)"Enable  PS", (char*)"Disable PS", disablePS); /* disable PS				*/
		create_button ((char*)"Disable PS", (char*)"Run 1"     , run1     ); /* refresh every 1    temp	*/
		create_button ((char*)"Run 1"     , (char*)"Run 10"    , run10    ); /* refresh every 10   temp	*/
 		create_button ((char*)"Run 10"    , (char*)"Run 100"   , run100   ); /* refresh every 100  temp	*/
		create_button ((char*)"Run 100"   , (char*)"Run 1000"  , run1000  ); /* refresh every 1000 temp	*/
		create_button ((char*)"Run 1000"  , (char*)"Run All"   , runAll   ); /* refresh at end only		*/
	}

	/* invoke simulated-annealing placement with designated parameters */
	fpAnneal(gfp
			,(gMoveTemp*(gfp->cellsN)^(4/3))
			,gInitTemp*(gfp->bbox)
			,gCoolRate
			,gFreezeTemp/(gfp->bbox/gfp->netsN)
	);

	/* finished! wait still until 'Exit" is pressed */
	if (gGUI)
		while(1) waitLoop();

	/* free database */
	fpDelete(gfp);


	return 1;
}

/***********************************************  main  *************************************************/


/* commandline arguments parsing						*/
/* results used to set global variables					*/
/* returns filename index in argv						*/
unsigned int commandlineParse(int argc, char *argv[]	){

	/* arguments parsing */
	int argi; /* arguments index */
	int fileNameArgInd=-1; /* file name argument index */
	for(argi=1;argi<argc;argi++) { /* check all argument */
		if (argv[argi][0]=='-') { /* switch argument */
			switch (tolower(argv[argi][1])) { /* consider first letter */

				/* help */
				case 'h':	printf("\nSimulated-Annealing Cell-Based Routing Tool\n"							);
							printf("Usage:\n"																	);
							printf("  annealing INFILE [OPTIONS]\n"												);
							printf("Options:\n"																	);
							printf("  -help       (also -h): Print this message\n"								);
							printf("  -gui        (also -g): Enable graphical user interface (GUI) \n"			);
							printf("                         Automatically enabled if postscript is enabled\n"	);
							printf("  -verbose    (also -v): Enable verbose logging,\n"							);
							printf("                         Automatically enabled if GUI is disabled\n"		);
							printf("  -postscript (also -p): Generate PostScript every screen refresh\n"		);
							printf("                         Will enable GUI mode\n"							);
							printf("  -refresh    (also -r): Refresh rate, followed by an integer indicating\n"	);
							printf("                         the number of temperature change loops between\n"	);
							printf("                         screen refresh; waits for 'Proceed' to continue\n"	);
							printf("                         Default is 10\n"									);
							printf("  -inittemp   (also -i): Initial temperature, followed by a double float\n"	);
							printf("                         Default is 200, multiplied by initialCost\n"		);
							printf("                         (same as VPR from utoronto, scaled)\n"						);
							printf("  -freezetemp (also -f): Freezing temperature, followed by a double float\n");
							printf("                         Default is 5e-6, multiplied by initialCost/net#\n"	);
							printf("                         (same as VPR from utoronto, scaled)\n"						);
							printf("  -coolrate   (also -c): Cooling rate, followed by a double float\n"		);
							printf("                         Default is 0.95 (same as VPR from utoronto)\n"		);
							printf("  -movetemp   (also -m): Moves per temperature change, followed by an int\n");
							printf("                         Default is 10, multiplied by (cells#)^(4/3)\n"		);
							printf("                         (same as VPR from utoronto)\n"						);
							printf("Input file syntax:\n"														);
							printf("  <CELLS#> <NET#> <ROWS#> <COLUMNS#>\n"										);
							printf("  <#CELLS_CONNECTED_TO_NET_1> <LIST_OF_CELLS_CONNECTED_TO_NET_1>\n"			);
							printf("  <#CELLS_CONNECTED_TO_NET_2> <LIST_OF_CELLS_CONNECTED_TO_NET_2>\n"			);
							printf("  ...\n"																	);
							printf("  <#CELLS_CONNECTED_TO_NET_n> <LIST_OF_CELLS_CONNECTED_TO_NET_n>\n"			);
							printf("Examples:\n"																);
							printf("  annealing cps.txt (using default options)\n"								);
							printf("  annealing cps.txt -gui -movetemp 1 (GUI enabled, refresh every temp.)\n"	);
							printf("  annealing cps.txt -gui -verbose -postscript -inittemp 300    \\\n"		);
							printf("                    -freezetemp 1e-6 -coolrate .97 -movetemp 100\n"			);
							printf("  annealing cps.txt -g -v -p -i 300 -f 1e-6 -c .97 -m 100 (same as above)\n");
							printf("Report bugs to <ameer.abdelhadi@gmail.com>\n\n"								);
							exit(1);

				/* verbose mode*/
				case 'v':	gVerbose=1;			/* set verbose */
							break;
				/* GUI mode */
				case 'g':	gGUI=1;			/* set verbose */
							break;

				/* PostScript mode */
				case 'p':	gPostScript=1;
							break;

				/* refresh rate */
				case 'r':	argi++;												/* next argument */
							if (sscanf(argv[argi],"%u",&gRefreshRate) == 0) {	
								printf("-E- Commandline error: -refreshrate should be followed by a positive integer! Exiting...\n");
								exit(-1);
							}
							break;

				/* initial temperature */
				case 'i':	argi++;												/* next argument */
							if (sscanf(argv[argi],"%lf",&gInitTemp) == 0) {	
								printf("-E- Commandline error: -refreshrate should be followed by a positive integer! Exiting...\n");
								exit(-1);
							}
							break;

				/* freezing temperature */
				case 'f':	argi++;												/* next argument */
							if (sscanf(argv[argi],"%lf",&gFreezeTemp) == 0) {	
								printf("-E- Commandline error: -refreshrate should be followed by a positive integer! Exiting...\n");
								exit(-1);
							}
							break;

				/* cooling rate */
				case 'c':	argi++;												/* next argument */
							if (sscanf(argv[argi],"%lf",&gCoolRate) == 0) {	
								printf("-E- Commandline error: -refreshrate should be followed by a positive integer! Exiting...\n");
								exit(-1);
							}
							break;

				/* moves per temperature change */
				case 'm':	argi++;												/* next argument */
							if (sscanf(argv[argi],"%u",&gMoveTemp) == 0) {	
								printf("-E- Commandline error: -refreshrate should be followed by a positive integer! Exiting...\n");
								exit(-1);
							}
							break;

				/* unknown argument */
				default :	printf("-E- unknown argument %s\n",argv[argi]);
							exit(-1);
			} /* switch */
		} else fileNameArgInd = argi; /* file name argument index */
	} /* arguments loop (for) */

	/* check if infile is supplied */
	if (fileNameArgInd<0) {printf(" -E- infile should be supplied\n"); exit(-1);}

	return fileNameArgInd;
}
