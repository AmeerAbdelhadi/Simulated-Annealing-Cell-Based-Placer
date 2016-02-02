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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include "floorplan.h"
#include "graphics.h"
#include "array.h"
#include "allocation.h"
#include "random.h"
#include "geometry.h"

/* global variables																		*/
double		 gLogCurTemp   ; /* log(temperature), for scale drawing 					*/
unsigned int gCurCost  = 1 ; /* cost of current solution								*/
unsigned int gInitCost = 1 ; /* cost of initiale solution								*/

/* returns a new allocated floorplan data structure, parsed from infile 				*/
floorplan *fpCreate(const char *fileName												){

	FILE		*infile			   ; /* input file 										*/
	floorplan	*fp=NULL		   ; /* floorplan data structure 						*/
	char		 line[1024]		   ; /* chars array holds one file line 				*/
	unsigned int buf[1024]		   ; /* unsigned int array holds int values of one line */
	int			 bufInd=-1		   ; /* index for previous buffer 						*/
	char		 delims[]=" \t\r\n"; /* delimiters for tokens: space, tap, and new line */
	char		*strToken=NULL	   ; /* string token from input file line 				*/
	unsigned int intToken		   ; /* unsigned integer token 							*/
	unsigned int lineNum=1		   ; /* current parsed line numer 						*/
	unsigned int neti=0,celli=0	   ; /* net and cell counters 							*/
	unsigned int curCell		   ; /* current cell 									*/
	unsigned int cellx,celly	   ; /* cell palce 										*/
	unsigned int nx,ny	   		   ; /* floorplan size 									*/
	unsigned int cellsN=0,netsN=0   ; /* number for cells/nets 							*/
	unsigned int fpSize			   ; /* floorplan area 									*/
	enum parseStates				 /* parsing states enum types						*/
			{HEADER,NET,EXTRA}		 /* header line, net line, or extra line			*/
				parseState=HEADER  ; /* parsing state 									*/
	

	/* open input file */
	infile=fopen(fileName,"r");
	if(infile==NULL) { /* open failed */
		printf("-E- File open error %s! Exiting...\n", fileName); exit(-1);
	}

	/* read a line, until end of file */
	while (fgets(line,sizeof line,infile) != NULL ) {	

		bufInd=-1;
		strToken = strtok( line, delims );     /* read token from line               */
		while(strToken != NULL) {              /* read all tokens                    */
			if (sscanf(strToken,"%u",&intToken) == 0) {			
				printf("-E- Parsing error: file %s, line %u: value is not positive integer! Exiting...\n",fileName,lineNum);
				exit(-1);
			}
        	bufInd++						; /* holds number of tokens */
			buf[bufInd]=intToken			; /* hold integer tokens	*/	
			strToken   = strtok(NULL,delims); /* next token 			*/
		}
		if (bufInd<0) continue; /* empty line */

		switch(parseState) {

		  /*read gird size*/
		  case HEADER:
			if (bufInd != 3) {
				printf("-E- Parsing error: file %s, line %u should include four integers! Exiting...\n",fileName,lineNum);
				exit(-1);
			}
			
			/* grid info */
			cellsN = buf[0];
			netsN  = buf[1];
			ny     = buf[2];
            nx     = buf[3];
			fpSize = nx*ny;

			if (cellsN>fpSize) {
				printf("-E- Floorplan error: %u cell sites can't fit %u cells! Exiting...\n",fpSize,cellsN);
				exit(-1);				
			}
			
			/* allocate and initialze floorplan */
			fp          	= (floorplan*)safeMalloc(sizeof(floorplan));
			fp->nx			= nx;
			fp->ny			= ny;
			fp->cellsN		= cellsN;
			fp->netsN		= netsN;
			fp->bbox		= 0;
			fp->cells		= (cell*)safeMalloc(sizeof(cell)*cellsN);
			fp->cellsOrder	= arrAlloc(fpSize);
			fp->nets		= (net*)safeMalloc(sizeof(net)*netsN);
			
			/* initialize each cell to zero nets */
			for (celli=0;celli<cellsN;celli++) fp->cells[celli].netsN=0;

			/* initialize to random order */
			arrRandInit(fp->cellsOrder,fpSize,0,fpSize-1);		

			parseState=NET;

			break;

		  /* read a net */
		  case NET:
			if (neti==netsN-1) parseState=EXTRA; /* if finished check extra lines */
			if (bufInd < 0) { 
				printf("-E- Parsing error: file %s, line %u: Should include at least one integer! Exiting...\n",fileName,lineNum);
				fpDelete(fp);
				exit(-1);
			}
			if (bufInd != buf[0]) { /* incomplete */
				printf("-E- Parsing error: file %s, line %u: Should include %u integer numbers! Exiting...\n",fileName,lineNum, 1+buf[0]);
				fpDelete(fp);
				exit(-1);
			}
			
			/* allocate a net in floorplan nets database */
			fp->nets[neti].cellsN = buf[0];
			fp->nets[neti].cells  = arrAlloc(buf[0]);
			
			for(celli=0;celli<buf[0];celli++) {
				fp->nets[neti].cells[celli] = buf[celli+1];
				(fp->cells[buf[celli+1]].netsN)++;
			}

			neti++; /* next net */

			break;

		  /* check extra lines */
		  case EXTRA:
			if (bufInd != -1) { 
				printf("-E- Parsing error in file %s, line %u: Extra line! Exiting...\n",fileName,lineNum);
				fpDelete(fp);
				exit(-1);
			}

		} /* switch */
	lineNum++; /* next line */
	} /* while fgets */

	/* cell initialization */
	cellx=0;celly=0;
	for(celli=0;celli<cellsN;celli++) {

		/* allocate nets list for each cell */
		fp->cells[celli].nets = arrAlloc(fp->cells[celli].netsN);

		/* reset nets numbers for each cell to use as array index */
		fp->cells[celli].netsN = 0;

	}

	/* upadte nets list for each cell */
	for(neti=0;neti<netsN;neti++){
		for(celli=0;celli<(fp->nets[neti].cellsN);celli++) {
			curCell = fp->nets[neti].cells[celli];
			fp->cells[curCell].nets[fp->cells[curCell].netsN] = neti;
			(fp->cells[curCell].netsN)++;

		}
	}

	/* update nets bbox */
	fpAllNetsUpdateBBox(fp);

	fclose(infile);
	return fp;

}	/* fpCreate	*/

/* free a floorplan data structure	*/
void fpDelete(floorplan *fp) {

	unsigned int celli,neti; /* indexes */

	/* free net arrays form cells */
	for(celli=0;celli<(fp->cellsN);celli++) {
		safeFree(fp->cells[celli].nets);
	}
	
	/* free cell arrays from nets */
	for(neti=0;neti<(fp->netsN);neti++) {
		safeFree(fp->nets[neti].cells);
	}

	safeFree(fp->cells);
	safeFree(fp->nets );

}	/* fpDelete */

/* dump a floorplan data structure to stdout - for verification purpose	*/
void fpDump (floorplan  *fp) {

	unsigned int neti,celli	; /* indexes 		*/
	point 		 cellLoc	; /* cell location 	*/

	/* print floorplan details */
	printf("Floorplan:\n");
	printf("\tSize   : (%u,%u)\n",fp->nx,fp->ny);
	printf("\tCells #: %u\n",fp->cellsN);
	printf("\tNets  #: %u\n",fp->netsN);
	printf("\tbbox  #: %u\n",fp->bbox);

	/* list nets */
	printf("Nets:\n");
	for(neti=0;neti<(fp->netsN);neti++) {
		printf("\tNet#%u: bbox: %u, cells #: %u, cells:",neti,fp->nets[neti].bbox,fp->nets[neti].cellsN);
		arrDump(fp->nets[neti].cells,fp->nets[neti].cellsN," ");
		printf("\n");
	}

	/* list cells */
	printf("Cells:\n");
	for(celli=0;celli<(fp->cellsN);celli++) {
		cellLoc = index1Dto2D(fp->cellsOrder[celli],fp->nx,fp->ny);
		printf("\tCell#%u: location (%u,%u), nets #: %u, nets:",
				celli, cellLoc.x, cellLoc.y,
				fp->cells[celli].netsN							);
		arrDump(fp->cells[celli].nets,fp->cells[celli].netsN," ");
		printf("\n");
	}

}	/* fpDump */

/* update net's bbox in the floorplan database											*/
unsigned int fpNetUpdateBBox(floorplan *fp, unsigned int netID) {

	net  curNet = fp->nets[netID];
	point *netPoints = (point*)safeMalloc(sizeof(cell)*curNet.cellsN);
	point cellLoc;
	unsigned int curCell, celli, bbox, bboxOld;

	for(celli=0; celli<curNet.cellsN; celli++) {
		curCell = curNet.cells[celli];
		cellLoc = index1Dto2D(fp->cellsOrder[curCell],fp->nx,fp->ny);
		netPoints[celli].x =   cellLoc.x;
		netPoints[celli].y = 2*cellLoc.y; /* double row */
	}
	bbox = pointsBBoxHalfPerimeter(netPoints,curNet.cellsN);
	bboxOld = fp->nets[netID].bbox;
	fp->nets[netID].bbox = bbox;
	fp->bbox += (bbox-bboxOld);
	safeFree(netPoints);

	return bbox;

} /* fpNetUpdateBBox */

/* update the bbox of all nets connected to this cell in the floorplan database			*/
void fpCellUpdateBBox(floorplan *fp, unsigned int cellId) {

	unsigned int neti;

	for(neti=0; neti<(fp->cells[cellId].netsN); neti++) {
		fpNetUpdateBBox(fp, fp->cells[cellId].nets[neti]);
	}

} /* fpCellUpdateBBox */

/* update cost of all nets in floorplan */
void fpAllNetsUpdateBBox(floorplan *fp)  {

	unsigned int neti;

	for(neti=0;neti<(fp->netsN);neti++){
		fpNetUpdateBBox(fp,neti);
	}

} /* fpAllNetsUpdateBBox */

/* draw floorplan grid using EasyGl graphics module from (x0,y0) to (x1,y1)				*/
void fpDrawGrid(floorplan *fp, float x0, float y0, float x1, float y1					){

	unsigned int ny=fp->ny, nx=fp->nx				; /* dimention        				*/
	unsigned int celli,neti							; /* cell/net counters 				*/
	float		 dx=x1-x0, dy=y1-y0					; /* grid dimention 				*/
	float		 step=MIN((dy/(ny*2)),(dx/(nx+1)))	; /* one grid step					*/
	char		 label[16]							; /* general label  				*/
	net			 curNet								; /* current net 					*/
	unsigned int curCell, srcCell					; /* current/source cell number 	*/
	point		 cellLoc							; /* cell location 					*/
	unsigned int srcx,srcy,curx,cury				; /* drawing location 				*/

	/* draw grid background */
	setcolor(LIGHTGREY);
	fillrect(x0,y0,(nx+1)*step+x0,ny*2*step+y0);

	/* draw cells on grid*/
	for(celli=0;celli<(nx*ny);celli++) {
		cellLoc = index1Dto2D(fp->cellsOrder[celli],fp->nx,fp->ny);
		if (celli<(fp->cellsN)) setcolor(BLUE ); /* if cell, paint with blue  */
						   else setcolor(WHITE); /* if void, paint with white */
		fillrect((cellLoc.x+.5)*step+x0,(cellLoc.y*2+.5)*step+y0,(cellLoc.x+1.5)*step+x0,(cellLoc.y*2+1.5)*step+y0);
		setcolor(BLACK);
		drawrect((cellLoc.x+.5)*step+x0,(cellLoc.y*2+.5)*step+y0,(cellLoc.x+1.5)*step+x0,(cellLoc.y*2+1.5)*step+y0);
		if (celli<(fp->cellsN)) { /* if cell, make label */
			setcolor(WHITE);
			sprintf(label,"%u",celli);
			drawtext((cellLoc.x+1)*step+x0,(cellLoc.y*2+0.75)*step+y0,label,step);
		}
	}

	/* draw wires */
	setcolor(RED);
	for(neti=0;neti<(fp->netsN);neti++) {
		curNet = fp->nets[neti];
		srcCell = curNet.cells[0];
		cellLoc = index1Dto2D(fp->cellsOrder[srcCell],fp->nx,fp->ny);
		srcx = cellLoc.x;
		srcy = cellLoc.y;
		fillarc ((srcx+1)*step+x0,(srcy*2+1)*step+y0,step/10,0.,360.);
		for(celli=1;celli<curNet.cellsN;celli++) {
			curCell = curNet.cells[celli];
			cellLoc = index1Dto2D(fp->cellsOrder[curCell],fp->nx,fp->ny);
			curx = cellLoc.x;
			cury = cellLoc.y;
			fillarc ((curx+1)*step+x0,(cury*2+1)*step+y0,step/10,0.,360.);
			drawline((srcx+1)*step+x0,(srcy*2+1)*step+y0,(curx+1)*step+x0,(cury*2+1)*step+y0);		
		}
	}

} /* fpDrawGrid */


/* draw floorplan using EasyGl graphics module. World size is xWorld*yWorld	*/
void fpDraw(floorplan *fp, float xWorld, float yWorld) {

	unsigned int nx=fp->nx, ny=fp->ny;	/* floorplan dimention				*/
	unsigned int i;						/* general counter					*/

	/* Grid parameters														*/
	float x0Grid=0,y0Grid=120,x1Grid=1000,y1Grid=1000; /* grid place		*/
	float dxGrid=x1Grid-x0Grid, dyGrid=y1Grid-y0Grid;  /* grid size			*/
	float step=MIN((dyGrid/(ny*2)),(dxGrid/(nx+1)));   /* grid step			*/

	char  label[1024]	; /* general label									*/
	float scaleStep		; /* a step for temperature and cost scales			*/
	float costReduction	; /* current_cost / initial_cost					*/

	/* draw header, including title */
	setcolor(LIGHTGREY);
	fillrect(0,0,(nx+1)*step+240,40);
	setcolor(BLACK);
	setfontsize(13);
	drawtext(((nx+1)*step+240)/2,20,"Simulated-Annealing Cell-Based Placement",(nx+1)*step+240);

	/* draw second line header, including simulated-annealing parameters */
	setcolor(LIGHTGREY);
	fillrect(0,60,(nx+1)*step,100);
	setcolor(BLACK);
	setfontsize(10);
	drawtext((nx+1)*step/2,80,gHeaderLabel,(nx+1)*step);

	/* draw temperature scale */
	scaleStep=ny*2*step/20;
	setcolor(LIGHTGREY); fillrect((nx+1)*step+20,60 ,(nx+1)*step+120,ny*2*step+120);
	setcolor(WHITE    ); fillrect((nx+1)*step+70,120,(nx+1)*step+100,ny*2*step+100);
	setcolor(BLACK    ); drawrect((nx+1)*step+70,120,(nx+1)*step+100,ny*2*step+100);
	if (gLogCurTemp>0) setcolor(RED); else setcolor(CYAN);
	fillrect((nx+1)*step+70,10*scaleStep+120,(nx+1)*step+100,(10-gLogCurTemp)*scaleStep+120);
	setcolor(BLACK);
	drawtext((nx+1)*step+70,85,"log(Temp)",100);
	for(i=1;i<=19;i++) {
		drawline((nx+1)*step+60,i*scaleStep+120,(nx+1)*step+100,i*scaleStep+120);
		sprintf(label,"%d",10-i);
		drawtext((nx+1)*step+40,i*scaleStep+120,label,20);
	}

	/* draw cost scale */
	setcolor(LIGHTGREY); fillrect((nx+1)*step+140,60 ,(nx+1)*step+240,ny*2*step+120);
	setcolor(WHITE    ); fillrect((nx+1)*step+190,120,(nx+1)*step+220,ny*2*step+100);
	setcolor(BLACK    ); drawrect((nx+1)*step+190,120,(nx+1)*step+220,ny*2*step+100);
	costReduction = (float)gCurCost/(float)gInitCost;
	if (costReduction>1) setcolor(RED); else setcolor(GREEN);
	fillrect((nx+1)*step+190,10*scaleStep+120,(nx+1)*step+220,(10-10*(costReduction-1))*scaleStep+120);
	setcolor(BLACK);
	drawtext((nx+1)*step+190,75,"New Cost",100);
	drawtext((nx+1)*step+190,100,"Init Cost",100);
	drawline((nx+1)*step+150,85,(nx+1)*step+230,85);
	for(i=1;i<=19;i++) {
		drawline((nx+1)*step+180,i*scaleStep+120,(nx+1)*step+220,i*scaleStep+120);
		sprintf(label,"%1.1f",2-i*.1);
		drawtext((nx+1)*step+160,i*scaleStep+120,label,30);
	}

	/* draw footer */
	setcolor(LIGHTGREY); fillrect(0,ny*2*step+140,(nx+1)*step+240,ny*2*step+180);
	setcolor(BLACK	  ); drawtext(((nx+1)*step+240)/2,ny*2*step+160,gFooterLabel,(nx+1)*step+240);

	/* draw grid */
	fpDrawGrid(gfp,x0Grid,y0Grid,x1Grid,y1Grid);

} /* fpDraw */

/* swap the order of two cells and update the cost										*/
void		 fpSwapCells(floorplan *fp, unsigned int cellId1, unsigned int cellId2) 	{

	unsigned int tmp;

	/* swap */
	tmp = fp->cellsOrder[cellId1];
	fp->cellsOrder[cellId1] = fp->cellsOrder[cellId2];
	fp->cellsOrder[cellId2] = tmp;

	/* update cost */
	if (cellId1<(fp->cellsN)) fpCellUpdateBBox(fp,cellId1);
	if (cellId2<(fp->cellsN)) fpCellUpdateBBox(fp,cellId2);

} /* fpSwapCells */

/* simulated annealing placement 											*/
void fpAnneal(floorplan *fp, unsigned int movesPerTemp, double initTemp		,
							 double coolingRate	      , double freezingTemp	){

	unsigned int cellId1,cellId2;
	unsigned int imove;
	unsigned int maxInd = (fp->nx)*(fp->ny)-1;
	unsigned int oldCost, newCost;
	int gain;
	unsigned int stepsCnt = 0;
	unsigned int *bestOrder = arrAlloc(maxInd+1);
	unsigned int  bestCost  = fp->bbox;

	double curTemp = initTemp;

	arrCopy(fp->cellsOrder,bestOrder,maxInd+1);

	gLogCurTemp	  = log(curTemp)/log(10);
	gInitCost	  = fp->bbox    ;
	gCurCost	  = gInitCost   ;

	if (gVerbose) {
		printf("Performing simulated annealing with the following parameters:\n");
		printf("\tInitial Temperature   : %f\n",curTemp);
		printf("\tFreezing Temperature  : %f\n",freezingTemp);
		printf("\tCooling Rate          : %f\n",coolingRate);
		printf("\tMoves per Temperature : %u\n\n",movesPerTemp);
	}

	if (gGUI) {
		/* update global message and wait for 'Proceed' to proceed */
		sprintf(gFooterLabel,"Initial floorplan. Press 'Proceed' to continue.");
		sprintf(gHeaderLabel,"InitialTemp:%.1e  FreezingTemp:%.1e  CoolingRate:.%d  MovesPerTemp:%u" ,
					initTemp, freezingTemp, (int)(coolingRate*100), movesPerTemp      );
		waitLoop();
		if (gPostScript) postscript(drawScreen);
	}

	while(curTemp>freezingTemp) {

			if (stepsCnt == gRefreshRate-1) {
				if (gVerbose) printf("- Current temperature: %e, Cost: %u\n",curTemp,fp->bbox);
				if (gGUI) {
					/* update global message and wait for 'Proceed' to proceed */
					sprintf(gFooterLabel,"Current temperature: %e, Cost: %u",curTemp,fp->bbox);
					waitLoop();
					if (gPostScript) postscript(drawScreen);
				}
				stepsCnt=0;
			} else	stepsCnt++;

		for(imove=1;imove<=movesPerTemp;imove++) {
			oldCost = fp->bbox;
			/* get two diffrerent random indexes */
			get2DiffUIntRand(&cellId1,&cellId2,0,maxInd);
			fpSwapCells(fp,cellId1,cellId2);	/* swap */
			newCost = fp->bbox;
			gain = oldCost-newCost;
			if (gain < 0) {
				if (getFloatRand(0,1) >= exp(gain/curTemp))
					fpSwapCells(fp,cellId1,cellId2);	/* undo swapping by swapping again*/
			} else { /* cost is improved */
				if (newCost<bestCost) {
					arrCopy(fp->cellsOrder,bestOrder,maxInd+1);
					bestCost = newCost;
				}
			}
		}

		gCurCost	 = fp->bbox    ;
		curTemp     *= coolingRate ;
		gLogCurTemp	 = log(curTemp)/log(10);
	}
	arrCopy(bestOrder,fp->cellsOrder,maxInd+1);
	fpAllNetsUpdateBBox(fp);

	/* finished! wait still until 'Exit' is pressed */
	if (gGUI) {
		/* update global message and wait for 'Proceed' to proceed */
		sprintf(gFooterLabel,"Finished! Final cost: %u - press 'Exit'", fp->bbox);
		waitLoop();
		if (gPostScript) postscript(drawScreen);
	}
	if (gVerbose) printf("- Finished with final cost: %u\n", fp->bbox);

	safeFree(bestOrder);

} /* fpAnneal */
