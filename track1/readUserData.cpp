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

#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "readUserData.h"
#include "globals.h"
#include "Bias.h"


void readTrack1DBFromTextFiles(char* filePrefs, DB_TYPE whichDB)
{			
	MetaData* pMetaData = &(TrainingMetaData);	
	ItemRating ** ppItemRatings = &(pItemRatings_training);
	unsigned int usrId, itemId, score;
	unsigned int total_ratings = 0;
	double total_scores=0;
	switch(whichDB)
	{
	case TRAINING:		
		total_ratings = TrainingMetaData.trainingTotalRatings;
		break;
	case VALIDATION:
		total_ratings = TrainingMetaData.validationTotalRatings;		
		ppItemRatings = &(pItemRatings_validation);
		pMetaData = &(ValidationMetaData);
		break;
	case TEST:
		total_ratings = TrainingMetaData.testTotalRatings;		
		ppItemRatings = &(pItemRatings_test);
		pMetaData = &(TestMetaData);
		break;
	default:		
		assert(0);
	}	

	bool *pItemsArr = new bool[TrainingMetaData.trainingTotalItems];
	unsigned int i =0;
	for(i=0;i<TrainingMetaData.trainingTotalItems;i++)
	{
		pItemsArr[i]=false;
	}
	int prevUser = -1;
	const int MaxLen  =1<<14; //Maximum length of a single line in the input file
	char  line[MaxLen];
	char* str;

	time_t Start_t, End_t; 
	Start_t = time(NULL);	
	
	if(TRAINING == whichDB)  //In the Training DB we allocate also the users array
	{
		if(pUsersData)
		{
			delete pUsersData;
			pUsersData = 0;
		}
		pUsersData = new UserData[TrainingMetaData.trainingTotalUsers+1];
		assert(pUsersData);
	}	
	
	FILE* fp = fopen(filePrefs,"rt");
	if (fp==NULL) {
		fprintf(stderr, "Cannot open: %s\n", filePrefs);
		exit(1);
	}
			
	if(*ppItemRatings)
	{
		delete *ppItemRatings;
		*ppItemRatings = 0;
	}
	(*ppItemRatings) = new ItemRating[total_ratings];
	cout<<"Allocated "<<sizeof(ItemRating)*((float)total_ratings/(1<<20))<<" Mega-Bytes!!"<<endl;
	usrId = 0;	
	int prevMaxItemId=0;	

	//Run on users:
	while (fgets(line, MaxLen, fp)) {

		// Get user id:
		str = strtok(line,"|");
		usrId = atoi(str);		
		if((prevUser+1) != (usrId))
		{
			cout<<"Users are un-ordered!! previous user was:"<<prevUser<<" this user is:"<<usrId<<endl;
			return;
		}
		prevUser = usrId;
		pMetaData->nUsers++;

		// Get number of ratings:
		unsigned int nUserRatings;
		str = strtok(NULL,"|");
		nUserRatings = atoi(str);
		if(TRAINING == whichDB)
		{			
			assert(nUserRatings>0);
			pUsersData[usrId].ratings = nUserRatings;	
		}
		else if(VALIDATION == whichDB)
		{
			assert(RATINGS_PER_USER_VALIDATION==nUserRatings);
		}
		else if(TEST == whichDB)
		{
			assert(RATINGS_PER_USER_TEST==nUserRatings);
		}
		else
		{
			assert(0);
		}						
		
		//Read the user's ratings:
		for(unsigned int rating_idx=0; rating_idx<nUserRatings; rating_idx++)
		{
			fgets(line, MaxLen, fp);									

			//Get the item ID:			
			str = strtok(line,"\t");
			itemId = atoi(str);
			assert(itemId<TrainingMetaData.trainingTotalItems);
			pItemsArr[itemId]=1;
			
			(*ppItemRatings)[pMetaData->nRecords].item = itemId;

			if(TEST != whichDB)
			{
				//Get the score:
				str = strtok(NULL,"\t");
				score = atoi(str);
				assert((score>=0)&&(score<=100));			
				(*ppItemRatings)[pMetaData->nRecords].score = score;
				total_scores+=score;
			}
			else
			{
				(*ppItemRatings)[pMetaData->nRecords].score = 0;
			}
									
			//Get the day and time:
			char *dayStr = 0; char *timeStr = 0; char *hourStr = 0; char *minuteStr = 0;
			unsigned int day=0, hour=0, minute=0;
			
			dayStr    = strtok(NULL,"\t");
			day = atoi(dayStr);

			timeStr    = strtok(NULL,"\t");

			hourStr    = strtok(timeStr,":");
			hour = atoi(hourStr);
			assert((hour>=0)&&(hour<=23));			

			minuteStr  = strtok(NULL,":");
			minute = atoi(minuteStr);
			assert((minute>=0)&&(minute<=59));									
			
			pMetaData->nRecords++;		 //Counts the number of records (ratings)			
		}
	}
		
	fclose(fp);
	pMetaData->totalMeanScore = total_scores/pMetaData->nRecords;

	//Update MetData:
	for(unsigned int i=0; i<TrainingMetaData.trainingTotalItems; i++)
	{
		pMetaData->nItems+=pItemsArr[i];
	}

	End_t = time(NULL);   
	cout<<"Done reading ratings!  Elapsed time (secs): "<<difftime(End_t, Start_t)<<endl;
	cout<<"----  SUMMARY:  ----"<<endl
		<<" Users:"<<pMetaData->nUsers<<" Records:"<<pMetaData->nRecords<<" Items:"<<pMetaData->nItems<<endl		
		<<" Mean score:"<<pMetaData->totalMeanScore<<endl;		
	return;
}


void writeTrack1DBIntoBinFile(DB_TYPE whichDB)
{
	FILE *FP = 0;
	MetaData* pMetaData = &(TrainingMetaData);
	ItemRating*   pItemRatings = pItemRatings_training;
	switch(whichDB)
	{
	case TRAINING:
		FP = fopen(TRACK1_TRAINING_DB_FILE, "wb");
		assert(FP);
		break;
	case VALIDATION:
		pMetaData = &(ValidationMetaData);
		pItemRatings = pItemRatings_validation;
		FP = fopen(TRACK1_VALIDATION_DB_FILE, "wb");
		assert(FP);
		break;
	case TEST:
		pMetaData = &(TestMetaData);
		pItemRatings = pItemRatings_test;
		FP = fopen(TRACK1_TEST_DB_FILE, "wb");
		assert(FP);
		break;
	default:		
		assert(0);
	}

	//Write meta-data:	
	unsigned int recordsWritten = 0;
	recordsWritten = fwrite(pMetaData, sizeof(MetaData),1, FP);	
	assert(1 == recordsWritten);

	if(TRAINING == whichDB)
	{
		//Write users data:	
		recordsWritten = 0; 	
		recordsWritten = fwrite(pUsersData, sizeof(UserData),(1+TrainingMetaData.trainingTotalUsers),FP);	
		assert((1+TrainingMetaData.trainingTotalUsers) == recordsWritten);		
	}

	//Write ratings data:
	recordsWritten = 0; 		
	recordsWritten = fwrite(pItemRatings, sizeof(ItemRating),pMetaData->nRecords, FP);
	assert(pMetaData->nRecords == recordsWritten);

	fclose(FP);	
	cout<<"---- WROTE RATING DATA INTO FILE ----"<<endl;
	return;
}

bool readTrack1DBFromBinFile(DB_TYPE whichDB)
{		
	FILE *FP = 0;	
	MetaData* pMetaData = &(TrainingMetaData);
	ItemRating**   ppItemRatings = &(pItemRatings_training);
	switch(whichDB)
	{
	case TRAINING:		
		FP = fopen(TRACK1_TRAINING_DB_FILE, "rb");		
		break;
	case VALIDATION:
		pMetaData = &(ValidationMetaData);
		ppItemRatings = &(pItemRatings_validation);
		FP = fopen(TRACK1_VALIDATION_DB_FILE, "rb");		
		break;
	case TEST:
		pMetaData = &(TestMetaData);
		ppItemRatings = &(pItemRatings_test);
		FP = fopen(TRACK1_TEST_DB_FILE, "rb");
		break;
	default:	
		assert(0);
	}
	if(!FP)
	{
		return false;
	}	

	//Reading meta-data:	
	unsigned int recordsRead = 0; 	
	MetaData metaTemp = {0};
	recordsRead = fread(&(metaTemp), sizeof(MetaData),1, FP);	
	(*pMetaData) = metaTemp;
	assert(pMetaData->nRecords);
	assert(pMetaData->nItems);
	assert(pMetaData->nUsers);
	
	if(TRAINING == whichDB)
	{
		//Reading users data:
		assert(!pUsersData);
		pUsersData = new UserData[(TrainingMetaData.trainingTotalUsers+1)];
		recordsRead = 0; 	
		recordsRead = fread(pUsersData,sizeof(UserData),(TrainingMetaData.trainingTotalUsers+1),FP);	
		assert((TrainingMetaData.trainingTotalUsers+1) == recordsRead);
	}

	//Reading ratings:
	assert(!*(ppItemRatings));
	*(ppItemRatings) = new ItemRating[pMetaData->nRecords];
	assert(*(ppItemRatings));
	recordsRead = 0; 	
	recordsRead = fread(*(ppItemRatings), sizeof(ItemRating),pMetaData->nRecords, FP);
	fclose(FP);				
	return true;
}

bool allBinaryFilesExist()
{
	struct stat stFileInfo;
	int returnVal=0;
	returnVal = stat(TRACK1_TRAINING_DB_FILE,&stFileInfo);
	if(returnVal)
		return false;
	returnVal = stat(TRACK1_VALIDATION_DB_FILE,&stFileInfo);
	if(returnVal)
		return false;
	returnVal = stat(TRACK1_TEST_DB_FILE,&stFileInfo);
	if(returnVal)
		return false;

	return true;
}

bool readAllTrack1BinData()
{		
	if(!allBinaryFilesExist())
	{
		return false;
	}

	if(!readTrack1DBFromBinFile(TRAINING))
	{		
		return false;
	}	

	
	if(!readTrack1DBFromBinFile(VALIDATION))
	{
		return false;
	}	
	
	if(!readTrack1DBFromBinFile(TEST))
	{
		return false;
	}
	
	return true;
}


void freeAll()
{	
	if(pItemRatings_training)
	{
		delete pItemRatings_training;
		pItemRatings_training = 0;
	}	
	if(pUsersData)
	{
		delete pUsersData;
		pUsersData = 0;
	}
	if(pItemRatings_validation)
	{
		delete pItemRatings_training;
		pItemRatings_training = 0;
	}
	return;
}


void readStats(char * statsFile)
{
	const int MaxLen  =1<<14; //Maximum length of a single line in the input file 		
	char  line[MaxLen], *pStr;	

	FILE* fp = fopen(statsFile,"rt");
	if (fp==NULL) 
	{
		fprintf(stderr, "Cannot open: %s\n", statsFile);
		exit(1);
	}

	while (fgets(line, MaxLen, fp)) 
	{
		pStr=strtok(line,"=");
		cout<<pStr<<"=";

		if(!strcmp("nUsers",pStr))
		{
			pStr=strtok(NULL,"=");
			TrainingMetaData.trainingTotalUsers = atoi(pStr);
			cout<<TrainingMetaData.trainingTotalUsers<<endl;
		}
		else if(!strcmp("nItems",pStr))
		{
			pStr=strtok(NULL,"=");
			TrainingMetaData.trainingTotalItems = atoi(pStr);
			cout<<TrainingMetaData.trainingTotalItems<<endl;
		}
		else if(!strcmp("nRatings",pStr))
		{
			pStr=strtok(NULL,"=");
			TrainingMetaData.DBallRatings = atoi(pStr);
			cout<<TrainingMetaData.DBallRatings<<endl;
		}
		else if(!strcmp("nTrainRatings",pStr))
		{
			pStr=strtok(NULL,"=");
			TrainingMetaData.trainingTotalRatings = atoi(pStr);
			cout<<TrainingMetaData.trainingTotalRatings<<endl;
		}
		else if(!strcmp("nProbeRatings",pStr))
		{
			pStr=strtok(NULL,"=");
			TrainingMetaData.validationTotalRatings = atoi(pStr);
			cout<<TrainingMetaData.validationTotalRatings<<endl;
		}
		else if(!strcmp("nTestRatings",pStr))
		{
			pStr=strtok(NULL,"=");
			TrainingMetaData.testTotalRatings = atoi(pStr);
			cout<<TrainingMetaData.testTotalRatings<<endl;
		}		
	}
	fclose(fp);
}
