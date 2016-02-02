#############################################################################################
##     Copyright (c) 2011, University of British Columbia (UBC); All rights reserved.      ##
##                                                                                         ##
##     Redistribution  and  use  in  source   and  binary  forms,   with  or  without      ##
##     modification,  are permitted  provided that  the following conditions are met:      ##
##       * Redistributions   of  source   code  must  retain   the   above  copyright      ##
##         notice,  this   list   of   conditions   and   the  following  disclaimer.      ##
##       * Redistributions  in  binary  form  must  reproduce  the  above   copyright      ##
##         notice, this  list  of  conditions  and the  following  disclaimer in  the      ##
##         documentation and/or  other  materials  provided  with  the  distribution.      ##
##       * Neither the name of the University of British Columbia (UBC) nor the names      ##
##         of   its   contributors  may  be  used  to  endorse  or   promote products      ##
##         derived from  this  software without  specific  prior  written permission.      ##
##                                                                                         ##
##     THIS  SOFTWARE IS  PROVIDED  BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"      ##
##     AND  ANY EXPRESS  OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE      ##
##     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE      ##
##     DISCLAIMED.  IN NO  EVENT SHALL University of British Columbia (UBC) BE LIABLE      ##
##     FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL      ##
##     DAMAGES  (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR      ##
##     SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER      ##
##     CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN CONTRACT, STRICT LIABILITY,      ##
##     OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE      ##
##     OF  THIS SOFTWARE,  EVEN  IF  ADVISED  OF  THE  POSSIBILITY  OF  SUCH  DAMAGE.      ##
#############################################################################################

#############################################################################################
##         makefile:  make file for simulated-annealing cell-based placement tool          ##
##                                                                                         ##
## source    : annealing.c allocation.c array.c random.c geometry.c graphics.c floorplan.c ##
## headers   :             allocation.h array.h random.h geometry.h graphics.h floorplan.h ##
## executable: annealing                                                                   ##
##                                                                                         ##
## invoke 'make' to generate binary and 'make clean' to clean objects                      ##
##                                                                                         ##
##                                                                                         ##
##        Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)         ##
##                    University of British Columbia (UBC), March 2011                     ##
#############################################################################################


CC = gcc
LIB = -lX11 -lm
FLG = -Wall -O2

EXE = annealing
OBJ = annealing.o allocation.o array.o random.o geometry.o graphics.o floorplan.o
SRC = annealing.c allocation.c array.c random.c geometry.c graphics.c floorplan.c
HDR = 			  allocation.h array.h random.h geometry.h graphics.h floorplan.h

$(EXE)		: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(EXE) $(LIB)

graphics.o	: graphics.c	$(HDR)
	$(CC) -c $(FLG) graphics.c
allocation.o: allocation.c	$(HDR)
	$(CC) -c $(FLG) allocation.c
array.o		: array.c		$(HDR)
	$(CC) -c $(FLG) array.c
random.o	: random.c		$(HDR)
	$(CC) -c $(FLG) random.c
geometry.o	: geometry.c	$(HDR)
	$(CC) -c $(FLG) geometry.c
floorplan.o	: floorplan.c	$(HDR)
	$(CC) -c $(FLG) floorplan.c
annealing.o	: annealing.c	$(HDR)
	$(CC) -c $(FLG) annealing.c

.PHONY: clean
clean:
	-rm -f $(OBJ)

.PHONY: clean~
clean~:
	-rm -f *~
