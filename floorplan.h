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
** floorplan.c:  floorplan, cell, net, and simulated-annealing module	            **
**               implements Simulated-Annealing Cell-Based Placement              **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**               University of British Columbia (UBC), March 2011                 **
***********************************************************************************/


#ifndef __FLOORPLAN_HEADER__
#define __FLOORPLAN_HEADER__

/* net data structure																	*/
struct netStruct {
	unsigned int  cellsN;	/* the amount of cells connected to this net				*/
	unsigned int *cells ;	/* a listing of cells id's connected with this net			*/
	unsigned int  bbox  ;	/* 1/2 perimeter of the smallest bounding box with all pins	*/
};
typedef struct netStruct net;

/* cell data structure																	*/
struct cellStruct {
	unsigned int  netsN   ; /* the amount of nets connected to this cell (pins#)		*/
	unsigned int *nets    ;	/* a listing of the nets connected with this cell			*/
};
typedef struct cellStruct cell;

/* floorplan data structure																*/
struct floorplanStruct {
	unsigned int  nx		; /* amount of rows in the floorplan						*/
	unsigned int  ny		; /* amount of columns in the floorplan						*/
	unsigned int  cellsN	; /* amount of cells in the floorplan						*/
	unsigned int  netsN		; /* amount of nets in the floorplan						*/
	unsigned int  bbox		; /* sum of bbox 1/2 perimeter for all nets in the floorplan*/
	cell         *cells		; /* a listing of the cells int the floorplan				*/
	unsigned int *cellsOrder; /* a listing of the cells int the floorplan				*/
	net          *nets		; /* a listing of the cells int the floorplan				*/
};
typedef struct floorplanStruct floorplan;

/* external global variables															*/
extern				floorplan *gfp	  ; /* global floorplan db   	  (see annealing.c)	*/
extern int			gVerbose    	  ; /* verbose status       	  (see annealing.c)	*/
extern int			gGUI         	  ; /* enable GUI mode      	  (see annealing.c)	*/
extern int			gPostScript   	  ;	/* enable postscript     	  (see annealing.c)	*/
extern unsigned int	gRefreshRate	  ; /* refreshRate/tempChange	  (see annealing.c)	*/
extern char			gFooterLabel[1024]; /* global footer text message (see annealing.c)	*/
extern char			gHeaderLabel[1024]; /* global header text message (see annealing.c)	*/
extern void			drawScreen()  	  ; /* draw graphics routine	  (see annealing.c)	*/
extern void			waitLoop()		  ; /* show message, wait for 'Proceed' to draw 	*/


/* returns a new allocated floorplan data structure, parsed from infile 				*/
floorplan	*fpCreate(const char *fileName											);

/* free a floorplan data structure														*/
void		 fpDelete(floorplan  *fp													);

/* dump a floorplan data structure to stdout - for verification purpose					*/
void		 fpDump(floorplan  *fp														);

/* update net's bbox in the floorplan database											*/
unsigned int fpNetUpdateBBox(floorplan *fp, unsigned int netID							);

/* update the bbox of all nets connected to this cell in the floorplan database			*/
void		 fpCellUpdateBBox(floorplan *fp, unsigned int cellId						);

/* update cost of all nets in floorplan													*/
void		 fpAllNetsUpdateBBox(floorplan *fp											);

/* draw floorplan grid using EasyGl graphics module from (x0,y0) to (x1,y1)				*/
void		 fpDrawGrid(floorplan *fp, float x0, float y0, float x1, float y1			);

/* draw floorplan using EasyGl graphics module. World size is xWorld*yWorld				*/
void		 fpDraw	(floorplan *fp, float xWorld, float yWorld							);

/* swap the order of two cells and update the cost										*/
void		 fpSwapCells(floorplan *pf, unsigned int cellId1, unsigned int cellId2		);

/* simulated annealing placement 														*/
void		 fpAnneal(floorplan *fp,  unsigned int movesPerTemp	, double initTemp		,
									  double coolingRate		, double freezingTemp	);

#endif /* __FLOORPLAN_HEADER__ */


