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


#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "globals.h"
#include "Bias.h"


void gradientDescent(int nIterations, double & correspondingTrainingRmse, double & bestValidationRmse, int & iterCount)
{	
	time_t Start_t, End_t; 	
	double trainingRmse = 999999, validationRmse = 999999,  prevValidationRmse = 999999;
	unsigned int nFaults=0;
	bestValidationRmse = 999999; correspondingTrainingRmse = 999999;
	double err, estScore;
	unsigned int user, i=0;
	double sqErrSum;
	double totalMse;	

	cout<<"Statring gradientDescent... (mean score is:"<<TrainingMetaData.totalMeanScore<<")"<<endl;
	BiasManager.allocate();
				
	//Iterate on training data:
	for (iterCount=0; iterCount<nIterations; iterCount++) //Run for nIterations iterations
	{		
		sqErrSum = 0;		
		unsigned int currentRatingIdx = 0;
		Start_t = time(NULL);
		for (user=0; user<TrainingMetaData.nUsers; user++) //Run on users
		{
			unsigned int userRatings = pUsersData[user].ratings;
			assert(userRatings);
			for (i=0; i<userRatings; i++) //Run on a user's items
			{
				// Compute error:				
				estScore =  estimate(pItemRatings_training[currentRatingIdx],user);
				estScore = min(estScore,(double)100);
				estScore = max(estScore,(double)0);
				err = pItemRatings_training[currentRatingIdx].score-estScore;								
				sqErrSum += err*err;
				
				// Change coefficients along computed gradient:
				update(pItemRatings_training[currentRatingIdx],user,pItemRatings_training[currentRatingIdx].item,err);				
				currentRatingIdx++;
			}
		}

		trainingRmse = sqrt(sqErrSum/TrainingMetaData.nRecords);		
		End_t = time(NULL);
		cout<<"Sweep: "<<iterCount+1<<" trainingRMSE="<<trainingRmse;		
		
		//Iterate on validation data:
		totalMse=0;
		currentRatingIdx = 0;
		for (user=0; user<ValidationMetaData.nUsers; user++) 
		{							
			for (i=0; i<RATINGS_PER_USER_VALIDATION; i++) 
			{
				// compute error:				
				estScore = estimate(pItemRatings_validation[currentRatingIdx],user);
				estScore = min(estScore,(double)100);
				estScore = max(estScore,(double)0);
				err = pItemRatings_validation[currentRatingIdx].score-estScore;
				totalMse += err*err;
				currentRatingIdx++;				
			}			
		}					
		validationRmse = sqrt(totalMse/ValidationMetaData.nRecords);
		cout<<"\t\ttvalidationRMSE="<<validationRmse<<"\t\t("<<difftime(End_t, Start_t)<<" secs)"<<endl;		
		
		if (validationRmse>=prevValidationRmse) 
		{
			nFaults++;
			if (nFaults>GRADIENT_DESCENT_FAULTS) 
			{
				cout<<"Early termination since current Validation RMSE ("<<validationRmse<<") is higher than prev. best ("<<bestValidationRmse<<") (number of faults: "<<nFaults<<")"<<endl;
				iterCount -= (GRADIENT_DESCENT_FAULTS);
				break;
			}
		}
		else
		{
			nFaults = 0;
			bestValidationRmse		  = validationRmse;
			correspondingTrainingRmse = trainingRmse;	
			//Reduce steps size:
			itemsStep				*= REDUCE_STEP_FACTOR;
			usersStep				*= REDUCE_STEP_FACTOR;			
		}
		prevValidationRmse = validationRmse;						
	}	
}

CBiasManager::CBiasManager()
{		
	pItemsBase=0;
	pUsersBase=0;	
}

CBiasManager::~CBiasManager()
{
	if(pItemsBase)
	{
		delete pItemsBase;
		pItemsBase = 0;
	}
	if(pUsersBase)
	{
		delete pUsersBase;
		pUsersBase = 0;
	}	
	return;
}

bool CBiasManager::allocateOrCleanBaseVector(float** ppBase, unsigned int length)
{
	unsigned int i;
	if(!(*ppBase))
	{
		(*ppBase) = new float[length]; 
		assert(*ppBase);
	}
	//Clear all values:
	for(i=0;i<length;i++)
	{
		(*ppBase)[i]=0;
	}
	return true;
}

void CBiasManager::allocate()
{			
	allocateOrCleanBaseVector(&pUsersBase,TrainingMetaData.trainingTotalUsers);
	allocateOrCleanBaseVector(&pItemsBase,TrainingMetaData.trainingTotalItems);	
}


void predictTrack1TestRatings(char * filename)
{
	unsigned int user=0, i=0, currentRatingIdx=0;
	double estScore=0;
	cout<<"Predicting Track1 TEST data into: "<<filename<<endl;
	FILE * fp = fopen(filename, "wt");
	for (user=0; user<TestMetaData.nUsers; user++) 
	{		
		for (i=0; i<RATINGS_PER_USER_TEST; i++) 
		{	
			estScore = estimate(pItemRatings_test[currentRatingIdx],user);
			estScore = min(estScore,(double)100);
			estScore = max(estScore,(double)0);				
			currentRatingIdx++;
			fprintf(fp, "%lf\n", estScore);
		}
	}
	assert(TestMetaData.nRecords == currentRatingIdx);				
	fclose(fp);
	cout<<"Done!"<<endl;
	return;
}
