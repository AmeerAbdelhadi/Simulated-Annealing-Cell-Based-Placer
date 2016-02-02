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
** array.c:  unsigned integer array module                                        **
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
#include "array.h"
#include "allocation.h"
#include "random.h"

/* return memory allocate unsigned int array; 'arrSize' size */
unsigned int* arrAlloc(unsigned int arrSize) {
	return (unsigned int*)safeMalloc(sizeof(unsigned int)*arrSize);
}

/* returns index of minimum number of an array */
unsigned int arrMinInd(unsigned int *arr, unsigned int arrSize) {
		unsigned int i;
		unsigned int min=arr[0];
		unsigned int minind=0;
		for (i=0;i<arrSize;i++) {
			if (arr[i]<min) { /* new minimum */
				min=arr[i];
				minind=i;
			}
		}
		return minind;
}

/* returns index of maximum number of an array */
unsigned int arrMaxInd(unsigned int *arr, unsigned int arrSize) {
		unsigned int i;
		unsigned int max=arr[0];
		unsigned int maxind=0;
		for (i=0;i<arrSize;i++) {
			if (arr[i]>max) { /* new maximum */
				max=arr[i];
				maxind=i;
			}
		}
		return maxind;
}

/* returns minimum number of an array */
unsigned int arrMin(unsigned int *arr, unsigned int arrSize) {
		unsigned int i;
		unsigned int min=arr[0];
		for (i=0;i<arrSize;i++) {
			if (arr[i]<min) min=arr[i]; /* new minimum */
		}
		return min;
}

/* returns index of maximum number of an array */
unsigned int arrMax(unsigned int *arr, unsigned int arrSize) {
		unsigned int i;
		unsigned int max=arr[0];
		for (i=0;i<arrSize;i++) {
			if (arr[i]>max) max=arr[i]; /* new maximum */
		}
		return max;
}

/* returns the index of the only positive value (when all others are zero), else return UINT_MAX */
unsigned int arrOnlyPositiveInd(unsigned int *arr, unsigned int arrSize) {
	unsigned int i					;
	unsigned int foundInd = UINT_MAX;
	unsigned int found    = 0		;
	for(i=0;i<arrSize;i++) {
		if ((arr[i])>0) {
			if (found)	return UINT_MAX;	/* another positive found */
			else {							/* first positive found   */
				found=1;
				foundInd=i;
			}
		}
	} /* for i */
	if (!found)	return UINT_MAX; /* no positive numbers 					 */
	else		return foundInd; /* return index of the only positive number */
}

/* copy array values from source array to target array								*/
void arrCopy(unsigned int *source, unsigned int *target,unsigned int arrSize		){
	unsigned int i;
	for (i=0;i<arrSize;i++) target[i]=source[i]; /* copy source to target element	*/
}


/* add values in first array to second array */
void arrAdd(unsigned int *arr, unsigned int *acc,unsigned int arrSize) {
	unsigned int i;
	for (i=0;i<arrSize;i++) acc[i]+=arr[i]; /* add arr to acc element by element */
}

/* initialize all array elements to initVal value */
void arrInit(unsigned int *arr, unsigned int initVal, unsigned int arrSize) {
	int i;
	for (i=0;i<arrSize;i++) arr[i]=initVal; /* init all arr elements to initVal */
}


/* generate a random numbers array between minRand and maxRand with different number	*/
void		arrRandInit(	unsigned int *arr    , unsigned int  arrSize	,
							unsigned int  minRand, unsigned int  maxRand	){
	unsigned int randNum, i;
	for(i=0; i<arrSize; i++) {
		randNum = getUIntRand(minRand,maxRand);
		while (arrValueFound(arr,arrSize,randNum,0,i-1)) /* until not found previously	*/
			randNum = getUIntRand(minRand,maxRand);			 /* generate new number		*/
		arr[i] = randNum;
	}
}

/* return 1 if 'value' is found in the array between fromInd to toInd, 0 otherwise		*/
int arrValueFound(	unsigned int *arr    , unsigned int arrSize  , unsigned int val,
					unsigned int  fromInd, unsigned int toInd							){
	unsigned int i;
	if ((fromInd>toInd)||(toInd>=arrSize)) return 0;
	for(i=fromInd; i<=toInd; i++) {
		if (arr[i] == val) return 1;
	}
	return 0;
}

/* dump array's values to stdout delimited with 'delimiter' string     */
void arrDump(unsigned int *arr, unsigned int arrSize, char * delimiter)	{
	unsigned int i;
	for (i=0;i<arrSize;i++) {
		printf("%u",arr[i]);
		if (i != arrSize-1) /* not last element */
			printf("%s",delimiter);
	}
}
