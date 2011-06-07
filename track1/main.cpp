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


#include <math.h>
#include <stdio.h>
#include <time.h>
#include <ctime>
#include "readUserData.h"
#include "globals.h"
#include "Bias.h"
#include "ratingStructures.h"

int main(int argc, char *argv[]) 
{			
	double trainingRmse=0, validationRmse=0;
	int iterCount=-1, i=0;	
	time_t Start_t, End_t;
	unsigned int tmp = 0;

	cout<<"Testing if a binary dataset already exists..."<<endl;

	if(!readAllTrack1BinData())
	{
		cout<<"Couldn't find the binary dataset files => Creating a binary dataset from text files..."<<endl;			
		
		cout<<"-----------------  LOADING STATISTICS -----------------"<<endl;		
		readStats(TRACK1_STATS_FILE);

		cout<<"-----------------  LOADING TRAINING DATA -----------------"<<endl;
		readTrack1DBFromTextFiles(TRACK1_TRAINING_FILE,TRAINING);	
		writeTrack1DBIntoBinFile(TRAINING);
				
		cout<<endl<<"-----------------  LOADING VALIDATION DATA -----------------"<<endl;
		readTrack1DBFromTextFiles(TRACK1_VALIDATION_FILE,VALIDATION);	
		writeTrack1DBIntoBinFile(VALIDATION);

		cout<<endl<<"-----------------  LOADING TEST DATA -----------------"<<endl;
		readTrack1DBFromTextFiles(TRACK1_TEST_FILE,TEST);	
		writeTrack1DBIntoBinFile(TEST);
	}
	else
	{
		cout<<"Loaded binary dataset files correctly!"<<endl;
	}

	
	cout<<endl<<"---------"<<endl;
	Start_t = time(NULL);
	gradientDescent(iterations,trainingRmse,validationRmse,iterCount);
	End_t = time(NULL);		
	cout<<endl<<"---------"<<endl;
			
	cout<<"Training RMSE: "<<trainingRmse<<"\tValidation RMSE: "<<validationRmse<<", secs:"<<(difftime(End_t, Start_t))<<endl;					
	
	cout<<endl<<"---------"<<endl;
	predictTrack1TestRatings(TRACK_1_RESULTS_FILE);

	freeAll();
				
	return 0;
}

