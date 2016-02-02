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
** random.c:  random numbers generator module                                     **
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
#include "random.h"

/* returns a random number between minRand and maxRand [minRand,maxRand]				*/
unsigned int getUIntRand(unsigned int minRand, unsigned int maxRand						){
	/* random()								gives a random number in [0,RAND_MAX]		*/
	/* random()%maxRand						gives a random number between [0,maxRand-1]	*/
	/* random()%(maxRand-minRan+1)			gives a random number in [0,maxRand-minRand]*/
	/* random()%(maxRand-minRan+1)+minRand	gives a random number in [minRand,maxRand]	*/
	return ( random() % (maxRand - minRand + 1) ) + minRand;
}

/* returns a random number between minRand and maxRand [minRand,maxRand]				*/
float		 getFloatRand(float minRand, float maxRand									){
  /* random()										random number in [0,RAND_MAX]		*/
  /* random()/RAND_MAX								random number between [0,1]			*/
  /* (random()/RAND_MAX)*(maxRand-minRand)			random number in [0,maxRand-minRand]*/
  /* (random()/RAND_MAX)*(maxRand-minRand)+minRand	random number in [minRand,maxRand]	*/
  return ((float)random()/RAND_MAX)*(maxRand-minRand)+minRand;
}

/* returns two different random numbers between minRand and maxRand [minRand,maxRand]	*/
void		 get2DiffUIntRand(	unsigned int *rand1  , unsigned int *rand2				,
							unsigned int  minRand, unsigned int  maxRand				){
	(*rand1) = getUIntRand(minRand,maxRand);
	while (((*rand2)=getUIntRand(minRand,maxRand))==(*rand1)); /* until different		*/
}
