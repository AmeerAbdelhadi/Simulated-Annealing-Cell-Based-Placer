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
** array.h:  unsigned integer array module                                        **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**               University of British Columbia (UBC), March 2011                 **
***********************************************************************************/


#ifndef __MATRIX_HEADER__
#define __MATRIX_HEADER__

/* return memory allocate unsigned int array; 'arrSize' size							*/
unsigned int* arrAlloc(unsigned int arrSize												);

/* returns index of minimum number														*/
unsigned int  arrMinInd		(unsigned int *arr, unsigned int arrSize					);

/* returns index of maximum number 														*/
unsigned int  arrMaxInd		(unsigned int *arr, unsigned int arrSize					);

/* returns minimum number of an array													*/
unsigned int  arrMin(unsigned int *arr, unsigned int arrSize							);

/* returns index of maximum number of an array											*/
unsigned int  arrMax(unsigned int *arr, unsigned int arrSize							);

/* returns the index of the only positive value (all others 0), else return UINT_MAX	*/
unsigned int  arrOnePositiveInd(unsigned int *arr, unsigned int arrSize					);

/* copy array values from source array to target array									*/
void		  arrCopy(unsigned int *source, unsigned int *target,unsigned int arrSize	);

/* add values in first array to second array											*/
void		  arrAdd(unsigned int *arr, unsigned int *acc,unsigned int arrSize			);

/* initialize all array elements to initVal value										*/
void		  arrInit(unsigned int *arr, unsigned int initVal, unsigned int arrSize		);

/* generate a random numbers array between minRand and maxRand with different number	*/
void		arrRandInit(	unsigned int *arr    , unsigned int  arrSize	,
							unsigned int  minRand, unsigned int  maxRand				);

/* dump array's values to stdout delimited with 'delimiter' string						*/
void		  arrDump(unsigned int *arr, unsigned int arrSize, char * delimiter			);

/* return 1 if 'value' is found in the array between fromInd to toInd, inclusive		*/
int			  arrValueFound(unsigned int *arr, unsigned int arrSize, unsigned int val	,
					        unsigned int fromInd, unsigned int toInd					);

#endif /* __MATRIX_HEADER__ 															*/
