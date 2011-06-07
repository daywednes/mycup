// Copyright 2011 Yahoo!. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
//    1. Redistributions of source code must retain the above copyright notice, this list of
//       conditions and the following disclaimer.
// 
//    2. Redistributions in binary form must reproduce the above copyright notice, this list
//       of conditions and the following disclaimer in the documentation and/or other materials
//       provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY Yahoo! ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Yahoo! OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// The views and conclusions contained in the software and documentation are those of the
// authors and should not be interpreted as representing official policies, either expressed


#ifndef _READ_USER_DATA_H_
#define _READ_USER_DATA_H_

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#include "ratingStructures.h"
#include "globals.h"

using namespace std;
#pragma warning(disable : 4996)  //This warning pops each time I call: fopen

void readStats(char* statsFile);

bool readAllTrack1BinData();

void readTrack1DBFromTextFiles(char* filePrefs, DB_TYPE whichDB);

void writeTrack1DBIntoBinFile(DB_TYPE whichDB);

bool readTrack1DBFromBinFile(DB_TYPE whichDB);

bool allBinaryFilesExist();

void freeAll();

#endif


