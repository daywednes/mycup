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


#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "ratingStructures.h"


//Gradient Descent params:
extern int    iterations;	//# of iterations in SGD
extern double itemsStep;	//Step size of item biases in SGD
extern double itemsReg;		//Regularization parameter for item biases in SGD
extern double usersStep;	//Step size of user biases in SGD
extern double usersReg;		//Regularization parameter for user biases in SGD


//Binary files names:
#define TRACK1_TRAINING_DB_FILE     "TrainDB.track1.dat"
#define TRACK1_VALIDATION_DB_FILE   "ValidationDB.track1.dat"
#define TRACK1_TEST_DB_FILE		    "TestDB.track1.dat"

//Text file names:
#define TRACK1_TRAINING_FILE     "trainIdx1.txt"
#define TRACK1_VALIDATION_FILE   "validationIdx1.txt"
#define TRACK1_TEST_FILE		 "testIdx1.txt"
#define TRACK1_STATS_FILE		 "stats1.txt"
#define TRACK_1_RESULTS_FILE	 "track1_results.txt"


//Input files Constants: 
#define	RATINGS_PER_USER_VALIDATION		 4
#define	RATINGS_PER_USER_TEST			 6

//Algorithm constants:
#define GRADIENT_DESCENT_FAULTS		2		//Number of SGD faults on the validation dataset
#define REDUCE_STEP_FACTOR			0.7f	//Step size reduction factor in SGD

/*
 * This is a simple data for counting the number of ratings each user has:
 */
struct UserData
{
	unsigned int ratings			:20;		
	UserData():ratings(0){};
};

extern ItemRating     *pItemRatings_training;
extern UserData		  *pUsersData;

//Validation and Test:
extern ItemRating     *pItemRatings_validation;		  //Probing Ratings
extern ItemRating     *pItemRatings_test;			  //Test Ratings

//Global structures and enums:
extern struct MetaData
{
	unsigned int nUsers;
	unsigned int nRecords;
	unsigned int nItems;	
	double		 totalMeanScore;	

	//Stats file data:
	unsigned int trainingTotalUsers;
	unsigned int trainingTotalItems;
	unsigned int DBallRatings;
	unsigned int trainingTotalRatings;
	unsigned int validationTotalRatings;
	unsigned int testTotalRatings;
} TrainingMetaData, ValidationMetaData, TestMetaData;

typedef enum
{
	TRAINING = 0,
	VALIDATION,
	TEST
} DB_TYPE;


#endif

