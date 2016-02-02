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
** geometry.c:  geometry module	                                                  **
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
#include "array.h"
#include "allocation.h"
#include "geometry.h"

/* change 2D index (ix,iy) in (nx,ny) size world into 1D linear world index				*/
unsigned int index2Dto1D(unsigned int ix,unsigned int iy,unsigned int nx,unsigned int ny){
	if ((ix>=nx)||(iy>=ny)) {
		printf("-W- Geometry: Out of range index passed to index2Dto1D\n");
		return UINT_MAX;
	}
	return iy*nx+ix;
}

/* change 1D linear world index ix into 2D index in (nx,ny) size world					*/
point		 index1Dto2D(unsigned int ix				,unsigned int nx,unsigned int ny){
	point index2D;
	if (ix >= (nx*ny)) {
		index2D.x = UINT_MAX;
		index2D.y = UINT_MAX;
		printf("-W- Geometry: Out of range index passed to index1Dto2D\n");
		return index2D;
	}
	index2D.x = ix%nx;
	index2D.y = ix/nx;
	return index2D;
}

/* returns bounding box half perimeter of points list									*/
unsigned int pointsBBoxHalfPerimeter(point *points, unsigned int pointsNum				){
	unsigned int pointi;
	unsigned int dx,dy ;
	unsigned int *xs = arrAlloc(pointsNum);
	unsigned int *ys = arrAlloc(pointsNum);
	for(pointi=0; pointi<pointsNum; pointi++) {
		xs[pointi] = points[pointi].x;
		ys[pointi] = points[pointi].y;
	}
	dx = arrMax(xs,pointsNum)-arrMin(xs,pointsNum);
	dy = arrMax(ys,pointsNum)-arrMin(ys,pointsNum);
	safeFree(xs);
	safeFree(ys);

	return	dx+dy;
}
